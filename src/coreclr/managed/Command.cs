/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:Command.cs
****/
using System;
using System.Windows.Input;
using System.Reflection;
using System.Threading.Tasks;

namespace Microsoft.PropertyModel
{
    /// <summary>
    /// Class VoidModelType
    /// Summary:
    /// A class that represent a Void Type for using in a parameter or result
    /// </summary>
    public class VoidModelType
    {
    }


    /// <summary>
    /// ICommandModel interface
    /// Summary:
    /// An enhanced ICommand interface to support a Command State
    /// that can change.
    /// Also it allow execution with a result value
    /// </summary>
    public interface ICommand<TCommandState, TParameter, TResult> :
        ICommand
    {
        /// <summary>
        /// The Command Id Property
        /// </summary>
        uint CommandId { get; }

        /// <summary>
        /// The Command State Property
        /// </summary>
        TCommandState CommandState { get; }

        /// <summary>
        /// Execute the Command with a Result
        /// </summary>
        /// <param name="parameter"></param>
        /// <returns></returns>
        TResult Execute();

        /// <summary>
        /// Execute the Command with a Result and a Paramater
        /// </summary>
        /// <param name="parameter"></param>
        /// <returns></returns>
        TResult Execute(TParameter parameter);
    }

    /// <summary>
    /// CommandModelState Enum
    /// </summary>
    public enum CommandState
    {
        /// <summary>
        /// Command State is Unknown , CanInvoke has to be used
        /// </summary>
        Unknown = 0,

        /// <summary>
        /// Command State is Enabled
        /// </summary>
        Enabled = 1,

        /// <summary>
        /// Command State is Disabled
        /// </summary>
        Disabled = 2
    }

    /// <summary>
    /// CommandModel Class
    /// </summary>
    public sealed class Command<TCommandState, TParameter, TResult> :
        BaseNotifyPropertyChanged<CommandAdapter>,
        ICommand<TCommandState, TParameter, TResult>
    {
        private EventHandler CanExecuteChangedHandler;
        private bool IsCanExecuteChangedCallback { get; set; }
        private Lazy<ObservableObjectAdapter> LazyPropertyModelAdapter;

        public Command(CommandAdapter adapter, ClassFactory classFactory) :
            base(adapter, classFactory)
        {
            LazyPropertyModelAdapter = new Lazy<ObservableObjectAdapter>(() =>
            {
                return new ObservableObjectAdapter(this.Adapter.CastTo(ObservableObjectAdapter.Type));
            });
        }

        #region Public Properties

        public uint CommandId
        {
            get
            {
                // TODO: need to sync with the Native Impl.
                return 0;
            }
        }

        public TCommandState CommandState
        {
            get
            {
                return (TCommandState)Enum.ToObject(typeof(TCommandState), this.Adapter.State);
            }
        }

        #endregion

        #region Public Methods

        public TResult Execute()
        {
            return Execute(default(TParameter));
        }

        public TResult Execute(TParameter parameter)
        {
            InspectableAdapter result = this.Adapter.Execute(this.ToAdapter(parameter));
            if(typeof(TResult) != typeof(VoidModelType))
            {
                var factoryObject = ToFactoryObject(result);

                var typeInfo = typeof(TResult).GetTypeInfo();
                if(typeInfo.IsGenericType && 
                    typeInfo.GetGenericTypeDefinition()==typeof(Task<>))
                {
                    var asynOperation = factoryObject.AssertCast<IAsyncOperation>();
                    return asynOperation.GetTask().AssertCast<TResult>();
                }
                return (TResult)factoryObject;
            }
            return default(TResult);
        }

        #endregion

        #region ICommand Interface

        /// <summary>
        /// CanExecuteChanged Event
        /// </summary>
        public event EventHandler CanExecuteChanged
        {
            add
            {
                this.CanExecuteChangedHandler += value;
                OnCanExecuteChangedChanged(true);
            }
            remove
            {
                this.CanExecuteChangedHandler -= value;
                OnCanExecuteChangedChanged(false);
            }
        }

        protected override ObservableObjectAdapter PropertyModelAdapter
        {
            get
            {
                return LazyPropertyModelAdapter.Value;
            }
        }
        private void OnCanExecuteChangedChanged(bool add)
        {
            int countHandlers = this.CanExecuteChangedHandler != null ?
                this.CanExecuteChangedHandler.GetInvocationList().Length :
                0;
            if (add && countHandlers == 1 && !this.IsCanExecuteChangedCallback)
            {
                this.Adapter.AddCanExecuteChanged(OnCanExecuteChanged);
                this.IsCanExecuteChangedCallback = true;
            }
            else if (!add && countHandlers == 0 && this.IsCanExecuteChangedCallback)
            {
                this.Adapter.RemoveCanExecuteChanged(OnCanExecuteChanged);
                this.IsCanExecuteChangedCallback = false;
            }
        }


        bool ICommand.CanExecute(object parameter)
        {
            return this.Adapter.CanExecute(this.ToAdapter(parameter));
        }

        void ICommand.Execute(object parameter)
        {
            this.Adapter.Execute(this.ToAdapter(parameter));
        }

        #endregion

        void OnCanExecuteChanged(CanExecuteChangedEventArgsAdapter eventArgsAdapter)
        {
            RaiseEvent(() =>
            {
                EventHandler canExecuteChangedHandler = this.CanExecuteChangedHandler;
                if (canExecuteChangedHandler != null)
                {
                    canExecuteChangedHandler(this, EventArgs.Empty);
                }
                OnPropertyChanged("CommandState");
            });
        }
    }

}
