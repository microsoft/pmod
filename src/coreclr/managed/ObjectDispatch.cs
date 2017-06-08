/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObjectDispatch.cs
****/
using System;
using System.Runtime.InteropServices;

namespace Microsoft.PropertyModel
{
    public class ObjectDispatch<TAdapter> : Object<TAdapter>
        where TAdapter:ObjectDispatchAdapter
    {
        internal ObjectDispatch(TAdapter adapter, ClassFactory classFactory):
            base(adapter, classFactory)
        {
        }

        public object GetProperty(uint propertyId)
        {
            return this.ToFactoryObject(this.Adapter.GetProperty(propertyId));
        }

        public void SetProperty(uint propertyId,object value)
        {
            this.Adapter.SetProperty(propertyId, ToAdapter(value));
        }

        protected T GetProperty<T>(uint propertyId)
        {
            object value = GetProperty(propertyId);
            return ConvertTo<T>(value);
        }

    }
}
