/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:App.cs
****/
using DemoApp;
using Microsoft.PropertyModel;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Xamarin.Forms;

namespace DemoAppUI
{
    public class App : Application
    {
        internal AppDemo AppDemo { get; private set; }

        public App(IntPtr pDemoAppInterface)
        {
            // The root page of your application
            MainPage = new DemoAppPage();
            ClassFactory classFactory = new ClassFactory(ClassFactoryTypes.Types);
            this.AppDemo = classFactory.GetOrCreateObject(new ObservableObjectAdapter(pDemoAppInterface)) as AppDemo;
            this.MainPage.BindingContext = this.AppDemo;
        }

        protected override void OnStart()
        {
            // Handle when your app starts
        }

        protected override void OnSleep()
        {
            // Handle when your app sleeps
        }

        protected override void OnResume()
        {
            // Handle when your app resumes
        }
    }
}
