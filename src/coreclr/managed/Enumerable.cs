/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:Enumerable.cs
****/
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Collections.Specialized;

namespace Microsoft.PropertyModel
{
    /// <summary>
    /// CollectionModel Class
    /// </summary>
    public sealed class Enumerable<T> : Object<EnumerableAdapter>,
        IEnumerable<T>
    {

        public Enumerable(EnumerableAdapter adapter, ClassFactory classFactory) :
            base(adapter, classFactory)
        {
        }

        #region IEnumerator Interface

        System.Collections.IEnumerator System.Collections.IEnumerable.GetEnumerator()
        {
            throw new NotImplementedException();
        }

        #endregion

        public IEnumerator<T> GetEnumerator()
        {
            throw new NotImplementedException();
        }
    }

}
