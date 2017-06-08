/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:logger_class.h
****/
#pragma once

#include <foundation/interfaces/logger.h>
#include "foundation_class_base.h"
#include "delegate_base.h"
#include "../foundation_lib_api.h"

namespace foundation
{
    namespace library
    {
        //////////////////////  Core Logger Internal Interfaces ////////////////////////////////
        BEGIN_DECLARE_INTERFACE(ILoggerSerialize, foundation::IUnknown, FOUNDATION_API)
            STDMETHOD(CreateLogParameters)(UINT32 *size, ILogParameterPair ***ppLogParameterPair) = 0;
        END_DECLARE_INTERFACE()

        BEGIN_DECLARE_INTERFACE(ILoggerClass, IFoundationClassBase, FOUNDATION_LIB_API)
        END_DECLARE_INTERFACE()

        BEGIN_DECLARE_INTERFACE(ILoggerDelegate, IDelegateBase<ILogCreatedEventHandler>, FOUNDATION_LIB_API)
        END_DECLARE_INTERFACE()

        BEGIN_DECLARE_INTERFACE(ITextLogListenerDelegate, foundation::IUnknown, FOUNDATION_LIB_API)
            STDMETHOD(LogText)(_In_ HSTRING text) = 0;
        END_DECLARE_INTERFACE()

        BEGIN_DECLARE_INTERFACE(IBinaryLogListenerDelegate, foundation::IUnknown, FOUNDATION_LIB_API)
            STDMETHOD(LogBinary)(_In_ UINT32 size, UINT8 *data) = 0;
        END_DECLARE_INTERFACE()

        BEGIN_DECLARE_INTERFACE(ILogRecordStatics, foundation::IUnknown, FOUNDATION_LIB_API)
            STDMETHOD(ToText)(_In_ ILogParameterPair *pLogParameterPair, HSTRING *text) = 0;
            STDMETHOD(ToText)(UINT32 size,_In_ ILogParameterPair **ppLogParameterPair, HSTRING *text) = 0;
            STDMETHOD(ToText)(_In_ ILogRecord *pLogRecord, HSTRING *text) = 0;
            STDMETHOD(SerializeLogRecord)(_In_ ILogRecord *pLogRecord, _Out_ UINT32 *size, _Outptr_ UINT8 **data) = 0;
            STDMETHOD(DeserializeLogRecord)(UINT32 size, UINT8 *data, _COM_Outptr_ ILogRecord **ppLogRecord) = 0;
            STDMETHOD(SerializeLogRecord)(_In_ ILogRecord *pLogRecord, _In_ IDictionary *pDictionary) = 0;
            STDMETHOD(DeserializeLogRecord)(_In_ IDictionary *pDictionary, _COM_Outptr_ ILogRecord **ppLogRecord) = 0;
        END_DECLARE_INTERFACE()

    }
}

