/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:NativeMethods.cs
****/
using System;
using System.Runtime.InteropServices;

namespace Microsoft.PropertyModel
{
    internal static class NativeMethods
    {
        internal delegate int OnCallbackEventHandler(IntPtr pEventArgs);

        internal delegate int HasThreadAccessCallback(ref byte result);
        internal delegate int RunAsyncCallback(IntPtr pDelegateDispatchable, IntPtr pAsyncOperation);

#if _WINDOWS
        const string _DllImport = "pmod_pinvoke";
        const CharSet _PlatformCharSet = CharSet.Unicode;
#else
        const string _DllImport = "pmod";
        const CharSet _PlatformCharSet = CharSet.Ansi;
#endif
        // PInvokes Utils
        [DllImport(_DllImport, CharSet = _PlatformCharSet)]
        internal static extern int PInvokeUtils_LoadLibrary(String moduleName);
        [DllImport(_DllImport)]
        internal static extern int PInvokeUtils_Dispose(IntPtr pInterface);

        // Inspectable PInvokes
        [DllImport(_DllImport)]
        internal static extern int Inspectable_AddRef(IntPtr pInterface);
        [DllImport(_DllImport)]
        internal static extern int Inspectable_Release(IntPtr pInterface);

        [DllImport(_DllImport)]
        internal static extern int Inspectable_IsTypeOf(IntPtr pInterface,ref Guid iidType, ref bool result);
        [DllImport(_DllImport, CharSet = _PlatformCharSet)]
        internal static extern int Inspectable_CastTo(IntPtr pInterface, ref Guid iidType, ref IntPtr pCastInterface);

        // PropertyValue PInvokes
        [DllImport(_DllImport)]
        internal static extern int PropertyValue_CreateBytes(UInt32 size, byte[] values, ref IntPtr pPropertyValue);

        [DllImport(_DllImport)]
        internal static extern int PropertyValue_GetType(IntPtr pPropertyValue, ref int type);
        
        // Boolean support
        [DllImport(_DllImport)]

        internal static extern int PropertyValue_CreateBoolean(bool value, ref IntPtr pPropertyValue);
        [DllImport(_DllImport)]
        internal static extern int PropertyValue_GetBoolean(IntPtr pPropertyValue, ref bool value);

        // String support
        [DllImport(_DllImport, CharSet = _PlatformCharSet)]
        internal static extern int PropertyValue_CreateString(String value, ref IntPtr pPropertyValue);

        [DllImport(_DllImport)]
        internal static extern int PropertyValue_GetString(IntPtr pPropertyValue, ref IntPtr values);

        // DateTime support
        [DllImport(_DllImport)]
        internal static extern int PropertyValue_CreateDateTime(long fileTime, ref IntPtr pPropertyValue);

        [DllImport(_DllImport)]
        internal static extern int PropertyValue_GetDateTime(IntPtr pPropertyValue, ref long fileTime);

        // Byte support
        [DllImport(_DllImport)]
        internal static extern int PropertyValue_CreateByte(byte value, ref IntPtr pPropertyValue);

        [DllImport(_DllImport)]
        internal static extern int PropertyValue_GetByte(IntPtr pPropertyValue, ref byte pValue);

        [DllImport(_DllImport)]
        internal static extern int PropertyValue_GetBytes(IntPtr pPropertyValue, ref UInt32 size, ref IntPtr values);

        // Int16 support
        [DllImport(_DllImport)]
        internal static extern int PropertyValue_CreateInt16(Int16 value, ref IntPtr pPropertyValue);

        [DllImport(_DllImport)]
        internal static extern int PropertyValue_GetInt16(IntPtr pPropertyValue, ref Int16 pValue);
        // UInt16 support
        [DllImport(_DllImport)]
        internal static extern int PropertyValue_CreateUInt16(UInt16 value, ref IntPtr pPropertyValue);

        [DllImport(_DllImport)]
        internal static extern int PropertyValue_GetUInt16(IntPtr pPropertyValue, ref UInt16 pValue);

        // Int32 support
        [DllImport(_DllImport)]
        internal static extern int PropertyValue_CreateInt32(Int32 value, ref IntPtr pPropertyValue);

        [DllImport(_DllImport)]
        internal static extern int PropertyValue_GetInt32(IntPtr pPropertyValue, ref Int32 pValue);

        [DllImport(_DllImport)]
        internal static extern int PropertyValue_CreateInt32Array(UInt32 size, UInt32[] values, ref IntPtr pPropertyValue);

        [DllImport(_DllImport)]
        internal static extern int PropertyValue_GetInt32Array(IntPtr pPropertyValue, ref UInt32 size, ref IntPtr values);

        // UInt32 support
        [DllImport(_DllImport)]
        internal static extern int PropertyValue_CreateUInt32(UInt32 value, ref IntPtr pPropertyValue);

        [DllImport(_DllImport)]
        internal static extern int PropertyValue_GetUInt32(IntPtr pPropertyValue, ref UInt32 pValue);

        [DllImport(_DllImport)]
        internal static extern int PropertyValue_CreateUInt32Array(UInt32 size, UInt32[] values, ref IntPtr pPropertyValue);

        [DllImport(_DllImport)]
        internal static extern int PropertyValue_GetUInt32Array(IntPtr pPropertyValue, ref UInt32 size, ref IntPtr values);

        // UInt64 support
        [DllImport(_DllImport)]
        internal static extern int PropertyValue_CreateUInt64(UInt64 value, ref IntPtr pPropertyValue);

        [DllImport(_DllImport)]
        internal static extern int PropertyValue_GetUInt64(IntPtr pPropertyValue, ref UInt64 pValue);

        // Inspectable Array support
        [DllImport(_DllImport)]
        internal static extern int PropertyValue_CreateInspectableArray(UInt32 size, IntPtr[] values, ref IntPtr pPropertyValue);

        [DllImport(_DllImport)]
        internal static extern int PropertyValue_GetInspectableArray(IntPtr pPropertyValue, ref UInt32 size, ref IntPtr values);


        // Logger PInvokes
        [DllImport(_DllImport)]
        internal static extern int Logger_IsAvailable(
            ref bool isAvailable);

        [DllImport(_DllImport)]
        internal static extern int Logger_GetFoundationInstance(
            ref IntPtr pInterface);

        [DllImport(_DllImport, CharSet = _PlatformCharSet)]
        internal static extern int Logger_GetCategoryLevel(
            IntPtr pInterface,
            String pCategoryPath,
            ref UInt32 level);

        [DllImport(_DllImport, CharSet = _PlatformCharSet)]
        internal static extern int Logger_SetCategoryLevel(
            IntPtr pInterface,
            UInt32 level,
            String pCategoryPath);

        [DllImport(_DllImport, CharSet = _PlatformCharSet)]
        internal static extern int Logger_LogMessage(
            UInt32 level,
            UInt32 messageId,
            String pText,
            IntPtr pParameter
            );
        // Dictionary PInvokes
        [DllImport(_DllImport, CharSet = _PlatformCharSet)]
        internal static extern int Dictionary_CreateInstance(ref IntPtr pInterface);

        [DllImport(_DllImport, CharSet = _PlatformCharSet)]
        internal static extern int Dictionary_Lookup(IntPtr pInterface, String key, ref IntPtr pValue);

        [DllImport(_DllImport, CharSet = _PlatformCharSet)]
        internal static extern int Dictionary_HasKey(IntPtr pInterface, String key, ref bool pValue);

        [DllImport(_DllImport, CharSet = _PlatformCharSet)]
        internal static extern int Dictionary_Insert(IntPtr pInterface, String key, IntPtr pValue, ref bool replace);

        [DllImport(_DllImport, CharSet = _PlatformCharSet)]
        internal static extern int Dictionary_Remove(IntPtr pInterface, String key);

        [DllImport(_DllImport)]
        internal static extern int Dictionary_GetView(IntPtr pInterface,ref UInt32 pSize, ref IntPtr pValues);

        [DllImport(_DllImport)]
        internal static extern int DictionaryPair_GetKey(IntPtr pInterface,ref IntPtr pValue);
        [DllImport(_DllImport)]
        internal static extern int DictionaryPair_GetValue(IntPtr pInterface, ref IntPtr pValue);

        // Object PInvokes
        [DllImport(_DllImport)]
        internal static extern int Object_GetUniqueId(IntPtr pInterface,ref UInt32 uniqueId);
        [DllImport(_DllImport)]
        internal static extern int Object_GetProcessId(IntPtr pInterface, ref UInt32 processId);
        [DllImport(_DllImport)]
        internal static extern int Object_GetTypeInfo(IntPtr pInterface, ref IntPtr pObjectTypeInfo);
        [DllImport(_DllImport)]
        internal static extern int Object_GetType(IntPtr pInterface, ref Guid type);

        // ObjectTypeInfo PInvokes
        [DllImport(_DllImport)]
        internal static extern int ObjectTypeInfo_GetTypeId(IntPtr pObjectTypeInfo, ref UInt32 typeId);
        [DllImport(_DllImport)]
        internal static extern int ObjectTypeInfo_GetType(IntPtr pObjectTypeInfo,ref Guid type);

        [DllImport(_DllImport)]
        internal static extern int ObjectTypeInfo_GetName(IntPtr pObjectTypeInfo, ref IntPtr values);

        // ObjectStaticsUtil PInvokes
        [DllImport(_DllImport)]
        internal static extern int ObjectStaticsUtil_ActivateObject(ref Guid iidType, IntPtr pContext, ref IntPtr pObject);
        [DllImport(_DllImport)]
        internal static extern int ObjectStaticsUtil_GetObjectType(IntPtr pInterface, ref Guid type);

        // Enum Values PInvokes
        [DllImport(_DllImport)]
        internal static extern int EnumValue_GetValue(IntPtr pInterface, ref UInt32 value);

        // PropertyTypeInfo PInvokes
        [DllImport(_DllImport)]
        internal static extern int PropertyTypeInfo_GetPropertyType(IntPtr pObjectTypeInfo, ref UInt32 value);
        [DllImport(_DllImport)]
        internal static extern int PropertyTypeInfo_GetTypeInfo(IntPtr pObjectTypeInfo, ref IntPtr pInterface);

        // Async Operation PInvokes
        [DllImport(_DllImport)]
        internal static extern int AsyncOperation_GetStatus(
            IntPtr pInterface,
            ref UInt32 pValue);

        [DllImport(_DllImport)]
        internal static extern int AsyncOperation_GetResults(
            IntPtr pInterface,
            ref IntPtr pValue);


        [DllImport(_DllImport)]
        internal static extern int AsyncOperation_AddCompleted(
            IntPtr pInterface,
            OnCallbackEventHandler callback,
            ref IntPtr pHandler);
        [DllImport(_DllImport)]
        internal static extern int AsyncOperation_RemoveCompleted(
            IntPtr pInterface,
            IntPtr pHandler);

        [DllImport(_DllImport)]
        internal static extern int AsyncOperation_GetResultTypeInfo(
            IntPtr pInterface,
            ref IntPtr pTypeInfo);

        // Collection Model PInvokes
        [DllImport(_DllImport)]
        internal static extern int ObservableCollection_GetCount(
            IntPtr pInterface,
            ref UInt32 pValue);

        [DllImport(_DllImport)]
        internal static extern int ObservableCollection_GetItem(
            IntPtr pInterface,
            UInt32 index,
            ref IntPtr pValue);

        [DllImport(_DllImport)]
        internal static extern int ObservableCollection_GetItems(
            IntPtr pInterface,
            ref UInt32 size,
            ref IntPtr pValues);

        [DllImport(_DllImport)]
        internal static extern int ObservableCollection_IndexOf(
            IntPtr pInterface,
            IntPtr pValue,
            ref Int32 pIndex);

        [DllImport(_DllImport)]
        internal static extern int ObservableCollection_AddNotifyCollectionChanged(
            IntPtr pInterface,
            OnCallbackEventHandler callback,
            ref IntPtr pHandler);
        [DllImport(_DllImport)]
        internal static extern int ObservableCollection_RemoveNotifyCollectionChanged(
            IntPtr pInterface,
            IntPtr pHandler);

        [DllImport(_DllImport)]
        internal static extern int Enumerable_GetItemTypeInfo(
            IntPtr pInterface,
            ref IntPtr pTypeInfo);

        // NotifyCollectionChangedEventArgs PInvokes
        [DllImport(_DllImport)]
        internal static extern int NotifyCollectionChangedEventArgs_GetAction(IntPtr pInterface, ref UInt32 value);
        [DllImport(_DllImport)]
        internal static extern int NotifyCollectionChangedEventArgs_GetOldStartingIndex(IntPtr pInterface, ref UInt32 value);
        [DllImport(_DllImport)]
        internal static extern int NotifyCollectionChangedEventArgs_GetNewStartingIndex(IntPtr pInterface, ref UInt32 value);

        [DllImport(_DllImport)]
        internal static extern int NotifyCollectionChangedEventArgs_GetOldItems(IntPtr pInterface, ref UInt32 size, ref IntPtr pValues);
        [DllImport(_DllImport)]
        internal static extern int NotifyCollectionChangedEventArgs_GetNewItems(IntPtr pInterface, ref UInt32 size, ref IntPtr pValues);

        // Command Model PInvokes
        [DllImport(_DllImport)]
        internal static extern int Command_AddCanExecuteChanged(
            IntPtr pInterface,
            OnCallbackEventHandler callback,
            ref IntPtr pHandler);
        [DllImport(_DllImport)]
        internal static extern int Command_RemoveCanExecuteChanged(
            IntPtr pInterface,
            IntPtr pHandler);

        [DllImport(_DllImport)]
        internal static extern int Command_GetState(
            IntPtr pInterface,
            ref UInt32 value);
        [DllImport(_DllImport)]
        internal static extern int Command_Execute(
            IntPtr pInterface,
            IntPtr pParameter,
            ref IntPtr pResult);
        [DllImport(_DllImport)]
        internal static extern int Command_CanExecute(
            IntPtr pInterface,
            IntPtr pParameter,
            ref bool bCanExecute);

        [DllImport(_DllImport)]
        internal static extern int Command_GetParameterTypeInfo(
            IntPtr pInterface,
            ref IntPtr pTypeInfo);

        [DllImport(_DllImport)]
        internal static extern int Command_GetResultTypeInfo(
            IntPtr pInterface,
            ref IntPtr pTypeInfo);

        // ObjectDispatch PInvokes
        [DllImport(_DllImport)]
        internal static extern int ObjectDispatch_GetProperty(IntPtr pInterface, UInt32 propertyId, ref IntPtr pValue);
        [DllImport(_DllImport)]
        internal static extern int ObjectDispatch_SetProperty(IntPtr pInterface, UInt32 propertyId, IntPtr pValue);

        [DllImport(_DllImport)]
        internal static extern int ObjectDispatch_Invoke(IntPtr pInterface, UInt32 methodId, UInt32 size, IntPtr[] pParameters, ref IntPtr pResult);

        // PropertyModel PInvokes
        [DllImport(_DllImport)]
        internal static extern int ObservableObject_AddNotifyPropertyChanged(
            IntPtr pInterface,
            OnCallbackEventHandler callback,
            ref IntPtr pHandler);
        [DllImport(_DllImport)]
        internal static extern int ObservableObject_RemoveNotifyPropertyChanged(
            IntPtr pInterface,
            IntPtr pHandler);
        [DllImport(_DllImport)]
        internal static extern int ObservableObject_AddEventModelEvent(
            IntPtr pInterface,
            OnCallbackEventHandler callback,
            ref IntPtr pHandler);
        [DllImport(_DllImport)]
        internal static extern int ObservableObject_RemoveEventModelEvent(
            IntPtr pInterface,
            IntPtr pHandler);


        // PropertyChangedEventArgs PInvokes
        [DllImport(_DllImport)]
        internal static extern int PropertyChangedEventArgs_GetProperty(
            IntPtr pInterface,ref UInt32 propertyId);

        [DllImport(_DllImport)]
        internal static extern int PropertyChangedEventArgs_GetOldValue(
           IntPtr pInterface, ref IntPtr pOldValue);

        [DllImport(_DllImport)]
        internal static extern int PropertyChangedEventArgs_GetNewValue(
           IntPtr pInterface, ref IntPtr pNewValue);

        [DllImport(_DllImport)]
        internal static extern int PropertyChangedEventArgs_GetPropertyName(
            IntPtr pInterface,
            ref IntPtr values);

        // EventModelEventArgs PInvokes
        [DllImport(_DllImport)]
        internal static extern int EventModelEventArgs_GetEvent(
            IntPtr pInterface,
            ref UInt32 eventId);
        [DllImport(_DllImport)]
        internal static extern int EventModelEventArgs_GetEventArgs(
            IntPtr pInterface,
            ref IntPtr pEventArgs);

        // Activation Factory PInvokes
        [DllImport(_DllImport, CharSet = _PlatformCharSet)]
        internal static extern int ActivationFactory_IsFactoryAvailable(String activationId,ref bool result);

        [DllImport(_DllImport, CharSet = _PlatformCharSet)]
        internal static extern int ActivationFactory_GetFactory(String activationId,ref IntPtr pFactory);

        // Dispatcher Adapter PInvokes
        [DllImport(_DllImport)]
        internal static extern int DispatcherAdapter_InvokeDispatchable(
            IntPtr pDelegateDispatchable);
        [DllImport(_DllImport)]
        internal static extern int DispatcherAdapter_CreateDispatcher(
            HasThreadAccessCallback hasThreadAccessCallback,
            RunAsyncCallback runAsyncCallback,
            ref IntPtr ppDispatcher);
    }
}
