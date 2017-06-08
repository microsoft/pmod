/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoLogRecordFactory.cpp
****/

#include "pch.h"

#include "MoLogRecordFactory.h"

#include <pmod/library/library_util.h>
#include <pmod/errors.h>
#include <foundation/library/logger_util.h>
#include "MoEnumValueInterop.h"

STDMETHODIMP MoLogRecordFactory::CreateInstance(
    LoggingLevel level,
    IInspectable *pMessage,
    ILogCategory *pLogCategory,
    UINT32 length,
    ILogParameter **logParameters,
    ILogRecord **instance)
{
    IFCPTR_ASSERT(pMessage);
    IFCPTR_ASSERT(pLogCategory);
    IFCPTR_ASSERT(logParameters);

    foundation::ComPtr<foundation::IEnumValue> spEnumValue;
    IFR_ASSERT(ObjectToEnumValue(pMessage, spEnumValue.GetAddressOf()));

    foundation::ComPtr<foundation::ILogCategory> spLogCategory;
    IFR_ASSERT(foundation::QueryInterface(pLogCategory,spLogCategory.GetAddressOf()));

    std::vector<foundation::ComPtr<foundation::ILogParameterPair>> logParametersVector;
    logParametersVector.resize(length);

    for(UINT32 index = 0;index < length;++index)
    {
        IFR_ASSERT(foundation::QueryInterface(logParameters[index],logParametersVector[index].GetAddressOf()));
    }

    foundation::ComPtr<foundation::ILogRecord> spLogRecord;
    IFR_ASSERT(foundation::logger_util::GetLoggerFactory()->CreateLogRecord(
        (foundation::LoggingLevel)level,
        spEnumValue,
        spLogCategory,
        length,
        logParametersVector.front().GetAddressOfPtr(),
        spLogRecord.GetAddressOf()));
    IFR_ASSERT(foundation::QueryInterface(spLogRecord,instance));
    return S_OK;
}
