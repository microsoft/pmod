/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:TestModelRegisterIf.cpp
****/
#include "TestModelTypeInfo.g.h"
#include "TestModelInterfaces.g.h"


 HRESULT RegisterTypesIf()
 {
     static bool registerTypes = false;
     if(!registerTypes)
     {
         registerTypes = true;
         TestModel::RegisterTypeInfos();
         TestModel::RegisterModelTypeAdapters();
     }
     return S_OK;
 }
