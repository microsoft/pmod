/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:AsyncOperation.winrt.cs
****/
using Microsoft.PropertyModel;
using System;
using System.Runtime.InteropServices;

namespace CoreInterop
{

    /// <summary>
    /// AsyncOperationModel Class
    /// </summary>
    public partial class AsyncOperation<TResult>
    {
        private _SinkHelper _sinkHelper;
        class _SinkHelper
        {
            private WeakReference _target;

            internal _SinkHelper(object target)
            {
                _target = new WeakReference(target);
            }
            internal void OnResultCompletedEventHandler(object sender, ResultCompletedEventArgs e)
            {
                if (_target.IsAlive)
                {
                    ((AsyncOperation<TResult>)_target.Target).OnCompleted(e);
                }
            }
        }
        private static object GetResults(IAsyncOperation asyncOperation)
        {
            return asyncOperation.GetResults();
        }

        internal override void AttachEvents()
        {
            if (_sinkHelper == null)
            {
                _sinkHelper = new _SinkHelper(this);
            }
            this._adapterInterface.ResultCompleted += _sinkHelper.OnResultCompletedEventHandler;
        }

        internal override void DetachEvents()
        {
            if (_sinkHelper != null)
            {
                this._adapterInterface.ResultCompleted -= _sinkHelper.OnResultCompletedEventHandler;
            }
        }
    }

}
