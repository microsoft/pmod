/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODAdapterLogCategory.cpp
****/


#include "PMODAdapterLogCategory.h"
#include "PMODAdapterLoggerTypeInfo.h"

#include <foundation/ctl/com_library.h>
#include <foundation/hstring_wrapper.h>
#include <foundation/library/logger_util.h>


using namespace foundation;

extern HRESULT  STDAPICALLTYPE register_nsadapter_logger_typeinfo(void);

CPMODAdapterLogger::CPMODAdapterLogger()
{
    register_nsadapter_logger_typeinfo();
    // create our logger
    logger_util::create_foundation_logger(
                                          nsadapter::NSAdapter_CategoryEnum::TypeId,
                                          LoggerOptions::None,
                                          _spNSAdapterLogger.GetAddressOf());
}

CPMODAdapterLogger& CPMODAdapterLogger::GetInstance()
{
    static CPMODAdapterLogger _instance;
    return _instance;
}

CPMODAdapterLogCategory::CPMODAdapterLogCategory()
{
    
    // PMODFoundation
    logger_util::GetCategory(CPMODAdapterLogger::GetPMODAdapterLogger(),
                              _spFoundationLogCategory.GetAddressOf(),
                              nsadapter::NSFoundation_CategoryEnum::TypeId,
                              -1);
    // PMODObjectBase
    logger_util::GetCategory(CPMODAdapterLogger::GetPMODAdapterLogger(),
                              _spObjectBaseLogCategory.GetAddressOf(),
                              nsadapter::NSFoundation_CategoryEnum::TypeId,
                              nsadapter::NSObjectBase_CategoryEnum::TypeId,
                              -1);
    // PMODObject
    logger_util::GetCategory(CPMODAdapterLogger::GetPMODAdapterLogger(),
                              _spObservableObjectLogCategory.GetAddressOf(),
                              nsadapter::NSFoundation_CategoryEnum::TypeId,
                              nsadapter::NSObjectBase_CategoryEnum::TypeId,
                              nsadapter::NSPropertyModel_CategoryEnum::TypeId,
                              -1);
    // PMODCollection
    logger_util::GetCategory(CPMODAdapterLogger::GetPMODAdapterLogger(),
                              _spObservableCollectionLogCategory.GetAddressOf(),
                              nsadapter::NSFoundation_CategoryEnum::TypeId,
                              nsadapter::NSObjectBase_CategoryEnum::TypeId,
                              nsadapter::NSCollectionModel_CategoryEnum::TypeId,
                              -1);
   
    // PMODAdapter Factory
    logger_util::GetCategory(CPMODAdapterLogger::GetPMODAdapterLogger(),
                                _spAdapterFactoryLogCategory.GetAddressOf(),
                                nsadapter::NSFactory_CategoryEnum::TypeId,
                                -1);
}

CPMODAdapterLogCategory& CPMODAdapterLogCategory::GetInstance()
{
    static CPMODAdapterLogCategory _instance;
    return _instance;
}

