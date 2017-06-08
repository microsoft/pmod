/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MethodSupport.cs
****/
using System;
using PropertyModel;

namespace TestModel
{
    public interface IMethodBase
    {
        void Method1();
    }

    public interface IMethodDerived : IMethodBase
    {
        void Method2();
    }

    public interface IMethodSupport
    {
        void VoidEmpty();
        void Void_Int(int value);
        void Void_Object(IObject pObject);
        void Void_Dictionary(IDictionary dictionary);
        //void Void_Struct(ITypeStruct pStruct);

        int ReturnInt();
        int ReturnInt_Int(int value);

        IDictionary ReturnDictionary();

        [MethodAttribute(IsAsync = true)]
        IObject ReturnObjectAsync();

        [MethodAttribute(IsAsync = true)]
        IDictionary ReturnDictionaryAsync();

        [MethodAttribute(IsAsync =true)]
        void VoidAsync();

        [MethodAttribute(IsAsync = true)]
        int VoidAsync_Int(int value);

        [MethodAttribute(IsAsync = true)]
        int ReturnInt_Async();

        [MethodAttribute(IsAsync = true)]
        void Void_ObjectAsync(IObject pObject);

        [MethodAttribute(IsAsync = true)]
        void Void_DictionaryAsync(IDictionary dictionary);


    }


}
