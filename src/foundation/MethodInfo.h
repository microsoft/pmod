/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MethodInfo.h
****/

#pragma once

#include "PropertyTypeInfo.h"

#include <foundation/hstring_wrapper.h>
#include <foundation/library/object_dispatch_type_info.h>
#include <foundation/library/interfaces/method_info_class.h>

#include <vector>

//------------------------------------------------------------------------------
// Class:   CParameterInfoBase
//
// Purpose: Define a Base class to implement a IParameterInfo interface
//
//------------------------------------------------------------------------------
typedef _TPropertyTypeInfo
<
    foundation::IParameterInfo, 
    foundation::MethodParamInfo
> CParameterInfoBase_BaseType;

class CParameterInfoBase :
    public CParameterInfoBase_BaseType
{
public:

    // IParameterInfo Interface
    STDMETHOD(GetParameterName)(_Outptr_ HSTRING* parameterName)
    {
        return CreateStringHelper(m_pPropertyTypeInfo->m_parameterName,parameterName);
    }
protected:
    // Override ComInspectableBase
    void FinalRelease() override
    {
        if(m_bDeletePropertyTypeInfo && m_pPropertyTypeInfo)
        {
            _pal_MemFree((LPVOID)m_pPropertyTypeInfo->m_parameterName);
        }
        CParameterInfoBase_BaseType::FinalRelease();
    }

    // friendly accessor functions.
    foundation::PropertyType GetPropertyType() const   
    {
        return m_pPropertyTypeInfo->m_property_type;
    }

    LPCSTR_t GetNameInternal() const   
    {
        return m_pPropertyTypeInfo->m_parameterName;
    }
};

//------------------------------------------------------------------------------
// Class:   CParameterInfo
//
// Purpose: A ParameterInfo implementation based on const struct MethodParamInfo
//
//------------------------------------------------------------------------------
class CParameterInfo :
    public CParameterInfoBase
{
 public:
     static HRESULT CreateInstance(
         _In_ const foundation::MethodParamInfo *pMethodParamInfo,
         _In_ const GUID *pModelType,
         _In_ bool bDeleteMethodParamInfo,
         _Outptr_ foundation::IParameterInfo **ppParameterInfo);
protected:
    CParameterInfo()
    {
    }

private:
    HRESULT Initialize(
        const foundation::MethodParamInfo* pMethodParamInfo,
        const GUID *pModelType,
        bool bDeleteMethodParamInfo);
};

//------------------------------------------------------------------------------
// Class:   CMethodInfo
//
// Purpose: A IMethodInfo implementation based on const struct MethodInfo
//
//------------------------------------------------------------------------------
class CMethodInfo :
    public foundation::library::_DefaultAdapterHost<foundation::IMethodInfo, foundation::ctl::ComInspectableBase>,
    public foundation::IMethodInfo,
    public foundation::library::IMethodInfoClass
{
public:
    static HRESULT CreateInstance(
    _In_ const foundation::MethodInfo *pMethodInfo,
    _In_ const GUID             *pResultTypeType,
    _Outptr_ foundation::IMethodInfo **ppMethodInfo);
protected:
    // Override ComInspectableBase
    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
    {
        if (iid == foundation::IMethodInfo::GetIID())
        {
            *ppInterface = static_cast<foundation::IMethodInfo *>(this);
        }
        else if (iid == foundation::library::IMethodInfoClass::GetIID())
        {
            *ppInterface = static_cast<foundation::library::IMethodInfoClass *>(this);
        }
        else
        {
            return foundation::library::_DefaultAdapterHost<foundation::IMethodInfo, foundation::ctl::ComInspectableBase>::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }

    STDMETHOD (GetId)(UINT32 *methodId) override
    {
        IFCPTR_ASSERT(methodId);
        *methodId = _methodId;
        return S_OK;
    }

    STDMETHOD (GetMethodName)(HSTRING* methodName) override
    {
        IFCPTR_ASSERT(methodName);
        return _pal_DuplicateString(_methodName,methodName);
    }

    STDMETHOD (GetParameters)(UINT32* size, foundation::IParameterInfo ***ppParameters) override
    {
        IFCPTR_ASSERT(size);
        IFCPTR_ASSERT(ppParameters);

        ContainerRefCountToArray(_parameters,size,ppParameters);
        return S_OK;
    }
    STDMETHOD (GetResultTypeInfo)(foundation::IPropertyTypeInfo **ppVarTypeInfo) override
    {
        IFCPTR_ASSERT(ppVarTypeInfo);
        return m_spResultTypeInfo.CopyTo(ppVarTypeInfo);
    }
    STDMETHOD (GetIsAsync)(_Out_ bool *value)
    {
        IFCPTR_ASSERT(value);
        *value = m_isAsync;
        return S_OK;
    }
    // Interface IMethodInfoClass
   STDMETHOD(AddParameterInfo)(
        _In_ HSTRING parameterName,
        _In_ foundation::PropertyType parameterType,
        _In_ GUID *pParameterModelType) override;

private:
    friend class CPropertyModelInfoFactory;
    void AddParameterInfoInternal(foundation::IParameterInfo *pParameterInfo);
    HRESULT AddParameterInfoArray(
        _In_ const foundation::MethodParamInfo *pParameterInfo);
    HRESULT AddParameterInfo(
        _In_ const foundation::MethodParamInfo *pParameterInfo,
        _In_ const GUID *pModelType,
         _In_ bool bDeleteMethodParamInfo,
        _Out_opt_ CParameterInfo **ppPropertyInfo);
    HRESULT Initialize(
        _In_ const foundation::MethodInfo *pMethodInfo,
        _In_ const GUID             *pResultTypeType
    );

private:

    typedef std::vector<foundation::ComPtr<foundation::IParameterInfo> > ParameterInfoVector;

    UINT32              _methodId;
    ParameterInfoVector _parameters;
    foundation::HStringPtr  _methodName;
    foundation::ComPtr<foundation::IPropertyTypeInfo> m_spResultTypeInfo;
    bool                                               m_isAsync;
};

