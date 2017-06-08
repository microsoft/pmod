/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PInvokeUtils.cs
****/
using System;
using System.Runtime.InteropServices;

namespace Microsoft.PropertyModel
{

    public static class PInvokeUtils
    {
        internal delegate int GetStringDelegate(ref IntPtr pStr);
        internal delegate int GetArrayDelegate(ref UInt32 size, ref IntPtr pStr);

        public static void LoadLibrary(string moduleName)
        {
            NativeMethods.PInvokeUtils_LoadLibrary(moduleName);
        }

        public static void Dispose(object factoryObject)
        {
            var pFactoryObject = factoryObject as IFactoryObject;
            if(pFactoryObject == null)
            {
                throw new ArgumentException("non IFactoryObject");
            }
            NativeMethods.PInvokeUtils_Dispose(pFactoryObject.Adapter.Interface);
        }

        internal static void ThrowIfResult(int hr)
        {
            if(hr < 0)
            {
                throw new COMException("Failure", hr);
            }
        }

        internal static string GetString(GetStringDelegate pInvokeAction)
        {
            IntPtr pStr = IntPtr.Zero;
            int result = pInvokeAction(ref pStr);
            ThrowIfResult(result);
#if _WINDOWS
            string str = Marshal.PtrToStringUni(pStr);
#else
            string str = Marshal.PtrToStringAnsi(pStr);
#endif
            Marshal.FreeCoTaskMem(pStr);
            return str;
        }

        internal static IntPtr[] GetPtrArray(GetArrayDelegate pInvokeAction)
        {
            IntPtr values = IntPtr.Zero;
            UInt32 size = 0;
            int result = pInvokeAction(ref size, ref values);
            ThrowIfResult(result);

            IntPtr[] retValues = new IntPtr[size];
            if (size > 0)
            {
                Marshal.Copy(values, retValues, 0, retValues.Length);
            }
            if (values != IntPtr.Zero)
            {
                Marshal.FreeCoTaskMem(values);
            }
            return retValues;
        }

    }
}
