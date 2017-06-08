/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ClassFactory.winrt.cs
****/
using System.Windows.Threading;
using propertymodelcore = Microsoft.PropertyModel;
 
namespace CoreInterop
{
    public partial class ClassFactory
    {
        internal static Dispatcher CurrentDispatcher
        {
            get
            {
                return System.Windows.Deployment.Current.Dispatcher;
            }
        }

        internal static System.Guid GetTypeFromObjectTypeInfo(propertymodelcore.IObjectTypeInfo objectTypeInfo)
        {
            return objectTypeInfo.IIdType;
        }

        internal IObjectAdapter CreateAsyncOperationAdapter(propertymodelcore.IAsyncOperation asyncOperationAdapter)
        {
            propertymodelcore.IAsyncOperationTypeInfo typeInfo=
            asyncOperationAdapter.ObjectTypeInfo as propertymodelcore.IAsyncOperationTypeInfo;

            System.Type resultType = GetTypeFromPropertyType(
                typeInfo.ResultTypeInfo.PropertyType);

            System.Type asyncOperationType = typeof(AsyncOperation<>).MakeGenericType(resultType);
            return (IObjectAdapter)ActivateObjectInstance(asyncOperationAdapter, asyncOperationType);
        }

        internal IObjectAdapter CreateCollectionModelAdapter(propertymodelcore.ICollectionModel collectionModelAdapter)
        {
            propertymodelcore.ICollectionModelInfo typeInfo =
            collectionModelAdapter.ObjectTypeInfo as propertymodelcore.ICollectionModelInfo;

            System.Type resultType = GetTypeFromPropertyType(
                typeInfo.ItemTypeInfo.PropertyType);

            System.Type collectionModelType = typeof(CollectionModel<>).MakeGenericType(resultType);
            return (IObjectAdapter)ActivateObjectInstance(collectionModelAdapter, collectionModelType);
        }

        private static System.Type GetTypeFromPropertyType(Windows.Foundation.PropertyType propertyType)
        {
            switch (propertyType)
            {
                case Windows.Foundation.PropertyType.Int16:
                    return typeof(short);
                case Windows.Foundation.PropertyType.UInt16:
                    return typeof(ushort);
                case Windows.Foundation.PropertyType.String:
                    return typeof(string);
                case Windows.Foundation.PropertyType.Int32:
                    return typeof(int);
                case Windows.Foundation.PropertyType.UInt32:
                    return typeof(uint);
                case Windows.Foundation.PropertyType.Int64:
                    return typeof(long);
                case Windows.Foundation.PropertyType.UInt64:
                    return typeof(ulong);
                case Windows.Foundation.PropertyType.Boolean:
                    return typeof(bool);
                case Windows.Foundation.PropertyType.UInt8:
                    return typeof(byte);
                case Windows.Foundation.PropertyType.Inspectable:
                    return typeof(object);
                case Windows.Foundation.PropertyType.Empty:
                    return typeof(VoidModelType);
            }
            throw new System.NotImplementedException();
        }
        internal static object ActivateObjectInternal(System.Guid iidType, object context)
        {
            //TODO: we need to implement a winrt access to IObjectStatics ?
            return null;
        }

        internal static void LogMessage(propertymodelcore.LoggingLevel level, uint messageId, string text, params object[] parameters)
        {
            var logger = propertymodelcore.PropertyModelInstance.Logger;

            const ulong _Foundation_CategoryEnum = 0x10400000000;
            const ulong _Message_LogRecordEnum = 0x10500000000;
            const uint _Foundation_Category_Message_Id = 0x1040000;

            uint categoryEnumTypeId = (uint)(_Foundation_CategoryEnum >> 32);

            propertymodelcore.ILogCategory logCategory = logger.GetCategory(
                new uint[] { categoryEnumTypeId });

            if (logCategory.IsLevelEnabled(
                level,
                _Foundation_Category_Message_Id,
                System.Guid.Empty))
            {
                propertymodelcore.LogRecord logRecord = new propertymodelcore.LogRecord(
                    level,
                    (ulong)(_Foundation_CategoryEnum + 0),
                    logCategory,
                    new propertymodelcore.LogParameter[]
                {
                    // Id
                    new propertymodelcore.LogParameter(_Message_LogRecordEnum + 0, messageId),
                    // Text
                    new propertymodelcore.LogParameter(_Message_LogRecordEnum + 1, text),
                    // Id
                    new propertymodelcore.LogParameter(_Message_LogRecordEnum + 2, parameters),
                });
                logger.Log(null, logRecord);
            }
        }

    }
}
