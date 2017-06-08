/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:DemoAppFactory.cs
****/
using Microsoft.PropertyModel;
using System;
using System.Runtime.InteropServices;

namespace DemoApp.Net
{
    public static class DemoAppFactory
    {
        [DllImport("DemoApp.dll")]
        private static extern int CreateDemoApp(ref IntPtr pInterface);
        [DllImport("DemoAppImpl.dll")]
        private static extern int CreateDemoAppImpl(ref IntPtr pInterface);


        public static DemoApp.AppDemo CreateAppDemo()
        {
            IntPtr pInterface = IntPtr.Zero;
            CreateDemoApp(ref pInterface);
            return (DemoApp.AppDemo)CreateDemoAppClassFactory().GetOrCreateObject(new ObservableObjectAdapter(pInterface));
        }

        public static DemoApp.AppDemo CreateAppDemoImpl()
        {
            IntPtr pInterface = IntPtr.Zero;
            CreateDemoAppImpl(ref pInterface);
            return (DemoApp.AppDemo)CreateDemoAppClassFactory().GetOrCreateObject(new ObservableObjectAdapter(pInterface));
        }

        private static ClassFactory CreateDemoAppClassFactory()
        {
            ClassFactory classFactory = new ClassFactory(DemoApp.ClassFactoryTypes.Types);

            // define a UI dispatcher for our Class Factory
            System.Windows.Threading.Dispatcher dispatcher = System.Windows.Threading.Dispatcher.CurrentDispatcher;
            var factoryDispatcher = new Action<Action, bool>((action, checkAccess) =>
            {
                if (!checkAccess || !dispatcher.CheckAccess())
                {
                    dispatcher.BeginInvoke(action);
                }
                else
                {
                    action();
                }
            });
            classFactory.Dispatcher = factoryDispatcher;
            return classFactory;
        }
    }
}
