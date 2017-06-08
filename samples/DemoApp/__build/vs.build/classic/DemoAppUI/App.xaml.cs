/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:App.xaml.cs
****/
#define NOLIB_IMPL
using DemoApp.Net;
using Microsoft.PropertyModel;
using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Runtime.InteropServices;
using System.Threading.Tasks;
using System.Windows;

namespace DemoAppUI
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
        static internal DemoApp.AppDemo AppDemo { get; private set; }

        protected override void OnStartup(StartupEventArgs e)
        {
#if NOLIB_IMPL
            AppDemo = DemoAppFactory.CreateAppDemoImpl();
#else
            AppDemo = DemoAppFactory.CreateAppDemo();
#endif
#if DEBUG
            if (LoggerAdapter.IsAvailable())
            {
                LoggerAdapter.FoundationInstance.SetCategoryLevel(LoggerLevel.Debug, null);
            }
#endif
            base.OnStartup(e);
        }
    }
}
