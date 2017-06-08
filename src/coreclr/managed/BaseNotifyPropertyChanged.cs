/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:BaseNotifyPropertyChanged.cs
****/
using System;
using System.ComponentModel;

namespace Microsoft.PropertyModel
{
    //public interface INotifyPropertyChangedAdapter : IObjectAdapter,
    //    INotifyPropertyChanged
    //{
    //}

    public class PropertyChangedEventArgs :
        System.ComponentModel.PropertyChangedEventArgs
    {
        private PropertyChangedEventArgsAdapter EventArgsAdapter;
        private IFactoryObject FactoryObject;

        internal PropertyChangedEventArgs(
            PropertyChangedEventArgsAdapter eventArgsAdapter,
            IFactoryObject factoryObject) :
            base(eventArgsAdapter.PropertyName)
        {
            this.EventArgsAdapter = eventArgsAdapter;
            this.FactoryObject = factoryObject;
        }

        public uint Property { get { return this.EventArgsAdapter.Property; } }
        public object OldValue { get { return this.FactoryObject.ToFactoryObject(this.EventArgsAdapter.OldValue); } }
        public object NewValue { get { return this.FactoryObject.ToFactoryObject(this.EventArgsAdapter.NewValue); } }
    }

    public abstract class BaseNotifyPropertyChanged<TAdapter> : Object<TAdapter>,
        INotifyPropertyChanged
        where TAdapter : ObjectAdapter
    {
        private event System.ComponentModel.PropertyChangedEventHandler PropertyChangedEventHandler;
        private bool IsPropertyChangedCallback { get; set; }

        internal BaseNotifyPropertyChanged(TAdapter adapter, ClassFactory classFactory) :
            base(adapter, classFactory)
        {
        }


        #region INotifyPropertyChanged Members

        /// <summary>
        /// PropertyChanged Event
        /// </summary>
        public event System.ComponentModel.PropertyChangedEventHandler PropertyChanged
        {
            add
            {
                this.PropertyChangedEventHandler += value;
                this.OnPropertyChangedChanged(true);
            }
            remove
            {

                this.PropertyChangedEventHandler -= value;
                this.OnPropertyChangedChanged(false);
            }
        }

        #endregion

        protected abstract ObservableObjectAdapter PropertyModelAdapter { get; }

        private void OnPropertyChangedChanged(bool add)
        {
            int countHandlers = this.CountPropertyChangedDelegates();
            if (add && countHandlers == 1 && !this.IsPropertyChangedCallback)
            {
                this.PropertyModelAdapter.AddNotifyPropertyChanged(OnPropertyChanged);
                this.IsPropertyChangedCallback = true;
            }
            else if (!add && countHandlers == 0 && this.IsPropertyChangedCallback)
            {
                this.PropertyModelAdapter.RemoveNotifyPropertyChanged(OnPropertyChanged);
                this.IsPropertyChangedCallback = false;
            }
        }

        private int CountPropertyChangedDelegates()
        {
            return this.PropertyChangedEventHandler != null ?
                this.PropertyChangedEventHandler.GetInvocationList().Length :
                0;
        }

        /// <summary>
        /// Raise PropertyChanged
        /// </summary>
        /// <param name="propertyName"></param>
        internal void OnPropertyChanged(string propertyName)
        {
            RaisePropertyChanged(() => { return new System.ComponentModel.PropertyChangedEventArgs(propertyName); });
        }

        /// <summary>
        /// Raise PropertyChanged based on a enum
        /// </summary>
        /// <param name="propertyId"></param>
        internal void OnPropertyChanged(Enum propertyId)
        {
            OnPropertyChanged(propertyId.ToString());
        }

        /// <summary>
        /// Raise a Property Changed Event with a custom Event Args
        /// </summary>
        /// <param name="createEventArgs"></param>
        internal void RaisePropertyChanged(Func<System.ComponentModel.PropertyChangedEventArgs> createEventArgs)
        {
            System.ComponentModel.PropertyChangedEventHandler handler = this.PropertyChangedEventHandler;
            if (handler != null)
            {
                System.ComponentModel.PropertyChangedEventArgs eventArgs = createEventArgs();
                // Iterate for each Delegate logging each Excpetion we could receive
                foreach (Delegate d in handler.GetInvocationList())
                {
                    d.DynamicInvoke(this, eventArgs);
                }
            }
        }

        private void OnPropertyChanged(PropertyChangedEventArgsAdapter eventArgsAdapter)
        {
            RaiseEvent(() =>
            {
                RaisePropertyChanged(() =>
                {
                    return new PropertyChangedEventArgs(eventArgsAdapter, this);
                });
            });
        }

    }
}
