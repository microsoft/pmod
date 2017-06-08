/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:TestModelRegister.cpp
****/
#include "TestModelTypeInfo.g.h"
#include "TestModelInterfaces.g.h"

EXTERN_C MODEL_API HRESULT STDAPICALLTYPE LoadLibraryStub()
{
    return S_OK;
}

 HRESULT RegisterTypes()
 {
     TestModel::RegisterTypeInfos();
     TestModel::RegisterModelTypeAdapters();
     return S_OK;
 }

 static HRESULT dummy = (RegisterTypes(), 0);
