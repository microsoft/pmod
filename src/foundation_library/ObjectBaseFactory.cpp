/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObjectBaseFactory.cpp
****/
#include "pch.h"


#include "ObjectBaseFactory.h"
#include "CreateFoundationHelper.h"
#include "../foundation/TypeInfo.h"

#include <foundation/library/com_ptr_singleton.h>
#include "pal_str.h"

using namespace foundation;

// {6C4FC98B-EF25-4BCA-BA21-A47A9AFD03A7}
const GUID library::IID_IObjectBaseFactory =
{ 0x6c4fc98b, 0xef25, 0x4bca,{ 0xba, 0x21, 0xa4, 0x7a, 0x9a, 0xfd, 0x3, 0xa7 } };

class CLogCategory;
class CObjectBaseLogFactory : public _ObjectBaseLogFactory<>
{
public:
    static const CLogCategory*GetLogCategory();
};

typedef _ObjectBase
<
    IObject,
    library::IObjectClass,
    library::IObjectBaseDelegate,
    library::object_base_create_parameters,
    IObjectBaseEventSource,
    IObjectBaseEventHandler,
    foundation::IUnknown,
    IObjectBaseEventArgs,
    IObjectTypeInfo,
    CObjectBaseLogFactory
>
_ObjectBase_BaseType;

const CLogCategory*CObjectBaseLogFactory::GetLogCategory()
{
    static library::_ComPtrSingleton<const CLogCategory, foundation::_NoRefCountPtrImpl> _objectBaseLogCategoryInstance;

    return _objectBaseLogCategoryInstance._GetInstance([]() {
        return CLogger::GetCategory(
            Foundation_CategoryEnum::TypeId,
            Object_CategoryEnum::TypeId,
            -1);
    });
}

typedef CTypeInfo<foundation::IObjectTypeInfo, foundation::TypeInfo> CObjectBaseTypeInfo;

static HRESULT CreateObjectTypeInfo(
    _In_ foundation::TypeInfo *p_typeInfo,
    foundation::IObjectTypeInfo **ppObjectTypeInfo)
{
    CObjectBaseTypeInfo *pT;
    foundation::ctl::ComInspectableObject<CObjectBaseTypeInfo>::CreateInstance(&pT);
    pT->_InitializeTypeInfo(p_typeInfo);
    pT->SetDeleteTypeInfo(true);
    *ppObjectTypeInfo = pT;
    return S_OK;
}

class CObjectBase :
    public _ObjectBase_BaseType
{
public:
    static HRESULT  CreateInstance(
        _In_ const library::object_base_create_parameters *parameters,
        _In_ foundation::IInspectable *pOuter,
        _COM_Outptr_ foundation::IInspectable **ppNewInstance)
    {
        return CreateFoundationInstance<CObjectBase>(parameters, pOuter, ppNewInstance);
    }

    HRESULT _Initialize(const foundation::library::object_base_create_parameters *pParameters)
    {
        foundation::TypeInfo *pTypeInfo = new
            foundation::TypeInfo(
            0,
            foundation::TypeInfo_None,
            _DuplicateIIDType(pParameters->m_iid_type),
            pParameters->_name ? _pal_strdup(pParameters->_name):nullptr,
            nullptr);
        IFR_ASSERT(CreateObjectTypeInfo(pTypeInfo, &this->m_pTypeInfo));
        IFR_ASSERT(_ObjectBase_BaseType::_Initialize(pParameters));
        return S_OK;
    }

protected:
    CObjectBase()
    {
    }

    HRESULT SetTypeInfo(_In_ const IID& iidType) override
    {
        return S_OK;
    }

#if defined(TRACK_FOUNDATION_ENABLED) && defined(DEBUG)

    void DumpObject() override
    {
        DumpObjectBase(U("ObjectBase"));
    }
#endif
};

foundation::library::IObjectBaseFactory *CObjectBaseFactory::GetInstance()
{
    return library::_GetFactoryInstance<CObjectBaseFactory, foundation::library::IObjectBaseFactory>();
}

STDMETHODIMP CObjectBaseFactory::CreateObject(
    _In_ const foundation::library::object_base_create_parameters *parameters,
    _In_ foundation::IInspectable *pOuter,
    _Outptr_ foundation::IInspectable **ppNewInstance)
{
    return CObjectBase::CreateInstance(parameters, pOuter, ppNewInstance);
}

static foundation::library::_RegisterActivationFactory _serviceRegister(
    U("foundation.Object"),
    reinterpret_cast<foundation::library::GetActivationFactoryInstanceFunc>(&CObjectBaseFactory::GetInstance)
    );
