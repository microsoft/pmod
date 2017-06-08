/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:BinaryLogListener.cpp
****/
#include "pch.h"

#include "BinaryLogListener.h"
#include "LogRecordStatics.h"

using namespace foundation;

STDMETHODIMP CBinaryLogListener::Invoke(_In_ foundation::IUnknown* pSender, _In_ ILogCreatedEventArgs* pArgs)
{
    foundation::ComPtr<ILogRecord> spLogRecord;
    IFR_ASSERT(pArgs->GetLogRecord(spLogRecord.GetAddressOf()));

    std::stringstream os;
    IFR_ASSERT(CLogRecordStatics::SerializeLogRecord(spLogRecord, os));

    library::IBinaryLogListenerDelegate *pBinaryLogListenerDelegate = nullptr;
    if(SUCCEEDED(foundation::QueryWeakReference(static_cast<foundation::ctl::ComInspectableBase *>(this),&pBinaryLogListenerDelegate)))
    {
        std::string osstr = os.str();
        return pBinaryLogListenerDelegate->LogBinary((UINT32)osstr.size(),(UINT8 *)osstr.data());
    }
    return S_OK;
}


