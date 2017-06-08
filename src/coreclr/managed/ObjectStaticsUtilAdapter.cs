/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObjectStaticsUtilAdapter.cs
****/
using System;
using System.Runtime.InteropServices;

namespace Microsoft.PropertyModel
{
    public static class ObjectStaticsUtil
    {
        public static readonly Guid InspectableType = new Guid("AF86E2E0-B12D-4c6a-9C5A-D7AA65101E90");

        public static ObjectAdapter ActivateObject(Guid iidType, InspectableAdapter pContext)
        {
            IntPtr pObject = IntPtr.Zero;
            PInvokeUtils.ThrowIfResult(NativeMethods.ObjectStaticsUtil_ActivateObject(
                ref iidType,
                pContext != null ? pContext.Interface : IntPtr.Zero, 
                ref pObject));
            return CreateObject(pObject, GetObjectType(pObject));
        }

        internal static Guid GetObjectType(IntPtr pInterface)
        {
            Guid type = Guid.Empty;
            PInvokeUtils.ThrowIfResult(NativeMethods.ObjectStaticsUtil_GetObjectType(pInterface, ref type));
            return type;
        }

        internal static ObjectAdapter CreateObject(IntPtr pInterface, Guid iidType)
        {
            if (iidType == ObservableObjectAdapter.Type)
            {
                return new ObservableObjectAdapter(pInterface);
            }
            else if (iidType == ObjectDispatchAdapter.Type)
            {
                return new ObjectDispatchAdapter(pInterface);
            }
            else if (iidType == CommandAdapter.Type)
            {
                return new CommandAdapter(pInterface);
            }
            else if (iidType == ObservableCollectionAdapter.Type)
            {
                return new ObservableCollectionAdapter(pInterface);
            }
            else if (iidType == AsyncOperationAdapter.Type)
            {
                return new AsyncOperationAdapter(pInterface);
            }
            else if (iidType == EnumValueAdapter.Type)
            {
                return new EnumValueAdapter(pInterface);
            }
            return new ObjectAdapter(pInterface);
        }

        internal static InspectableAdapter CreateInspectable(IntPtr pInterface)
        {
            if(pInterface == IntPtr.Zero)
            {
                return null;
            }
            return CreateInspectable(pInterface,GetObjectType(pInterface));
        }

        internal static InspectableAdapter CreateInspectable(IntPtr pInterface, Guid iidType)
        {
            IntPtr pInterfaceCastTo = InspectableAdapter.CastToInternal(pInterface, iidType);
            NativeMethods.Inspectable_Release(pInterface);

            if (iidType == PropertyValueAdapter.Type)
            {
                return new PropertyValueAdapter(pInterfaceCastTo);
            }
            else if (iidType == DictionaryAdapter.Type)
            {
                return new DictionaryAdapter(pInterfaceCastTo);
            }
            else if (iidType == InspectableType)
            {
                return new InspectableAdapter(pInterfaceCastTo);
            }
            return CreateObject(pInterfaceCastTo, iidType);
        }

        internal static InspectableAdapter[] CreateInspectableArray(IntPtr[] pInterfaces)
        {
            System.Diagnostics.Debug.Assert(pInterfaces != null);
            InspectableAdapter[] result = new InspectableAdapter[pInterfaces.Length];
            for(int index = 0;index < pInterfaces.Length;++index)
            {
                result[index] = CreateInspectable(pInterfaces[index]);
            }
            return result;
        }
    }
}
