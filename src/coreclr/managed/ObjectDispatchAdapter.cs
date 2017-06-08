/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObjectDispatchAdapter.cs
****/
using System;
using System.Runtime.InteropServices;

namespace Microsoft.PropertyModel
{
    public class ObjectDispatchAdapter : ObjectAdapter
    {
        public static readonly Guid Type = new Guid("665F384D-5BE6-40D6-992B-B2D9AA3890AE");
    

        public ObjectDispatchAdapter(IntPtr pPropertyData):base(pPropertyData)
        {
        }

        public InspectableAdapter GetProperty(uint propertyId)
        {
            IntPtr pValue = IntPtr.Zero;
            PInvokeUtils.ThrowIfResult(NativeMethods.ObjectDispatch_GetProperty(this.Interface, propertyId,ref pValue));
            return ObjectStaticsUtil.CreateInspectable(pValue);
        }

        public void SetProperty(uint propertyId,InspectableAdapter pValue)
        {
            PInvokeUtils.ThrowIfResult(NativeMethods.ObjectDispatch_SetProperty(
                this.Interface, 
                propertyId, 
                pValue != null ? pValue.Interface:IntPtr.Zero));
        }

        public InspectableAdapter Invoke(uint methodId, InspectableAdapter[] parameters)
        {
            IntPtr[] pParameters = new IntPtr[parameters != null ? parameters.Length : 0];
            for (int index = 0; index < pParameters.Length; ++index)
            {
                var adapterParameter = parameters[index];
                pParameters[index] = adapterParameter != null ? adapterParameter.Interface : IntPtr.Zero;
            }
            IntPtr pResult = IntPtr.Zero;
            PInvokeUtils.ThrowIfResult(NativeMethods.ObjectDispatch_Invoke(this.Interface, methodId, (uint)pParameters.Length, pParameters, ref pResult));
            return ObjectStaticsUtil.CreateInspectable(pResult);
        }

    }
}
