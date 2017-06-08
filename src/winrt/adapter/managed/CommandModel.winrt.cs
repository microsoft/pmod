/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:CommandModel.winrt.cs
****/
using Microsoft.PropertyModel;
using System;
using System.Runtime.InteropServices;
using System.Windows.Input;

namespace CoreInterop
{

    /// <summary>
    /// CommandModel Class
    /// </summary>
    public partial class CommandModel<TCommandState, TParameter, TResult>
    {
        private _SinkHelper _sinkHelper;
        class _SinkHelper
        {
            private WeakReference _target;

            internal _SinkHelper(object target)
            {
                _target = new WeakReference(target);
            }
            internal void OnCanExecuteChanged(object sender, CanExecuteChangedEventArgs e)
            {
                if (_target.IsAlive)
                {
                    ((CommandModel<TCommandState, TParameter, TResult>)_target.Target).OnCanExecuteChanged(e);
                }
            }
        }
        
        internal override void AttachEvents()
        {
            if (_sinkHelper == null)
            {
                _sinkHelper = new _SinkHelper(this);
            }
            this._adapterInterface.CanExecuteChanged += _sinkHelper.OnCanExecuteChanged;
        }

        internal override void DetachEvents()
        {
            if (_sinkHelper != null)
            {
                this._adapterInterface.CanExecuteChanged -= _sinkHelper.OnCanExecuteChanged;
            }
        }
    }

}
