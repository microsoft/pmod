/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:logger_library.h
****/
#pragma once

#include <foundation/assert.h>
#include <iostream>

#include <foundation/ctl/com_library.h>
#include <foundation/com_ptr.h>
#include "logger_util.h"
#include <foundation/event_util.h>

namespace foundation
{
    //------------------------------------------------------------------------------
    // Class:   LoggerListenerBase
    //
    // Purpose: Class for Foundation Logger Listener
    //
    //------------------------------------------------------------------------------
    class LoggerListenerBase :
        public BaseListener
        <
            LoggerListenerBase,
            ILogger,
            ILogCreatedEventHandler,
            ILogCreatedEventArgs,
            ILogCreatedEventSource
        >
    {
    public:
        static HRESULT GetEventSource(ILogger *pSource, ILogCreatedEventSource **ppEventSource)
        {
            return pSource->GetLogCreatedEventSource(ppEventSource);
        }
    };

    typedef
        SingleSourceEventHandlerConnection
        <
        ILogger,
        ILogCreatedEventHandler,
        ILogCreatedEventArgs,
        LoggerListenerBase
        > log_created_eventhandler_connection;

    typedef
        MultiSourceEventHandlerConnection
        <
        ILogger,
        ILogCreatedEventHandler,
        ILogCreatedEventArgs,
        LoggerListenerBase
        > multi_source_log_created_eventhandler_connection;
//////////////////////////////////////////////////////////////////////////
// LogListenerBase Class
//////////////////////////////////////////////////////////////////////////
template <class TClass,class TLogDelegate>
class LogListenerBase :
    public ctl::ComAggregateInspectableBase,
    public TLogDelegate
{
public:
    template <class T>
    static HRESULT CreateInstance(_COM_Outptr_ T **ppT)
    {
        IFCPTR_ASSERT(ppT);
        T *pT = nullptr;
        IFR_ASSERT(ctl::inspectable::CreateInstance(&pT));
        IFR_ASSERT((static_cast<TClass *>(pT))->_CreateInnerLogListener());
        foundation_assert(pT->GetInnerRaw());
        IFR_ASSERT(foundation::QueryWeakReference(
            pT->GetInnerRaw(),
            &pT->m_pLogCreatedEventHandler));
        *ppT = pT;
        return S_OK;
    }

    HRESULT Attach(ILogger *pLogger)
    {
        IFCPTR_ASSERT(pLogger);
        return LoggerListenerBase::AttachEventHandler(pLogger, m_pLogCreatedEventHandler);
    }
    HRESULT Detach(ILogger *pLogger)
    {
        IFCPTR_ASSERT(pLogger);
        return LoggerListenerBase::DetachEventHandler(pLogger, m_pLogCreatedEventHandler);
    }
protected:
    typedef ctl::ComAggregateInspectableBase _BaseType;
    LogListenerBase():
        m_pLogCreatedEventHandler(nullptr)
    {
    }

    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
    {
        if (iid == TLogDelegate::GetIID())
        {
            *ppInterface = static_cast<TLogDelegate *>(this);
        }
        else
        {
            return _BaseType::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }
protected:
    ILogCreatedEventHandler     *m_pLogCreatedEventHandler;
};
//////////////////////////////////////////////////////////////////////////
// TextLogListenerBase Class
//////////////////////////////////////////////////////////////////////////
class TextLogListenerBase :
    public LogListenerBase<TextLogListenerBase,library::ITextLogListenerDelegate>
{
public:
    HRESULT _CreateInnerLogListener()
    {
        IFR_ASSERT(
            logger_util::GetLoggerFactory()->CreateTextLogListener(
            static_cast<foundation::ctl::ComInspectableBase *>(this),
            this->GetInnerAddressOf()));
        return S_OK;
    }
protected:
    STDMETHOD (LogText)(_In_ HSTRING text)
    {
        return this->LogTextInternal(text);
    }

    virtual HRESULT LogTextInternal(_In_ HSTRING text) = 0;
};

//////////////////////////////////////////////////////////////////////////
// ConsoleLogListener Class
//////////////////////////////////////////////////////////////////////////
class ConsoleLogListener : public TextLogListenerBase
{
protected:
    HRESULT LogTextInternal(_In_ HSTRING text) override
    {
        LPCSTR_t pstr_t = _pal_GetStringRawBuffer(text,nullptr);
#if defined(_WINDOWS)
#ifdef __UTF16_STRINGS
        wprintf(pstr_t);
#else
        printf(pstr_t);
#endif
        fflush(stdout);
#else
        _pal_Trace(pstr_t);
#endif
        return S_OK;
    }
};
//////////////////////////////////////////////////////////////////////////
// TraceLogListener Class
//////////////////////////////////////////////////////////////////////////
class TraceLogListener : public TextLogListenerBase
{
protected:
    HRESULT LogTextInternal(_In_ HSTRING text) override
    {
        _pal_Trace(_pal_GetStringRawBuffer(text,nullptr));
        return S_OK;
    }
};
//////////////////////////////////////////////////////////////////////////
// StreamTextLogListener Class
//////////////////////////////////////////////////////////////////////////
class StreamTextLogListener : public TextLogListenerBase
{
public:
    void AttachStream(std::wostream *ostream)
    {
        _wostream = ostream;
    }
protected:
    StreamTextLogListener():
        _wostream(nullptr)
    {
    }

    HRESULT LogTextInternal(_In_ HSTRING text) override
    {
        if(_wostream)
        {
            *_wostream << _pal_GetStringRawBuffer(text,nullptr);
        }
        return S_OK;
    }
private:
    std::wostream *_wostream;
};
//////////////////////////////////////////////////////////////////////////
// BinaryLogListenerBase Class
//////////////////////////////////////////////////////////////////////////
class BinaryLogListenerBase :
    public LogListenerBase<BinaryLogListenerBase,library::IBinaryLogListenerDelegate>
{
public:
    HRESULT _CreateInnerLogListener()
    {
        IFR_ASSERT(
            logger_util::GetLoggerFactory()->CreateBinaryLogListener(
            static_cast<foundation::ctl::ComInspectableBase *>(this),
            this->GetInnerAddressOf()));
        return S_OK;
    }
protected:
    STDMETHOD (LogBinary)(_In_ UINT32 size,UINT8 *data)
    {
        return this->log_binary_internal(size,data);
    }

    virtual HRESULT log_binary_internal(_In_ UINT32 size,UINT8 *data) = 0;
};
//////////////////////////////////////////////////////////////////////////
// StreamBinaryLogListener Class
//////////////////////////////////////////////////////////////////////////
class StreamBinaryLogListener : public BinaryLogListenerBase
{
public:
    void AttachStream(std::ostream *ostream)
    {
        _ostream = ostream;
    }
protected:
    StreamBinaryLogListener():
        _ostream(nullptr)
    {
    }
    HRESULT log_binary_internal(_In_ UINT32 size,UINT8 *data) override
    {
        if(_ostream)
        {
            // serialize the size of the record first
            UINT16 logRecordSize = (UINT16)size;
            _ostream->write((const char *)&logRecordSize,(std::streamsize)sizeof(UINT16));
            // now the Log Record binary itself
            _ostream->write((const char *)data,(std::streamsize)size);
        }
        return S_OK;
    }
private:
    std::ostream *_ostream;
};

}

