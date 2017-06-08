/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:TestModelRegisterCallback.cpp
****/
#include "TestModelTypeInfo.g.h"
#include "TestModelInterfaces.g.h"
#include "UnitTestBase.h"


 void RegisterRunSetup()
 {
     TestClassDeclaration::AddRunSetup([](const std::set<std::string> *) {
         TestModel::RegisterTypeInfos();
         TestModel::RegisterModelTypeAdapters();
     });
 }

 static HRESULT dummy = (RegisterRunSetup(), 0);
