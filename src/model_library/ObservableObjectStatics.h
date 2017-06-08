/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObservableObjectStatics.h
****/
#pragma once

#include <foundation/ctl/com_library.h>
#include <pmod/library/model_library.h>
#include <foundation/com_ptr.h>

class CPropertyInfoBase;
class CPropertyTypeInfo;
class CParameterInfoBase;


class CObservableObjectStatics :
    public foundation::ctl::ComInspectableBase,
    public pmod::library::IObservableObjectStatics
{
 public:
    static pmod::library::IObservableObjectStatics *GetInstance();
    static HRESULT GetOrCreateInstance(pmod::library::IObservableObjectStatics **ppPropertyModelStatics);
    static HRESULT GetSourceInternal(foundation::IUnknown *pUnk, foundation::IObject **ppSource);
    static HRESULT GetSourceOfTypeInternal(foundation::IUnknown *pUnk, REFIID iidInstanceType, foundation::InstanceTypeOptions options, bool isModelClassType, foundation::IObject **ppSource);
    static HRESULT GetPropertyValueInternal(pmod::IObservableObject *pObservableObject,foundation::IInspectable *path,foundation::IInspectable **ppValue);
    static HRESULT SetPropertyValueInternal(pmod::IObservableObject *pObservableObject,foundation::IInspectable *path,foundation::IInspectable *pValue);
protected:
    ~CObservableObjectStatics();

    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
    {
        if (iid == pmod::library::IObservableObjectStatics::GetIID())
        {
            *ppInterface = static_cast<pmod::library::IObservableObjectStatics *>(this);
        }
        else
        {
            return foundation::ctl::ComInspectableBase::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }

    // IObservableObjectStatics Interface

    // Attemt to get the 'Source' Object if the instance is a ISourceModel interface
    STDMETHOD(GetSource)(foundation::IUnknown *pUnk, foundation::IObject **ppSource);

    STDMETHOD(GetPropertyValue)( 
            pmod::IObservableObject *pObservableObject,
            foundation::IInspectable *path,
            foundation::IInspectable **ppValue);

    STDMETHOD(SetPropertyValue)( 
            pmod::IObservableObject *pObservableObject,
            foundation::IInspectable *path,
            foundation::IInspectable *pValue);
  
    STDMETHOD(GetSourceOfType)(
            foundation::IUnknown *pUnk,
            REFIID iidInstanceType,
            foundation::InstanceTypeOptions options,
            bool isModelClassType,
            foundation::IObject **ppSource);

private:
    static foundation::ComPtr<pmod::library::IObservableObjectStatics> s_spPropertyModelStatics; // singleton instance
};


