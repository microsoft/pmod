/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:TextLogListener.cpp
****/
#include "pch.h"

#include "TextLogListener.h"
#include "LogRecordStatics.h"

using namespace foundation;

STDMETHODIMP CTextLogListener::Invoke(foundation::IUnknown* pSender, ILogCreatedEventArgs* pArgs)
{
    foundation::ComPtr<ILogRecord> spLogRecord;
    IFR_ASSERT(pArgs->GetLogRecord(spLogRecord.GetAddressOf()));

    string_t text;
    IFR_ASSERT(CLogRecordStatics::ToText(spLogRecord, text));

    library::ITextLogListenerDelegate *pTextLogListenerDelegate = nullptr;
    if (SUCCEEDED(foundation::QueryWeakReference(static_cast<foundation::ctl::ComInspectableBase *>(this), &pTextLogListenerDelegate)))
    {
        HSTRING_HEADER hHeader;
        foundation::HStringRef hstr;
        _pal_CreateStringReference(text.c_str(), (UINT32)text.size(), &hHeader, hstr.GetAddressOf());
        return pTextLogListenerDelegate->LogText(hstr);
    }
    return S_OK;
}
