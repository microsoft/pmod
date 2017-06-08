/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MainActivity.cs
****/
using System;

using Android.App;
using Android.Content.PM;
using Android.Runtime;
using Android.Views;
using Android.Widget;
using Android.OS;
using System.Runtime.InteropServices;

namespace DemoAppUI.Droid
{
    [Activity(Label = "DemoAppUI", Icon = "@drawable/icon", MainLauncher = true, ConfigurationChanges = ConfigChanges.ScreenSize | ConfigChanges.Orientation)]
    public class MainActivity : global::Xamarin.Forms.Platform.Android.FormsApplicationActivity
    {
        [DllImport("demoapp_impl")]
        static extern int CreateDemoAppImpl(ref IntPtr pDemoAppInterface);

        protected override void OnCreate(Bundle bundle)
        {
            base.OnCreate(bundle);

            IntPtr pDemoAppInterface = IntPtr.Zero;
            CreateDemoAppImpl(ref pDemoAppInterface);

            global::Xamarin.Forms.Forms.Init(this, bundle);
            LoadApplication(new App(pDemoAppInterface));
        }
    }
}


