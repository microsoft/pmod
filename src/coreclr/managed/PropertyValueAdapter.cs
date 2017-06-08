/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PropertyValueAdapter.cs
****/
using System;
using System.Runtime.InteropServices;
using System.Reflection;

namespace Microsoft.PropertyModel
{
    public class PropertyValueAdapter : InspectableAdapter
    {
        public static readonly Guid Type = new Guid("4BD682DD-7554-40E9-9A9B-82654EDE7E62");

        public static PropertyValueAdapter CreateValue(object o)
        {
            Type type = o.GetType();
            if (type == typeof(Boolean))
            {
                return new PropertyValueAdapter((Boolean)o);
            }
            else if (type == typeof(String))
            {
                return new PropertyValueAdapter((String)o);
            }
            else if (type == typeof(byte))
            {
                return new PropertyValueAdapter((Byte)o);
            }
            else if (type == typeof(DateTime))
            {
                return new PropertyValueAdapter((DateTime)o);
            }
            else if (type == typeof(short))
            {
                return new PropertyValueAdapter((short)o);
            }
            else if (type == typeof(ushort))
            {
                return new PropertyValueAdapter((ushort)o);
            }
            else if (type == typeof(int))
            {
                return new PropertyValueAdapter((int)o);
            }
            else if (type == typeof(uint))
            {
                return new PropertyValueAdapter((uint)o);
            }
            else if (type == typeof(uint[]))
            {
                return new PropertyValueAdapter((uint[])o);
            }
            else if (type.GetTypeInfo().IsEnum)
            {
                return new PropertyValueAdapter(Convert.ToUInt32(o));
            }
            throw new NotImplementedException();
        }

        private delegate int CreateValueFunc<T>(T value, ref IntPtr pvalue);

        private void CreateHelper<T>(T value, CreateValueFunc<T> pInvokeCreate)
        {
            IntPtr pValue = IntPtr.Zero;
            PInvokeUtils.ThrowIfResult(pInvokeCreate(value, ref pValue));
            this.SetInterface(pValue);
        }
        private delegate int CreateArrayValueFunc<T>(UInt32 size,T[] value, ref IntPtr pvalue);

        private void CreateArrayHelper<T>(T[] value, CreateArrayValueFunc<T> pInvokeCreate)
        {
            IntPtr pValue = IntPtr.Zero;
            PInvokeUtils.ThrowIfResult(pInvokeCreate((uint)value.Length, value, ref pValue));
            this.SetInterface(pValue);
        }

        internal PropertyValueAdapter(IntPtr pInterface):base(pInterface)
        {
        }

        public PropertyValueAdapter(bool value)
        {
            CreateHelper<bool>(value, NativeMethods.PropertyValue_CreateBoolean);
        }

        public PropertyValueAdapter(string value)
        {
            CreateHelper<string>(value, NativeMethods.PropertyValue_CreateString);
        }
        public PropertyValueAdapter(DateTime value)
        {
            CreateHelper<long>(value.ToFileTimeUtc(), NativeMethods.PropertyValue_CreateDateTime);
        }

        public PropertyValueAdapter(byte value)
        {
            CreateHelper<byte>(value, NativeMethods.PropertyValue_CreateByte);
        }
        public PropertyValueAdapter(Int16 value)
        {
            CreateHelper<Int16>(value, NativeMethods.PropertyValue_CreateInt16);
        }
        public PropertyValueAdapter(UInt16 value)
        {
            CreateHelper<UInt16>(value, NativeMethods.PropertyValue_CreateUInt16);
        }
        public PropertyValueAdapter(Int32 value)
        {
            CreateHelper<Int32>(value, NativeMethods.PropertyValue_CreateInt32);
        }
        public PropertyValueAdapter(UInt32 value)
        {
            CreateHelper<UInt32>(value, NativeMethods.PropertyValue_CreateUInt32);
        }
        public PropertyValueAdapter(UInt64 value)
        {
            CreateHelper<UInt64>(value, NativeMethods.PropertyValue_CreateUInt64);
        }

        public PropertyValueAdapter(byte[] values)
        {
            CreateArrayHelper<byte>(values, NativeMethods.PropertyValue_CreateBytes);
        }
        public PropertyValueAdapter(uint[] values)
        {
            CreateArrayHelper<uint>(values, NativeMethods.PropertyValue_CreateUInt32Array);
        }

        public PropertyValueAdapter(IntPtr[] values)
        {
            CreateArrayHelper<IntPtr>(values, NativeMethods.PropertyValue_CreateInspectableArray);
        }

        public object ToObject()
        {
            switch(this.PropertyType)
            {
                case PropertyType.UInt8:
                    return GetByte();
                case PropertyType.Boolean:
                    return GetBoolean();
                case PropertyType.DateTime:
                    return GetDateTime();
                case PropertyType.Int16:
                    return GetInt16();
                case PropertyType.UInt16:
                    return GetInt16();
                case PropertyType.Int32:
                    return GetInt32();
                case PropertyType.UInt32:
                    return GetUInt32();
                case PropertyType.UInt64:
                    return GetUInt64();
                case PropertyType.String:
                    return GetString();
                case PropertyType.UInt8Array:
                    return GetBytes();
                case PropertyType.UInt32Array:
                    return GetUInt32Array();
                case PropertyType.InspectableArray:
                    return GetInspectableArray();
            }
            throw new InvalidOperationException();
        }

        public PropertyType PropertyType
        {
            get
            {
                int type = 0;
                PInvokeUtils.ThrowIfResult(NativeMethods.PropertyValue_GetType(this.Interface, ref type));
                return (PropertyType)type;
            }
        }
        public String GetString()
        {
            return PInvokeUtils.GetString(delegate (ref IntPtr pStr)
            {
                return NativeMethods.PropertyValue_GetString(this.Interface, ref pStr);
            });
        }

        public IntPtr[] GetInspectableArray()
        {
            return PInvokeUtils.GetPtrArray(delegate (ref UInt32 size, ref IntPtr pValues)
            {
                return NativeMethods.PropertyValue_GetInspectableArray(this.Interface, ref size, ref pValues);
            });
        }

        public DateTime GetDateTime()
        {
            long value = 0;
            PInvokeUtils.ThrowIfResult(NativeMethods.PropertyValue_GetDateTime(this.Interface, ref value));
            return DateTime.FromFileTime(value);
        }

        public byte GetByte()
        {
            byte value = 0;
            PInvokeUtils.ThrowIfResult(NativeMethods.PropertyValue_GetByte(this.Interface, ref value));
            return value;
        }

        public bool GetBoolean()
        {
            bool value = false;
            PInvokeUtils.ThrowIfResult(NativeMethods.PropertyValue_GetBoolean(this.Interface, ref value));
            return value;
        }
        public Int32 GetInt16()
        {
            Int16 value = 0;
            PInvokeUtils.ThrowIfResult(NativeMethods.PropertyValue_GetInt16(this.Interface, ref value));
            return value;
        }
        public Int32 GetUInt16()
        {
            UInt16 value = 0;
            PInvokeUtils.ThrowIfResult(NativeMethods.PropertyValue_GetUInt16(this.Interface, ref value));
            return value;
        }

        public Int32 GetInt32()
        {
            Int32 value = 0;
            PInvokeUtils.ThrowIfResult(NativeMethods.PropertyValue_GetInt32(this.Interface, ref value));
            return value;
        }

        public UInt32 GetUInt32()
        {
            UInt32 value = 0;
            PInvokeUtils.ThrowIfResult(NativeMethods.PropertyValue_GetUInt32(this.Interface, ref value));
            return value;
        }
        public UInt64 GetUInt64()
        {
            UInt64 value = 0;
            PInvokeUtils.ThrowIfResult(NativeMethods.PropertyValue_GetUInt64(this.Interface, ref value));
            return value;
        }

        private delegate int GetArrayDelegate(IntPtr pInterface,ref UInt32 size, ref IntPtr pStr);
        private T[] GetArrayHelper<T>(GetArrayDelegate pDelegate,Action<IntPtr, T[]> copyFunc)
            where T :struct
        {
            UInt32 size = 0;
            IntPtr values = IntPtr.Zero;
            PInvokeUtils.ThrowIfResult(pDelegate(this.Interface, ref size, ref values));

            T[] retValues = new T[size];
            copyFunc(values, retValues);
            Marshal.FreeCoTaskMem(values);
            return retValues;
        }

        public byte[] GetBytes()
        {
            return GetArrayHelper<byte>(NativeMethods.PropertyValue_GetBytes, (values,retValues) =>
            {
                Marshal.Copy(values, retValues, 0, retValues.Length);
            });
        }
        public int[] GetInt32Array()
        {
            return GetArrayHelper<int>(NativeMethods.PropertyValue_GetInt32Array, (values, retValues) =>
            {
                Marshal.Copy(values, retValues, 0, retValues.Length);
            });
        }

        public uint[] GetUInt32Array()
        {
            return GetArrayHelper<uint>(NativeMethods.PropertyValue_GetInt32Array, (values, retValues) =>
            {
                int[] temp = new int[retValues.Length];
                Marshal.Copy(values, temp, 0, retValues.Length);
                for (int index = 0; index < temp.Length; ++index)
                {
                    retValues[index] = (uint)temp[index];
                }
            });
        }
    }
}
