/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ProxyTest.cs
****/
using Microsoft.PropertyModel.Library;
using System;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using Xunit;

namespace Microsoft.PropertyModel.Proxy.Test
{
    public class ProxyTest
    {
        static ProxyTest()
        {
            // Register Proxy projection
            Class.Register();
            // Register TestModel projection
            TestModel.Class.Register();
        }

        [Fact]
        public void TestProxyFactory()
        {
            var proxyFactory = CreateProxyObjectFactory(ProxyObjectFactoryOptions.NoPurgeState, null, null);

            var collection = new ObservableCollection(ObservableCollectionOptions.None);
            var proxy = proxyFactory.ToProxyObject(collection);

            var typeProperties = new TestModel.TypeProperties();
            proxy = proxyFactory.ToProxyObject(typeProperties);
            var typePropertiesProxy = proxy as TestModel.ITypeProperties;

            typeProperties.IntProperty = 200;
            Assert.Equal(typePropertiesProxy.IntProperty, 200);
            PropertyChangedEventArgs lastEventArgs = null;
            PropertyChangedEventHandler eventHandler = null;
            eventHandler = (object sender, PropertyChangedEventArgs e) =>
            {
                lastEventArgs = e;
            };
            typePropertiesProxy.PropertyChanged += eventHandler;
            typeProperties.IntProperty = 300;
            Assert.NotNull(lastEventArgs);
            Assert.Equal(lastEventArgs.NewValue, 300);
        }

        [Fact]
        public void TestWithWorkingDisptahcer()
        {
            var workingDispatcher = Dispatcher.CreateThreadingPoolDispatcher();

            var proxyFactory = CreateProxyObjectFactory(ProxyObjectFactoryOptions.NoPurgeState, workingDispatcher, null);
            var typeProperties = new TestModel.TypeProperties();
            var typePropertiesProxy = proxyFactory.ToProxyObject(typeProperties) as TestModel.ITypeProperties;

            var notifyReceived = new EventWaitHandle(false, EventResetMode.AutoReset);
            PropertyChangedEventHandler eventHandler = null;
            eventHandler = (object sender, PropertyChangedEventArgs e) =>
            {
                notifyReceived.Set();
            };
            typePropertiesProxy.PropertyChanged += eventHandler;
            typeProperties.IntProperty = 100;
            Assert.True(notifyReceived.WaitOne(1000));
        }

        static IProxyObjectFactory CreateProxyObjectFactory(ProxyObjectFactoryOptions options,object workingDispatcher, object eventDispatcher)
        {
            var proxyFactoryFactory = Model.GetActivationFactory("pmod.proxy.ProxyObjectFactory") as IProxyObjectFactoryFactory;
            var proxyFactory = proxyFactoryFactory.Create(options, workingDispatcher, eventDispatcher);
            return proxyFactory;
        }
    }
}
