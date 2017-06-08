/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MainWindow.xaml.cs
****/
using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace DemoAppUI
{
    public class VisibilityConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            bool visibility = (bool)value;
            return visibility ? Visibility.Visible : Visibility.Collapsed;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }

    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
            this.DataContext = App.AppDemo;
            lbCurrentItems.SelectionChanged += lbCurrentItems_SelectionChanged;
        }

        void lbCurrentItems_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            btnRemove.IsEnabled = lbCurrentItems.SelectedItem != null;
        }

        private void btnRemove_Click(object sender, RoutedEventArgs e)
        {
            DemoApp.Item item = lbCurrentItems.SelectedItem as DemoApp.Item;
            App.AppDemo.RemoveItem.Execute(item);
        }

        private void btnCollect_Click(object sender, RoutedEventArgs e)
        {
            System.GC.Collect();
            System.GC.Collect();
        }

    }
}
