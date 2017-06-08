/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:Dictionary.winrt.cs
****/
using Microsoft.PropertyModel;
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace CoreInterop
{

    /// <summary>
    /// Dictionary Class
    /// </summary>
    public partial class Dictionary :
        BaseCoreAdapter<IDictionary<String, Object>>
    {

        internal Dictionary(object  adapter, ClassFactory classFactory):
            base((IDictionary<String, Object>)adapter,classFactory)
        {
        }

        static internal bool IsDictionary(object o)
        {
            System.Diagnostics.Debug.Assert(o != null);

            return o is System.Collections.Generic.IDictionary<String, Object>;
        }

        public object this[string key]
        {
            get { return this.ToAdapter(this._adapterInterface[key]); }
            set
            {
                this._adapterInterface[key] = FromAdapter(value);
            }
        }
    }
}
