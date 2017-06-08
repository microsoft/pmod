/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObservableObject.cs
****/
using System;
using System.Runtime.InteropServices;

namespace Microsoft.PropertyModel
{

    /// <summary>
    /// EventModelArgs Class
    /// </summary>
    public class EventModelArgs : EventArgs
    {
        private uint _eventId;
        private object _eventArgs;

        internal EventModelArgs(
            uint eventId,
            object eventArgs)
        {
            _eventId = eventId;
            _eventArgs = eventArgs;
        }

        public uint Event { get { return _eventId; } }
        public object EventArgs { get { return _eventArgs; } }
    }

    public class ObservableObject : BaseNotifyPropertyChanged<ObservableObjectAdapter>
    {
        private event EventHandler<EventModelArgs> EventModelHandler;

        public ObservableObject(ObservableObjectAdapter adapter, ClassFactory classFactory):
            base(adapter, classFactory)
        {
        }

        /// <summary>
        /// EventModel Event
        /// </summary>
        public event EventHandler<EventModelArgs> EventModel
        {
            add
            {
                this.EventModelHandler += value;
                //EnsureAdviseSink();
            }
            remove
            {
                this.EventModelHandler -= value;
                //EnsureUnadviseSink();
            }
        }
        public object GetProperty(uint propertyId)
        {
            return this.ToFactoryObject(this.Adapter.GetProperty(propertyId));
        }

        public void SetProperty(uint propertyId, object value)
        {
            this.Adapter.SetProperty(propertyId, ToAdapter(value));
        }

        public object Invoke(uint methodId, params object[] invokeParameters)
        {
            InspectableAdapter[] parameterAdapters = new InspectableAdapter[invokeParameters.Length];
            for (int index = 0; index < invokeParameters.Length; ++index)
            {
                parameterAdapters[index] = this.ToAdapter(invokeParameters[index]);
            }
            InspectableAdapter result = this.Adapter.Invoke(methodId, parameterAdapters);
            return this.ToFactoryObject(result);
        }

        protected T Invoke<T>(uint methodId, params object[] invokeParameters)
        {
            object result = this.Invoke(methodId, invokeParameters);
            return ConvertTo<T>(result);
        }

        protected T GetProperty<T>(uint propertyId)
        {
            object value = GetProperty(propertyId);
            return ConvertTo<T>(value);
        }

        protected override ObservableObjectAdapter PropertyModelAdapter
        {
            get
            {
                return this.Adapter;
            }
        }

        private void OnEventModel(EventModelEventArgsAdapter eventArgsAdapter)
        {
            RaiseEvent(() =>
            {
                EventHandler<EventModelArgs> eventModelHandler = this.EventModelHandler;
                if (eventModelHandler != null)
                {
                    eventModelHandler(this, new EventModelArgs(
                            eventArgsAdapter.Event,
                            ToFactoryObject(eventArgsAdapter.EventArgs)));
                }
            });
        }
    }
}
