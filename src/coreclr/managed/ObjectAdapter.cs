/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObjectAdapter.cs
****/
using System;
using System.Runtime.InteropServices;

namespace Microsoft.PropertyModel
{
    public class ObjectAdapter : InspectableAdapter
    {

        public ObjectAdapter(IntPtr pObject) :base(pObject)
        {
        }

        public UInt32 UniqueId
        {
            get
            {
                UInt32 uniqueId = 0;
                PInvokeUtils.ThrowIfResult(NativeMethods.Object_GetUniqueId(this.Interface,ref uniqueId));
                return uniqueId;
            }
        }

        public UInt32 ProcessId
        {
            get
            {
                UInt32 processId = 0;
                PInvokeUtils.ThrowIfResult(NativeMethods.Object_GetProcessId(this.Interface, ref processId));
                return processId;
            }
        }

        public Guid RuntimeType
        {
            get
            {
                Guid runtimeType = Guid.Empty;
                PInvokeUtils.ThrowIfResult(NativeMethods.Object_GetType(this.Interface, ref runtimeType));
                return runtimeType;
           }
        }

        public ObjectTypeInfoAdapter TypeInfo
        {
            get
            {
                IntPtr pObjectTypeInfo = IntPtr.Zero;
                PInvokeUtils.ThrowIfResult(NativeMethods.Object_GetTypeInfo(this.Interface, ref pObjectTypeInfo));
                return new ObjectTypeInfoAdapter(pObjectTypeInfo);
            }
        }
    }
}
