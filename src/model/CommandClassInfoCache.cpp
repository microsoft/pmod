/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:CommandClassInfoCache.cpp
****/

#include "pch.h"

#include "CommandClassInfoCache.h"
#include "CommandInfo.h"

#include "PtrSingleton.h"

// from public
#include <pmod/library/type_adapter_base.h>

using namespace pmod;

const UINT32 _CORE_BASE_COMMAND_TYPE_ID = foundation::_ReservedTypeId_Base + 0x200;

//////////////////////////////// ObjectObjectCommand ////////////////////////////////////////
// {AFAD3B17-330C-4DF2-A194-321DDA117988}
// Note: this is a copy of IID_ICommandModel
const GUID pmod::ObjectObjectCommandTypeInfo::IIDType =
{ 0xafad3b17, 0x330c, 0x4df2, { 0xa1, 0x94, 0x32, 0x1d, 0xda, 0x11, 0x79, 0x88 } };


const CommandTypeInfo ObjectObjectCommandTypeInfo::_CommandTypeInfo(
    _CORE_BASE_COMMAND_TYPE_ID + 0,
    ObjectObjectCommandTypeInfo::IIDType,
    U("CommandModel"),
    &foundation::_InspectableTypeInfo,
    &foundation::_InspectableTypeInfo,
    false);

//////////////////////////////// AsyncCommandTypeInfo ////////////////////////////////////////
// {81378F87-8A76-492D-A6D1-905DCB237B26}
// Note: this is a copy of IID_IAsyncCommand
const GUID pmod::AsyncCommandTypeInfo::IIDType =
{ 0x81378f87, 0x8a76, 0x492d, { 0xa6, 0xd1, 0x90, 0x5d, 0xcb, 0x23, 0x7b, 0x26 } };

const CommandTypeInfo AsyncCommandTypeInfo::_CommandTypeInfo(
    _CORE_BASE_COMMAND_TYPE_ID + 1,
    AsyncCommandTypeInfo::IIDType,
    U("ObjectAsyncCommand"),
    &foundation::_InspectableTypeInfo,
    &foundation::_InspectableTypeInfo,
    true);

//////////////////////////////// SimpleCommandTypeInfo ////////////////////////////////////////
// {81C05E25-DB59-4A54-A7DD-405E9AC97C7B}
// Note: this is a copy of IID_ISimpleCommand
const GUID pmod::SimpleCommandTypeInfo::IIDType =
{ 0x81c05e25, 0xdb59, 0x4a54, { 0xa7, 0xdd, 0x40, 0x5e, 0x9a, 0xc9, 0x7c, 0x7b } };

const CommandTypeInfo       SimpleCommandTypeInfo::_CommandTypeInfo(
    _CORE_BASE_COMMAND_TYPE_ID + 2,
    SimpleCommandTypeInfo::IIDType,
    U("SimpleCommand"),
    &foundation::_VoidTypeInfo,
    &foundation::_VoidTypeInfo,
    false);

//////////////////////////////// SimpleAsyncCommandTypeInfo ////////////////////////////////////////
// {11B0E777-37AA-4DCA-B0D5-68C9A86E9839}
// Note: this is a copy of IID_ISimpleAsyncCommand
const GUID pmod::SimpleAsyncCommandTypeInfo::IIDType =
{ 0x11b0e777, 0x37aa, 0x4dca, { 0xb0, 0xd5, 0x68, 0xc9, 0xa8, 0x6e, 0x98, 0x39 } };

const CommandTypeInfo       SimpleAsyncCommandTypeInfo::_CommandTypeInfo(
    _CORE_BASE_COMMAND_TYPE_ID + 3,
    SimpleAsyncCommandTypeInfo::IIDType,
    U("SimpleAsyncCommandModel"),
    &foundation::_VoidTypeInfo,
    &foundation::_VoidTypeInfo,
    true);


template<
class TInterface,
const IID* piid
>
class CCommandModelAdapterBase:
    public library::_AdapterBase<ICommand, TInterface,piid>
{
protected:
    // Properties
    STDMETHOD(GetState)(UINT32* pVal)
    {
        return this->m_pInner->GetState(pVal);
    }
    // Event methods.
    STDMETHOD(GetCanExecuteChangedEventSource)(ICanExecuteChangedEventSource** ppEventSource)
    {
        return this->m_pInner->GetCanExecuteChangedEventSource(ppEventSource);
    }

};


class CAsyncCommandModelAdapter :
    public CCommandModelAdapterBase<IAsyncCommand, &IID_IAsyncCommand>
{
protected:

    // Methods.
    STDMETHOD(CanExecute)(foundation::IInspectable *pParameter,bool *pCanExecute)
    {
        return this->m_pInner->CanExecute(
            pParameter,
            pCanExecute);
    }

    STDMETHOD(Execute)(foundation::IInspectable *pParameter, foundation::IAsyncOperation **ppResult)
    {
        foundation::InspectablePtr spResult;
        _IFR_(this->m_pInner->Execute(
            pParameter,
            spResult.GetAddressOf()));
        _IFR_(foundation::QueryInterfaceIf(spResult, ppResult));

        return S_OK;
    }
};


class CSimpleCommandModelAdapter :
    public CCommandModelAdapterBase<ISimpleCommand, &IID_ISimpleCommand>
{
protected:

    // Methods.
    STDMETHOD(CanExecute)(bool *pCanExecute)
    {
        foundation::InspectablePtr spEmptyParameter;
        return this->m_pInner->CanExecute(
            spEmptyParameter,
            pCanExecute);
    }

    STDMETHOD(Execute)()
    {
        foundation::InspectablePtr spEmptyParameter, spEmptyResult;

        return this->m_pInner->Execute(
            spEmptyParameter,
            spEmptyResult.GetAddressOf());
    }
};


class CSimpleAsyncCommandModelAdapter :
    public CCommandModelAdapterBase<ISimpleAsyncCommand, &IID_ISimpleAsyncCommand>
{
protected:

    // Methods.
    STDMETHOD(CanExecute)(bool *pCanExecute)
    {
        foundation::InspectablePtr spEmptyParameter;
        return this->m_pInner->CanExecute(
            spEmptyParameter,
            pCanExecute);
    }

    STDMETHOD(Execute)(_COM_Outptr_ foundation::IAsyncOperationBase **ppResult)
    {
        foundation::InspectablePtr spEmptyParameter, spResult;

        _IFR_(this->m_pInner->Execute(
            spEmptyParameter,
            spResult.GetAddressOf()));
        _IFR_(foundation::QueryInterfaceIf(spResult, ppResult));

        return S_OK;
    }
};


class CCommandModelClassInfoCache_Type : public CCommandModelClassInfoCache::_TypeModelInfoCacheType
{
public:
    CCommandModelClassInfoCache_Type()
    {
        const CommandTypeInfo *factoryModelInfos[] = {
            &ObjectObjectCommandTypeInfo::_CommandTypeInfo,
            &AsyncCommandTypeInfo::_CommandTypeInfo,
            &SimpleCommandTypeInfo::_CommandTypeInfo,
            &SimpleAsyncCommandTypeInfo::_CommandTypeInfo,
        };
        for (int index = 0;
            index < sizeof(factoryModelInfos) / sizeof(const CommandTypeInfo *);
            index++)
        {
            IGNOREHR(RegisterCommandModelTypeInfoInternal(factoryModelInfos[index]));
        }

        const foundation::library::_TypeAdapterEntry _modelFactoryEntries[] = {
            { SimpleCommandTypeInfo::IIDType, SimpleCommandTypeInfo::IIDType, foundation::CreateAdapter<CSimpleCommandModelAdapter> },
            { SimpleAsyncCommandTypeInfo::IIDType, SimpleAsyncCommandTypeInfo::IIDType, foundation::CreateAdapter<CSimpleAsyncCommandModelAdapter> },
            { AsyncCommandTypeInfo::IIDType, AsyncCommandTypeInfo::IIDType, foundation::CreateAdapter<CAsyncCommandModelAdapter> },
            { foundation_GUID_NULL, foundation_GUID_NULL, 0L }
        };
        foundation::ComPtr<foundation::library::IInterfaceAdapterFactory> spModelAdapterFactory;
        foundation::library::CreateTypeAdapterFactory(_modelFactoryEntries, spModelAdapterFactory.GetAddressOf());
        foundation::library::RegisterInterfaceAdapterFactory(spModelAdapterFactory);
    }

private:
    HRESULT RegisterCommandModelTypeInfoInternal(_In_ const CommandTypeInfo *pCommandModelTypeInfo)
    {
        CCommandModelTypeInfo *pCommandModelTypeInfoClass = nullptr;
        IFR_ASSERT(foundation::ctl::ComInspectableObject<CCommandModelTypeInfo>::CreateInstance(&pCommandModelTypeInfoClass));
        IFR_ASSERT(pCommandModelTypeInfoClass->_Initialize(pCommandModelTypeInfo));

        this->AddToCache(pCommandModelTypeInfo->get_iid_type(), pCommandModelTypeInfoClass);
        return S_OK;
    }
};

CCommandModelClassInfoCache::_TypeModelInfoCacheType& CCommandModelClassInfoCache::GetInstance()
{
	return *_GetPtrInstance<CCommandModelClassInfoCache_Type>();
}


