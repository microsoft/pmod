/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:Logger.h
****/

#pragma once

#include <foundation/library/foundation_library.h>

#include "FoundationTypes.h"


#include "FoundationBase.h"
#include "LogCategory.h"

//------------------------------------------------------------------------------
// Class:   CLogger
//
// Purpose: Implements the ICoreLogger interface
//
//------------------------------------------------------------------------------
typedef _FoundationBase
    <
    foundation::ILogger,
    foundation::library::ILoggerClass,
    foundation::library::ILoggerDelegate,
    foundation::library::LoggerCreateParameters,
    foundation::ILogCreatedEventSource,
    foundation::ILogCreatedEventHandler,
    foundation::IUnknown,
    foundation::ILogCreatedEventArgs,
    false
    >
    _LoggerBase;

class CLogger:
    public _LoggerBase
{
public:
    static CLogger *GetLogger();

    static const CLogCategory *GetCategory(_In_ UINT32 categoryTypeId,...);

    static HRESULT CreateInstance(
        _In_ const foundation::library::LoggerCreateParameters *pCoreLoggerParam,
        _In_opt_ foundation::IInspectable *pOuter,
        _COM_Outptr_ foundation::IInspectable **ppNewInstance);

    HRESULT _Initialize(const foundation::library::LoggerCreateParameters *pCoreLoggerParam);

    // support for external callback logger type info
    static HRESULT register_factory_type_info(foundation::library::IDelegateDispatchable *pCallback);

    static bool IsAvailable();
    static void _ForceUnavailable();

private:
    static _Dispatchable_ptr_vector_type& GetCallbacks();
protected:
    CLogger():
        _is_fire_logging(false)
    {}
    // Interface ICoreLogger
    STDMETHOD (ConfigureCategories)(foundation::IDictionary *pConfigParameters);
    STDMETHOD (GetRootCategory)(foundation::ILogCategory **ppResult);
    STDMETHOD (GetCategory)(UINT32 size,UINT32 *pCategories,foundation::ILogCategory **ppResult);
    STDMETHOD (GetCategoryByName)(HSTRING categoryPath,foundation::ILogCategory **ppResult);
    STDMETHOD (Log)(foundation::IInspectable *pContext, foundation::ILogRecord *pLogRecord);
    STDMETHOD (GetLogCreatedEventSource)(_Outptr_ foundation::ILogCreatedEventSource** ppEventSource) override;

#if defined(TRACK_FOUNDATION_ENABLED) && defined(DEBUG)

    void DumpObject() override
    {
        foundation_assert(false);
    }

#endif

private:
    HRESULT GetOrCreateCategory(UINT32 size,UINT32 *pCategories,CLogCategory **ppLogFactory);

    // Override from IFireEventHandlerContainer
    STDMETHOD(OnFireEventError)(
        HRESULT hr,
        foundation::IUnknown *pEventHandler,
        foundation::IUnknown *pEventArgs) override;

    static HRESULT CreateLoggerInstance(CLogger **ppCoreLogger);
private:
    bool _is_fire_logging;
    foundation::ComPtr<foundation::ILogCategory> _rootCategory;
};

