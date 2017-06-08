/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:DemoAppPage.xaml.cs
****/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Xamarin.Forms;

namespace DemoAppUI
{
    public partial class DemoAppPage : ContentPage
    {
        public DemoAppPage()
        {
            try
            {
                InitializeComponent();
            }
            catch (Exception e)
            {
                var txt = e.Message;
            }
            this.btnAdd.SetBinding(Button.CommandParameterProperty, new Binding("Text") { Source = tbName });
            this.btnCollect.Clicked += BtnCollect_Clicked;
        }

        private void BtnCollect_Clicked(object sender, EventArgs e)
        {
            System.GC.Collect();
            System.GC.Collect();
        }
    }
}
