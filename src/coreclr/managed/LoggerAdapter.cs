/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:LoggerAdapter.cs
****/
using System;
using System.Runtime.InteropServices;

namespace Microsoft.PropertyModel
{
    public enum LoggerLevel
    {
        Off,
        Fatal,
        Error,
        Warning,
        Info,
        Debug,
        Trace
    }

    public class LoggerAdapter : InspectableAdapter
    {


        private static Lazy<LoggerAdapter> LazyFoundationInstance;
        static LoggerAdapter()
        {
            LazyFoundationInstance = new Lazy<LoggerAdapter>(() =>
            {
                IntPtr pInterface = IntPtr.Zero;

                PInvokeUtils.ThrowIfResult(NativeMethods.Logger_GetFoundationInstance(ref pInterface));
                return new LoggerAdapter(pInterface);
            });
        }

        public static bool IsAvailable()
        {
            bool isAvailable = false;
            PInvokeUtils.ThrowIfResult(NativeMethods.Logger_IsAvailable(ref isAvailable));
            return isAvailable;
        }

        public static LoggerAdapter FoundationInstance
        {
            get
            {
                return LazyFoundationInstance.Value;
            }
        }
        public static void LogMessage(LoggerLevel level,uint messageId,string text,InspectableAdapter pParameter)
        {
            PInvokeUtils.ThrowIfResult(NativeMethods.Logger_LogMessage((uint)level, messageId, text, pParameter != null ? pParameter.Interface:IntPtr.Zero ));
        }

        public LoggerAdapter(IntPtr pLoggerInterface):base(pLoggerInterface)
        {
        }

        public LoggerLevel GetCategoryLevel(string categoryPath)
        {
            UInt32 level = 0;
            PInvokeUtils.ThrowIfResult(NativeMethods.Logger_GetCategoryLevel(this.Interface, categoryPath, ref level));
            return (LoggerLevel)level;
        }

        public void SetCategoryLevel(LoggerLevel level,string categoryPath)
        {
            PInvokeUtils.ThrowIfResult(NativeMethods.Logger_SetCategoryLevel(this.Interface, (uint)level, categoryPath));
        }

    }
}
