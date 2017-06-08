/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PInvokeLogger.cpp
****/
#include "../pinvoke_api.h"
#include "../PInvokeUtils.h"
#include <foundation/library/logger_util.h>

using namespace foundation;

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE Logger_IsAvailable(bool *isAvailable)
{
    *isAvailable = logger_util::IsAvailable();
    return S_OK;
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE Logger_GetFoundationInstance(
	ILogger **pLogger)
{
	*pLogger =  logger_util::GetFoundationLogger();
	(*pLogger)->AddRef();
	return S_OK;
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE Logger_GetCategoryLevel(
	ILogger *pLogger,
	LPCSTR_t pCategoryPath,
	UINT32 *pLevel)
{
	LoggingLevel level;
	_IFR_(logger_util::GetCategoryLevel(pLogger,level, pCategoryPath));
	*pLevel = (UINT32)level;
	return S_OK;
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE Logger_SetCategoryLevel(
	ILogger *pLogger,
	UINT32 level,
	LPCSTR_t pCategoryPath)
{	
	return logger_util::SetCategoryLevel(pLogger, (LoggingLevel)level, pCategoryPath);
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE Logger_LogMessage(
	UINT32 level,
	UINT32 messageId,
	LPCSTR_t text,
	foundation::IInspectable *pParameter)
{
	return logger_util::LogMessage((LoggingLevel)level, messageId, text, pParameter);
}
