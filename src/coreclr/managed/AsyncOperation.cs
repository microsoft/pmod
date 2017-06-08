/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:AsyncOperation.cs
****/
using System;
using System.Threading.Tasks;

namespace Microsoft.PropertyModel
{
    public interface IAsyncOperation : IObject
    {
        Task GetTask();
    }

    /// <summary>
    /// AsyncOperation<> Class
    /// </summary>
    public sealed class AsyncOperation<TResult> : Object<AsyncOperationAdapter>,
        IAsyncResult,
        IAsyncOperation
    {
        private EventHandler CompletedHandler;
        private bool IsCompletedCallback { get; set; }

        const uint _Infinite_Timeout = unchecked((uint)-1);

        public AsyncOperation(AsyncOperationAdapter adapter, ClassFactory classFactory) :
            base(adapter, classFactory)
        {
        }

        /// <summary>
        /// Completed Event
        /// </summary>
        public event EventHandler Completed
        {
            add
            {
                this.CompletedHandler += value;
                OnCompletedChanged(true);
            }
            remove
            {
                this.CompletedHandler -= value;
                OnCompletedChanged(false);
            }
        }

        #region Public Properties

        public AsyncStatus Status
        {
            get
            {
                return this.Adapter.Status;
            }
        }

        #endregion

        #region Public methods

        public TResult GetResults()
        {
            object results = ToFactoryObject(this.Adapter.GetResults());
            return ConvertTo<TResult>(results);
        }

        Task IAsyncOperation.GetTask()
        {
            return this.GetTask();
        }

        public Task<TResult> GetTask()
        {
            return Task<TResult>.Factory.FromAsync(
                this as IAsyncResult,
                (asyncResult) =>
                {
                    return this.GetResults();
                });
        }

        #endregion

        private void OnCompletedChanged(bool add)
        {
            int countHandlers = this.CompletedHandler != null ?
                this.CompletedHandler.GetInvocationList().Length :
                0;
            if (add && countHandlers == 1 && !this.IsCompletedCallback)
            {
                this.Adapter.AddCompleted(OnCompleted);
                this.IsCompletedCallback = true;
            }
            else if (countHandlers == 0 && this.IsCompletedCallback)
            {
                this.Adapter.RemoveCompleted(OnCompleted);
                this.IsCompletedCallback = false;
            }
        }

        private void OnCompleted(CompletedEventArgsAdapter eventArgsAdapter)
        {
            RaiseEvent(() =>
            {
                EventHandler completedHandler = this.CompletedHandler;
                if (completedHandler != null)
                {
                    completedHandler(this, EventArgs.Empty);
                }
            });
        }

        object IAsyncResult.AsyncState
        {
            get { return null; }
        }

        System.Threading.WaitHandle IAsyncResult.AsyncWaitHandle
        {
            get
            {
                bool completed = this.Status != AsyncStatus.Started;
                var waitHandle = new System.Threading.EventWaitHandle(
                    completed,
                    System.Threading.EventResetMode.ManualReset);
                if (!completed)
                {
                    var completedHandler = new EventHandler((sender, e) =>
                    {
                        waitHandle.Set();
                    });
                    this.Completed += completedHandler;
                }
                return waitHandle;
            }
        }

        bool IAsyncResult.CompletedSynchronously
        {
            get { return false; }
        }

        bool IAsyncResult.IsCompleted
        {
            get
            {
                return this.Status != AsyncStatus.Started;
            }
        }
    }

}
