/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:com_interfaces.h
****/
#pragma once

#include "assert.h"
#include "foundation_api.h"
#include "pal.h"

#if defined(_WINDOWS)
// For Windows we just need to include inspectable interface
#include <inspectable.h>
#include <WeakReference.h>

namespace foundation {
    typedef IUnknown        IUnknown;
    typedef IInspectable    IInspectable;
    typedef IWeakReference  IWeakReference;
    typedef IWeakReferenceSource IWeakReferenceSource;
    typedef TrustLevel      TrustLevel;
}
#define foundation_GUID_NULL GUID_NULL

#define foundation_IID_IUnknown IID_IUnknown
#define foundation_IID_IInspectable IID_IInspectable
#define foundation_IID_IWeakReference IID_IWeakReference
#define foundation_IID_IWeakReferenceSource IID_IWeakReferenceSource

#else

namespace foundation
{

    extern FOUNDATION_API const IID GUID_NULL;
#define foundation_GUID_NULL foundation::GUID_NULL

    extern FOUNDATION_API const IID IID_IUnknown;

#define foundation_IID_IUnknown foundation::IID_IUnknown

    struct IUnknown
    {
        virtual HRESULT STDMETHODCALLTYPE QueryInterface(
        REFIID riid,
        void **ppvObject) = 0;
        virtual _pal_AtomicIntegerType STDMETHODCALLTYPE AddRef(void) = 0;
        virtual _pal_AtomicIntegerType STDMETHODCALLTYPE Release(void) = 0;
    protected:
        ~IUnknown(){}
    };


    extern FOUNDATION_API const IID IID_IInspectable;
#define foundation_IID_IInspectable foundation::IID_IInspectable

    typedef
    enum TrustLevel
    {
        BaseTrust = 0,
        PartialTrust = (BaseTrust + 1),
        FullTrust = (PartialTrust + 1)
    }TrustLevel;

    struct IInspectable : public IUnknown
    {
        virtual HRESULT STDMETHODCALLTYPE GetIids(
        /* [out] */ __RPC__out ULONG *iidCount,
        /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids) = 0;

        virtual HRESULT STDMETHODCALLTYPE GetRuntimeClassName(
            /* [out] */ __RPC__deref_out_opt HSTRING *className) = 0;

        virtual HRESULT STDMETHODCALLTYPE GetTrustLevel(
            /* [out] */ __RPC__out TrustLevel *trustLevel) = 0;
    protected:
        ~IInspectable(){}
    };

    extern FOUNDATION_API const IID IID_IWeakReference;
#define foundation_IID_IWeakReference foundation::IID_IWeakReference

    struct IWeakReference : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Resolve(
            /* [in] */ __RPC__in REFIID riid,
            /* [iid_is][out] */ __RPC__deref_out IInspectable **objectReference) = 0;
    };

    extern FOUNDATION_API const IID IID_IWeakReferenceSource;
#define foundation_IID_IWeakReferenceSource foundation::IID_IWeakReferenceSource

    struct IWeakReferenceSource : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetWeakReference(
            /* [retval][out] */ __RPC__deref_out_opt IWeakReference **weakReference) = 0;

    };

}

#define _HRESULT_TYPEDEF_(_sc) ((HRESULT)_sc)

#define NOERROR             0


#define E_UNEXPECTED                     _HRESULT_TYPEDEF_(0x8000FFFFL)
#define E_NOTIMPL                        _HRESULT_TYPEDEF_(0x80004001L)
#define E_OUTOFMEMORY                    _HRESULT_TYPEDEF_(0x8007000EL)
#define E_INVALIDARG                     _HRESULT_TYPEDEF_(0x80070057L)
#define E_NOINTERFACE                    _HRESULT_TYPEDEF_(0x80004002L)
#define E_POINTER                        _HRESULT_TYPEDEF_(0x80004003L)
#define E_HANDLE                         _HRESULT_TYPEDEF_(0x80070006L)
#define E_ABORT                          _HRESULT_TYPEDEF_(0x80004004L)
#define E_FAIL                           _HRESULT_TYPEDEF_(0x80004005L)
#define E_ACCESSDENIED                   _HRESULT_TYPEDEF_(0x80070005L)
#define E_BOUNDS                         _HRESULT_TYPEDEF_(0x8000000BL)

#define SUCCEEDED(hr)   (((HRESULT)(hr)) >= 0)
#define FAILED(hr)      (((HRESULT)(hr)) < 0)

//
// Severity values
//

#define SEVERITY_SUCCESS    0
#define SEVERITY_ERROR      1

//
// Generic test for error on any status value.
//

#define IS_ERROR(Status) (((unsigned long)(Status)) >> 31 == SEVERITY_ERROR)

//
// Return the code
//

#define HRESULT_CODE(hr)    ((hr) & 0xFFFF)
#define SCODE_CODE(sc)      ((sc) & 0xFFFF)

//
//  Return the facility
//

#define HRESULT_FACILITY(hr)  (((hr) >> 16) & 0x1fff)
#define SCODE_FACILITY(sc)    (((sc) >> 16) & 0x1fff)

//
//  Return the severity
//

#define HRESULT_SEVERITY(hr)  (((hr) >> 31) & 0x1)
#define SCODE_SEVERITY(sc)    (((sc) >> 31) & 0x1)

//
// Create an HRESULT value from component pieces
//

#define MAKE_HRESULT(sev,fac,code) \
    ((HRESULT) (((unsigned long)(sev)<<31) | ((unsigned long)(fac)<<16) | ((unsigned long)(code))) )
#define MAKE_SCODE(sev,fac,code) \
    ((SCODE) (((unsigned long)(sev)<<31) | ((unsigned long)(fac)<<16) | ((unsigned long)(code))) )


#define S_OK        ((HRESULT)0L)
#define S_FALSE     ((HRESULT)1L)


#endif

#define BEGIN_DECLARE_IID(iface,decl_api)    \
EXTERN_C decl_api const IID IID_##iface;                        \

#define BEGIN_DECLARE_CLASS(class_decl,iface,ibase,decl_api)    \
BEGIN_DECLARE_IID(iface,decl_api)                               \
class_decl iface : public ibase                                 \
{                                                               \
public:                                                         \
    static const IID& GetIID() { return IID_##iface; }          \

#define _VOID_MACRO

#define BEGIN_DECLARE_INTERFACE(iface,ibase,decl_api)           \
    BEGIN_DECLARE_CLASS(struct,iface,ibase,decl_api)         \

#define BEGIN_DECLARE_INTERNAL_INTERFACE(iface,ibase) \
    BEGIN_DECLARE_INTERFACE(iface, ibase, _VOID_MACRO) \

#define BEGIN_DECLARE_CLASS_IID(iface,ibase,iidType)    \
struct iface : public ibase                                 \
{                                                               \
public:                                                         \
    static const IID& GetIID() { return iidType; }              \

#define END_DECLARE_INTERFACE()        \
};                                     \

#define END_DECLARE_INTERNAL_INTERFACE() \
    END_DECLARE_INTERFACE() \

namespace foundation
{
    template <class Q>
    static HRESULT QueryWeakReference(_In_ foundation::IUnknown *pInnerUnk, REFIID riid, _COM_Outptr_ Q** ppInterface)
    {
        foundation_assert(pInnerUnk);
        HRESULT hr = pInnerUnk->QueryInterface(riid, (void **)ppInterface);
        if(SUCCEEDED(hr))
        {
            (*ppInterface)->Release();
        }

        return hr;
    }

    template <class Q>
    static inline const IID& GetIIDOf()
    {
#if defined(_WINDOWS)
        __if_exists (Q::GetIID)
        {
            return Q::GetIID();
        }
        __if_not_exists (Q::GetIID)
        {
            return __uuidof(Q);
        }
#else
        return Q::GetIID();
#endif
    }

    template <class Q>
    static HRESULT QueryWeakReference(_In_ foundation::IUnknown *pInnerUnk, _COM_Outptr_ Q** pp)
    {
        return QueryWeakReference(pInnerUnk, GetIIDOf<Q>(), pp);
    }

    template <class T>
    HRESULT QueryInterface(_In_ foundation::IUnknown *pIn, _COM_Outptr_ T** pp)
    {
        foundation_assert(pIn);
        return pIn->QueryInterface(GetIIDOf<T>(), (void **)pp);
    }

    template <>
    inline HRESULT QueryWeakReference(_In_ foundation::IUnknown *pInnerUnk, _COM_Outptr_ IInspectable** pp)
    {
        return QueryWeakReference(pInnerUnk,IID_IInspectable,pp);
    }
    
    template <>
    inline HRESULT QueryWeakReference(_In_ foundation::IUnknown *pInnerUnk, _COM_Outptr_ IUnknown** pp)
    {
        return QueryWeakReference(pInnerUnk,IID_IUnknown,pp);
    }
    
    template <>
    inline HRESULT QueryInterface(_In_ foundation::IUnknown *pIn, _COM_Outptr_ IInspectable** pp)
    {
        return pIn->QueryInterface(IID_IInspectable, (void **)pp);
    }
    
    template <>
    inline HRESULT QueryInterface(_In_ foundation::IUnknown *pIn, _COM_Outptr_ foundation::IUnknown** pp)
    {
        return pIn->QueryInterface(IID_IUnknown, (void **)pp);
    }

    template <class T>
    HRESULT QueryInterfaceIf(_In_opt_ foundation::IUnknown *pIn, _COM_Outptr_result_maybenull_ T** pp)
    {
        if (pIn == nullptr)
        {
            *pp = nullptr;
            return S_OK;
        }

        return QueryInterface(pIn, pp);
    }

    template <class T>
    HRESULT QueryInterface(_In_ foundation::IUnknown *pIn, REFIID riid, _COM_Outptr_ T** pp)
    {
        foundation_assert(pIn);

        HRESULT hr = S_OK;
        hr = pIn->QueryInterface(riid, (void**)pp);
        return hr;
    }

    template <class T>
    HRESULT QueryInterfaceIf(_In_opt_ foundation::IUnknown *pIn, REFIID riid, _COM_Outptr_result_maybenull_ T** pp)
    {
        if (pIn == nullptr)
        {
            *pp = nullptr;
            return S_OK;
        }

        return QueryInterface(pIn, riid, pp);
    }

    template <class T>
    T *CastTo(_In_ foundation::IUnknown *pIn, const IID& iidType)
    {
        foundation_assert(pIn);
        T *pT = nullptr;
        QueryWeakReference(pIn, iidType, &pT);
        return pT;
    }

    template <class T>
    T *CastTo(_In_ foundation::IUnknown *pIn)
    {
        return CastTo<T>(pIn, GetIIDOf<T>());
    }

    inline bool IsTypeOf(_In_ foundation::IUnknown *pIn, const IID& iidType)
    {
        return CastTo<foundation::IUnknown>(pIn, iidType) != nullptr;
    }

    template <class T>
    bool IsTypeOf(_In_ foundation::IUnknown *pIn)
    {
        return IsTypeOf(pIn, GetIIDOf<T>());
    }
}

