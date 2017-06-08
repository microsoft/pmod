/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PropertyModelLibraryUtil.cs
****/
namespace Microsoft.PropertyModel.Library
{
    using System;
    using System.Collections.Generic;
    using System.Diagnostics;
    using System.Reflection;
    using Windows.Foundation;

    static class UtilHelper
    {
        public static T AssertCast<T>(this object value)
        {
            Debug.Assert(null == value || value is T, "null == value || value is T");
            return (T)value;
        }

        public static T AssertCastNotNull<T>(this object value)
        {
            Debug.Assert(value != null, "value != null");
            return value.AssertCast<T>();
        }
    }

    delegate object ExecuteProperty(uint propertyid, object parameter);
    delegate bool CanExecuteProperty(uint propertyid,object parameter);

    static class PropertyModelHelper
    {
        internal static Microsoft.PropertyModel.PropertyTypeInfo CreatePropertyTypeInfo(
                                     PropertyType type,
                                     Guid iidModelType)
        {
            Microsoft.PropertyModel.PropertyTypeInfo pi;
            pi.PropertyType = type;
            pi.ObjectType = iidModelType;

            return pi;
        }

        internal static Microsoft.PropertyModel.PropertyInfo CreatePropertyInfo(
                                     uint id,
                                     string name,
                                     uint flags,
                                     PropertyType type,
                                     Guid iidObjectType)
        {
            Microsoft.PropertyModel.PropertyInfo pi;
            pi.Id = id;
            pi.Name = name;
            pi.Flags = flags;
            pi.PropertyType = type;
            pi.ObjectType = iidObjectType;

            return pi;
        }

        internal static Microsoft.PropertyModel.ParameterInfo CreateParameterInfo(
                                     string parameterName,
                                     PropertyType type,
                                     Guid iidObjectType)
        {
            Microsoft.PropertyModel.ParameterInfo pi;
            pi.Name = parameterName;
            pi.PropertyType = type;
            pi.ObjectType = iidObjectType;
            return pi;
        }

//#if !WINDOWS_PHONE
//        internal static async void InvokeAsync(this IModel model, Action action)
//        {
//            Windows.UI.Core.CoreDispatcher dispatcher = Windows.UI.Xaml.Window.Current.Dispatcher;
//            await dispatcher.RunAsync(Windows.UI.Core.CoreDispatcherPriority.Normal, () =>
//            {
//                action();
//            });
//        }
//#endif

        internal static ObservableObjectOptions ToFreeThreadApartmentOptions(this ObservableObjectOptions options)
        {
            return (ObservableObjectOptions)((int)options | (int)CommonModelOptions.IsFreeThreadApartment);
        }

        internal static void SetPropertyInternalHelper(
            this IObservableObjectClass propertyModel,
            uint propertyId, 
            object value)
        {
            if (value != null && value.GetType().GetTypeInfo().IsEnum)
            {
                value = Convert.ToInt32(value);
            }
            propertyModel.SetPropertyInternal(propertyId, value);
        }

        internal static T GetPropertyValue<T>(
            this IObservableObject propertyModel,
            uint propertyId)
        {
            object value = propertyModel.GetProperty(propertyId);
            if (typeof(T).GetTypeInfo().IsEnum)
            {
                return (T)Enum.ToObject(typeof(T), value);
            }
            return value.AssertCast<T>();
        }

        internal static object InvokMethodHelper(
            this object instance, 
            string methodName, 
            params object[] parameters)
        {
            System.Reflection.MethodInfo methodInfo = instance.GetType().GetTypeInfo().GetDeclaredMethod(methodName);
            if (methodInfo == null)
            {
                throw new NotImplementedException();
            }
            return methodInfo.Invoke(instance, parameters);
        }

        internal static Command CreatePropertyCommand(
            CommandOptions options,
            Guid iidType,
            uint propertyId,
            ExecuteProperty executeCallback,
            CanExecuteProperty canExecuteCallback)
        {
            var commandModel = new Command(options, iidType);
            if(canExecuteCallback != null)
            {
                commandModel.CanExecuteDelegate = (object parameter) =>
                {
                    return canExecuteCallback(propertyId, parameter);
                };
            }
            if (executeCallback != null)
            {
                commandModel.ExecuteDelegate = (object parameter) =>
                {
                    return executeCallback(propertyId, parameter);
                };
            }
            return commandModel;
        }

        internal static Command CreatePropertyCommand(
            CommandOptions options,
            Guid iidType,
            uint propertyId,
            Tuple<ExecuteProperty,CanExecuteProperty> commandPropertyCallbacks)
        {
            return CreatePropertyCommand(
                    options,
                    iidType,
                    propertyId,
                    commandPropertyCallbacks.Item1,
                    commandPropertyCallbacks.Item2);
        }

        internal static T GetAsyncCommandParameter<T>(this object parameter)
        {
            return parameter.AssertCast<object[]>()[0].AssertCast<T>();
        }

        internal static AsyncOperation GetAsyncCommandOperation(this object parameter)
        {
            return parameter.AssertCast<object[]>()[1].AssertCast<AsyncOperation>();
        }
    }

    class MethodCallback<T>
    {
        protected T _callback;

        internal MethodCallback(T callback)
        {
            _callback = callback;
        }

        internal object InvokeDelegate(int methodId, object[] parameters)
        {
            return null;
        }
    }

    class CommandCallback<T>
    {
        protected T _callback;

        internal CommandCallback(T callback)
        {
            _callback = callback;
        }

        internal object ExecuteProperty(uint propertyid, object parameter)
        {
            return null;
        }

        internal bool CanExecuteProperty(uint propertyid, object parameter)
        {
            return true;
        }

        internal static void InitializeCommandProperties(
            IObservableObjectClass model,
            Tuple<ExecuteProperty, CanExecuteProperty> callbacks)
        {
        }
    }
}
