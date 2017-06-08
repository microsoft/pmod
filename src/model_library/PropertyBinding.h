/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PropertyBinding.h
****/

#ifndef _PROPERTY_BINDING_DEFINED
#define _PROPERTY_BINDING_DEFINED


#include "BindingBase.h"
#include "FoundationBase.h"
#include "BaseModelSource.h"
#include <pmod/library/interfaces.h>

#include <memory>

typedef _FoundationBase
    <
        pmod::IPropertyBinding,
        pmod::library::IPropertyBindingClass,
        pmod::library::IPropertyBindingDelegate,
        pmod::library::PropertyBindingCreateParameters,
        pmod::IBindingValueChangedEventSource,
        pmod::IBindingValueChangedEventHandler,
        foundation::IUnknown,
        pmod::IBindingValueChangedEventArgs
    >
    _BindingBase;

typedef TSingleSourceModelBase
    <
        SingleSourceModelImpl
        <
            _SingleBaseModelSourceRefHolder
            <
                pmod::IObservableObject,
                pmod::IPropertyChangedEventSource,
                pmod::IPropertyChangedEventHandler,
                pmod::IPropertyChangedEventArgs
            >,
            pmod::IObservableObject
        >,                                  // TBaseModelSource
        _BindingBase,                       // TBase
        pmod::library::IPropertyBindingDelegate,           // Source Model Delegate
        pmod::IObservableObject,                     // the Source Model class
        pmod::IBindingValueChangedEventHandler    // the Model Event Handler Type
    >
    _BindingSourceModelBase;

typedef CBindingBase<_BindingSourceModelBase> _CPropertyBindingBase_Type;

class CPropertyBinding :
    public _CPropertyBindingBase_Type
{
 public:
     // Initialize this instance
     HRESULT _Initialize(const pmod::library::PropertyBindingCreateParameters *pInitializeParameters);

    // Invoked by the Delegate created from BaseSourceModel class
    HRESULT InvokeInternal(foundation::IUnknown *pSender,pmod::IPropertyChangedEventArgs *pArgs);

    static HRESULT CreateInstance(
        _In_ const pmod::library::PropertyBindingCreateParameters *pModelImpl,
        _In_ foundation::IInspectable *pOuter,
        _COM_Outptr_ foundation::IInspectable **ppNewInstance);

    static HRESULT ResolvePath(
        _In_            pmod::IObservableObject *pSource,
        _In_            foundation::IInspectable *path,
        _Outptr_        UINT32 *propertyId,
        _COM_Outptr_    pmod::IObservableObject **ppResolvedReference);

    static HRESULT GetValue(
        _In_            pmod::IObservableObject *pSource,
        _In_            foundation::IInspectable *path,
        _COM_Outptr_    foundation::IInspectable **ppValue);

protected:
    CPropertyBinding():
        m_bUseWeakReferenceSource(false),
        m_bUseWeakReferenceSteps(false)
    {
    }

    ~CPropertyBinding()
    {
    }

    #if defined(TRACK_FOUNDATION_ENABLED) && defined(DEBUG)

    void DumpObject() override
    {
        _traceMessage(U("PropertyBinding this:%04x"),this);
    }

    #endif
    

    // ComBase Override
    void OnFinalRelease() override;

     // IBindingBase Interface
    STDMETHOD( GetValue )(foundation::IInspectable **ppValue);

    // IPropertyBinding Interface
    STDMETHOD( GetPath )(foundation::IInspectable **ppValue);
    STDMETHOD( SetPath )(foundation::IInspectable *value);

    // IPropertyBindingClass Interface
    STDMETHOD(GetBindingsSteps)(
        _Out_ UINT32 *size_path_steps,
        pmod::IObservableObject ***ppPathSteps,
        _Out_ UINT32 *size_property_steps,
        UINT32 **ppPropertySteps) override;

    // Overrides from TSingleSourceModelBase
    pmod::library::IPropertyBindingDelegate *GetSourceDelegate() override { return m_pDelegate; }

     // Overrides from _BaseModelSource
    bool NeedAttachToEventSource() override;

    HRESULT AttachToSource() override;
    HRESULT DetachFromSource() override;

private:
    friend class CObservableObject;

    typedef foundation::ReferenceHolderBase<pmod::IObservableObject> _PropertyModelRefHolderT;
    typedef std::pair<_PropertyModelRefHolderT *,UINT32>  _BindingPathType;

    class _BindingStepsVector : public std::vector<_BindingPathType>
    {
    public:
        ~_BindingStepsVector()
        {
            clearAndDelete();
        }

        void clearAndDelete()
        {
            for(_BindingStepsVector::const_iterator iter = this->begin();
                iter != this->end();
                ++iter)
            {
                delete (*iter).first;
            }
            clear();
        }
    };

    // Private Methods
    HRESULT AddPathStep(foundation::string_t& pathSetp);
    HRESULT InternalFireBindingValueChanged();

    HRESULT ResolveBindings(_Outptr_ _BindingStepsVector& bindings);
    HRESULT UpdateBindings();
    HRESULT ConnectBindings();
    HRESULT DisconnectBindings();
    HRESULT CleanupBindings();

    HRESULT GetValueInternal(const _BindingStepsVector& bindings,foundation::IInspectable **ppValue);
    HRESULT GetBindingsStepsInternal(
        const _BindingStepsVector& bindings,
        _Out_ UINT32 *size_path_steps,
        pmod::IObservableObject ***ppPathSteps,
        _Out_ UINT32 *size_property_steps,
        UINT32 **ppPropertySteps);

    HRESULT ResolveSender(pmod::IObservableObject *pSender,size_t& nIndex);

    bool IsValueSet() override;
    bool IsPropertyPathValid() { return m_PropertyPath.size() > 0; }
    bool IsValueValid() override { return IsPropertyPathValid() && m_spReferenceHolder->IsReferenceAttached(); }
private:
    HRESULT GetPropertyValueStep(
        pmod::IObservableObject *pPropertyModelStep,
        UINT32 propertyId,
        foundation::IInspectable **ppValue);

    static HRESULT ResolveBindingReference(
        _In_ const _BindingPathType &bindingPath,
        _In_ bool ignoreNullReference,
        _Outptr_ pmod::IObservableObject **ppWeakReferenceModel);

    static HRESULT ResolveProperty(
        _In_ pmod::IObservableObject *pObservableObject,
        _In_ HSTRING propertyName,
        _Outptr_ UINT32 *propertyId);

    static HRESULT CreateBinding(
        _In_            pmod::IObservableObject *pSource,
        _In_            foundation::IInspectable *path,
        CPropertyBinding **ppPropertyBinding);

private:
    _Inspectable_ptr_vector_type               m_PropertyPath;
    _BindingStepsVector                 m_ConnectedBindings;
    bool                                m_bUseWeakReferenceSource:1;
    bool                                m_bUseWeakReferenceSteps:1;

};

#endif
