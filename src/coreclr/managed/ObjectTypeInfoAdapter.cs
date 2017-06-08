/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObjectTypeInfoAdapter.cs
****/
using System;
using System.Runtime.InteropServices;

namespace Microsoft.PropertyModel
{
    public class ObjectTypeInfoAdapter : InspectableAdapter, IObjectTypeInfo
    {

        internal ObjectTypeInfoAdapter(IntPtr pInterface):base(pInterface)
        {
        }

        public UInt32 TypeId
        {
            get
            {
                UInt32 typeId = 0;
                PInvokeUtils.ThrowIfResult(NativeMethods.ObjectTypeInfo_GetTypeId(this.Interface, ref typeId));
                return typeId;
            }
        }

        public Guid Type
        {
            get
            {
                Guid type = Guid.Empty;
                PInvokeUtils.ThrowIfResult(NativeMethods.ObjectTypeInfo_GetType(this.Interface, ref type));
                return type;
            }
        }

        public string Name
        {
            get
            {
                return PInvokeUtils.GetString(delegate (ref IntPtr pStr)
                {
                    return NativeMethods.ObjectTypeInfo_GetName(this.Interface, ref pStr);
                });
            }
        }

    }
}
