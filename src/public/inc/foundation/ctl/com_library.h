/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:com_library.h
****/
#pragma once

#include <foundation/assert.h>
#include <foundation/com_interfaces.h>
#include <foundation/com_ptr.h>
#include <foundation/macros.h>

#if defined(_VARIADIC_SUPPORT)

#define TYPE_NAME_CREATE_VAR_ARGUMENTS typename... Arguments
#define DECLARE_CREATE_VAR_ARGUMENTS const Arguments&... params
#define PASS_CREATE_VAR_ARGUMENTS params...
#else

#define TYPE_NAME_CREATE_VAR_ARGUMENTS typename Argument
#define DECLARE_CREATE_VAR_ARGUMENTS const Argument& params
#define PASS_CREATE_VAR_ARGUMENTS params
#endif

// Note: uncomment this line if you want debugging the final release
// preventing any recursive call to AddRef/Release
//#define _DEBUG_FINAL_RELEASE

namespace foundation {
namespace ctl
{
#if defined(_TRACK_MEMORY_COM_TEMPLATE_ALLOC) && defined(DEBUG)
    class Allocator
    {
    public:
        static void alloc(size_t size);
        static void release(size_t size);
    };
#endif

    struct INonDelegatingUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE NonDelegatingQueryInterface(
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject) = 0;
        virtual _pal_AtomicIntegerType STDMETHODCALLTYPE NonDelegatingAddRef(void) = 0;
        virtual _pal_AtomicIntegerType STDMETHODCALLTYPE NonDelegatingRelease(void) = 0;
    };

    struct INonDelegatingInspectable : public INonDelegatingUnknown
    {
        virtual HRESULT STDMETHODCALLTYPE NonDelegatingGetIids(
            /* [out] */ __RPC__out ULONG *iidCount,
            /* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids) = 0;

        virtual HRESULT STDMETHODCALLTYPE NonDelegatingGetRuntimeClassName(
            /* [out] */ __RPC__deref_out_opt HSTRING *className) = 0;

        virtual HRESULT STDMETHODCALLTYPE NonDelegatingGetTrustLevel(
            /* [out] */ __RPC__out foundation::TrustLevel *trustLevel) = 0;
    };

    ///
    /// @brief implement the 'TRefCountImpl' pattern using a local ref counted atomic
    ///
    class _RefCountImpl
    {
    protected:
        _RefCountImpl() :
            m_cRef(1)
        {
        }

        // allow derived classes to addRef/release without the destroying logic
        // this is useful in scenarios where the ref counting is controlled 
        // by another class in the hierarchy
        inline _pal_AtomicIntegerType AddRefImplInternal()
        {
            return _pal_InterlockedIncrement(&m_cRef);
        }

        inline _pal_AtomicIntegerType ReleaseImplInternal()
        {
            return _pal_InterlockedDecrement(&m_cRef);
        }

        inline _pal_AtomicIntegerType GetRefCountInternal()
        {
            return m_cRef;
        }
    private:
        volatile _pal_AtomicIntegerType m_cRef;
    };

    ///
    /// @brief implement the 'TRefCountImpl' pattern with no ref count
    ///
    class _NoRefCountImpl
    {
    protected:
        inline _pal_AtomicIntegerType AddRefImplInternal()
        {
            return 1;
        }

        inline _pal_AtomicIntegerType ReleaseImplInternal()
        {
            return 1;
        }

        inline _pal_AtomicIntegerType GetRefCountInternal()
        {
            return 1;
        }
    };

    ///
    /// @brief _IRefCountImpl interface to abstract the 'TRefCountImpl' pattern
    ///
    struct _IRefCountImpl
    {
        virtual ~_IRefCountImpl() {}
        virtual _pal_AtomicIntegerType _AddRefImpl() = 0;
        virtual _pal_AtomicIntegerType _ReleaseImpl() = 0;
        virtual _pal_AtomicIntegerType _GetRefCountImpl() = 0;
    };

    ///
    /// @brief Implements the _IRefCountImpl from a base class
    ///
    template <class TBASE, class TRefCountImpl>
    class _RefCountStubImpl :
        public TBASE,
        public TRefCountImpl
    {
    public:
        _pal_AtomicIntegerType _AddRefImpl() override
        {
            return TBASE::AddRefImplInternal();
        }
        _pal_AtomicIntegerType _ReleaseImpl()  override
        {
            return TBASE::ReleaseImplInternal();
        }
        _pal_AtomicIntegerType _GetRefCountImpl() override
        {
            return TBASE::GetRefCountInternal();
        }
    };

    ///
    /// @brief 'TRefCountImpl' pattern that use an underlying _IRefCountImpl interface
    ///
    template <class TRefCountImpl>
    class _RefCountDelegeteImpl
    {
    protected:
        _RefCountDelegeteImpl() :
            m_cRefCountImpl(nullptr)
        {}

        ~_RefCountDelegeteImpl()
        {
            delete m_cRefCountImpl;
        }

        inline _pal_AtomicIntegerType AddRefImplInternal()
        {
            return m_cRefCountImpl->_AddRefImpl();
        }

        inline _pal_AtomicIntegerType ReleaseImplInternal()
        {
            return m_cRefCountImpl->_ReleaseImpl();
        }

        inline _pal_AtomicIntegerType GetRefCountInternal()
        {
            return m_cRefCountImpl->_GetRefCountImpl();
        }
        inline TRefCountImpl *_Get_ref_count_impl()
        {
            return m_cRefCountImpl;
        }
        inline void _SetRefCountImpl(TRefCountImpl *pRefCountImpl)
        {
            m_cRefCountImpl = pRefCountImpl;
        }
    private:
        TRefCountImpl *m_cRefCountImpl;
    };

    ///
    /// @brief com based template class that Implements IUnknown interface
    ///
    template <class TBaseInterface, class TRefCountImpl = _RefCountImpl>
    class _ComBase : 
        public TBaseInterface, 
        public TRefCountImpl,
        public _ComPtrImplBase
    {
    public:
        typedef _ComBase<TBaseInterface, TRefCountImpl> _ComBaseType;

        template <class T>
        static inline _pal_AtomicIntegerType _AddRef(T *pT)
        {
            return CastToUnknown(pT)->AddRef();
        }

        template <class T>
        static inline _pal_AtomicIntegerType _Release(T *pT)
        {
            return CastToUnknown(pT)->Release();
        }

        template <class T>
        static inline HRESULT _QueryInterface(T *pT, REFIID riid, _COM_Outptr_ void **ppvObject)
        {
            return CastToUnknown(pT)->QueryInterface(riid, ppvObject);
        }

        inline foundation::IUnknown *CastToUnknown()
        {
            return static_cast<foundation::IUnknown *>(this);
        }
    protected:
        _ComBase()
        {
#ifdef _DEBUG_FINAL_RELEASE
            m_fFinalReleaseCalled = false;
#endif
        }

        virtual ~_ComBase()
        {
#ifdef _DEBUG_FINAL_RELEASE
            foundation_assert(m_fFinalReleaseCalled);
#endif
        }
    public:
        virtual bool IsComposed()
        {
            return false;
        }

        virtual TBaseInterface *GetInnerInterface()
        {
            return static_cast<TBaseInterface *>(this);
        }
    protected:

        // foundation::IUnknown implementation
        virtual HRESULT QueryInterfaceImpl(REFIID iid, _COM_Outptr_ foundation::IUnknown **ppInterface)
        {
            if (iid == foundation_IID_IUnknown)
            {
                *ppInterface = static_cast<foundation::IUnknown *>(this);
                return S_OK;
            }

            *ppInterface = nullptr;
            return E_NOINTERFACE;
        }

        virtual _pal_AtomicIntegerType AddRefImpl()
        {
#ifdef _DEBUG_FINAL_RELEASE
            foundation_assert(!m_fFinalReleaseCalled);
#endif
            return this->AddRefImplInternal();
        }

        virtual _pal_AtomicIntegerType ReleaseImpl()
        {
#ifdef _DEBUG_FINAL_RELEASE
            foundation_assert(!pThis->m_fFinalReleaseCalled);
#endif
            _pal_AtomicIntegerType l = this->ReleaseImplInternal();
            if (l == 0)
            {
                // Don't allow calls to AddRef/Release anymore in the future
#ifdef _DEBUG_FINAL_RELEASE
                this->m_fFinalReleaseCalled = true;
#endif
                (static_cast<_ComBaseType *>(this))->FinalRelease();
                foundation_assert(this->GetRefCountInternal() == 0);
                delete this;
                return 0;
            }
            return l;
        }

        virtual _pal_AtomicIntegerType GetRefCount()
        {
            return this->GetRefCountInternal();
        }

        // Called when the object refcount reaches zero.
        // Derived implementations ARE NOT allowed to increase the refcount.
        // This method is guaranteed to be called only once.
        virtual void FinalRelease()
        {
        }

    private:
        template<class T>
        static inline foundation::IUnknown * CastToUnknown(T * pT)
        {
            return static_cast<foundation::IUnknown *>(static_cast<_ComBaseType *>(pT));
        }
#ifdef _DEBUG_FINAL_RELEASE
        bool m_fFinalReleaseCalled : 1;
#endif
    };

    typedef _ComBase<foundation::IUnknown> ComBase;

    ///
    /// @brief explicit ComBase implementation of _ComPtrImpl
    ///
    struct _ComBasePtrImpl
    {
        template <class T>
        static inline _pal_AtomicIntegerType _AddRef(T *pT)
        {
            return CastToUnknown(pT)->AddRef();
        }

        template <class T>
        static inline _pal_AtomicIntegerType _Release(T *pT)
        {
            return CastToUnknown(pT)->Release();
        }

        template <class T>
        static inline HRESULT _QueryInterface(T *pT, REFIID riid, _COM_Outptr_ void **ppvObject)
        {
            return CastToUnknown(pT)->QueryInterface(riid, ppvObject);
        }
    };


    ///
    /// @brief com based template class that Implements IInspectable interface
    /// use _ComBase template as it's base class
    ///
    template <class TBase>
    class _ComInspectableBase : public TBase
    {
    public:
        typedef _ComInspectableBase<TBase> _ComInspectableBaseType;

        inline foundation::IInspectable *CastToInspectable()
        {
            return static_cast<foundation::IInspectable *>(this);
        }
    protected:

        // foundation::IUnknown implementation
        HRESULT QueryInterfaceImpl(REFIID iid, _COM_Outptr_ foundation::IUnknown **ppInterface) override
        {
            if (iid == foundation_IID_IInspectable)
            {
                *ppInterface = static_cast<foundation::IInspectable *>(this);
            }
            else
            {
                return TBase::QueryInterfaceImpl(iid, ppInterface);
            }
            return S_OK;
        }
    private:
        IID* GetLocalImplementedIIDs(_Out_ INT *count)
        {
            static IID localIIDs[] =
            {
                foundation_IID_IInspectable,
                foundation_IID_IUnknown
            };

            *count = sizeof(localIIDs) / sizeof(IID);

            return localIIDs;
        }

        INT GetLocalImplementedIIDsCount()
        {
            INT count = 0;

            GetLocalImplementedIIDs(&count);

            return count;
        }

    protected:

        // The Top of the chain of implemented IIDs
        virtual INT GetImplementedIIDsCount()
        {
            return GetLocalImplementedIIDsCount();
        }

        virtual void CopyIIDsToArray(INT first, IID *pResult)
        {
            INT count = 0;
            IID *pLocalIIDs = GetLocalImplementedIIDs(&count);

            for (INT current = 0; current < count; current++)
            {
                pResult[first + current] = pLocalIIDs[current];
            }
        }

    protected:

        // foundation::IInspectable implementation
        virtual HRESULT GetRuntimeClassNameImpl(_Out_ HSTRING *pClassName)
        {
            if (!pClassName)
            {
                return E_POINTER;
            }
            *pClassName = nullptr;
            return S_OK;
        }

        virtual HRESULT GetTrustLevelImpl(_Out_ foundation::TrustLevel *pTrustLevel)
        {
            if (!pTrustLevel)
            {
                return E_POINTER;
            }
            *pTrustLevel = foundation::TrustLevel::BaseTrust;
            return S_OK;
        }

        virtual HRESULT GetIIDsImpl(_Out_ ULONG *iidCount, _Outptr_ IID **iids)
        {
            IID *pResult = nullptr;
            ULONG count = 0;

            count = GetImplementedIIDsCount();

            pResult = reinterpret_cast<IID *>(_pal_MemAlloc(sizeof(IID) * count));
            if (pResult == nullptr)
            {
                return E_OUTOFMEMORY;
            }

            CopyIIDsToArray(0, pResult);
            *iids = pResult;
            *iidCount = count;
            return S_OK;
        }
    };

    typedef _ComInspectableBase< _ComBase<foundation::IInspectable> > ComInspectableBase;

    template <class TBASE>
    class ComObject : public TBASE
    {
    protected:
#if defined(_VARIADIC_SUPPORT)

        template <typename... Arguments>
        ComObject(_In_ Arguments&&... params) :
            TBASE(params...)
        {
        }
#else
        template <class TParameter>
        ComObject(const TParameter& parameter) :
            TBASE(parameter)
        {
        }

        template <class TParameter>
        ComObject(TParameter& parameter) :
            TBASE(parameter)
        {
        }

        ComObject()
        {
        }

#endif
        virtual ~ComObject()
        {
#if defined(_TRACK_MEMORY_COM_TEMPLATE_ALLOC) && defined(DEBUG)
            Allocator::release(sizeof(ComObject<TBASE>));
#endif
        }

    public:

        STDMETHODIMP QueryInterface(REFIID iid, _COM_Outptr_ void **ppValue)
        {
            HRESULT hr = S_OK;
            foundation::IUnknown *pInterface = nullptr;

            hr = TBASE::QueryInterfaceImpl(iid, &pInterface);
            if (FAILED(hr))
            {
                *ppValue = nullptr;
                return hr;
            }

            *ppValue = pInterface;
            pInterface->AddRef();
            return S_OK;
        }

        STDMETHODIMP_(_pal_AtomicIntegerType) AddRef()
        {
            return TBASE::AddRefImpl();
        }

        STDMETHODIMP_(_pal_AtomicIntegerType) Release()
        {
            return TBASE::ReleaseImpl();
        }

    public:
#if defined(_VARIADIC_SUPPORT)
        template <typename... Arguments>
        static HRESULT CreateInstance(_COM_Outptr_ TBASE **ppNewInstance, _In_ Arguments&&... params)
        {
            *ppNewInstance = new ComObject<TBASE>(params...);
#if defined(_TRACK_MEMORY_COM_TEMPLATE_ALLOC) && defined(DEBUG)
            Allocator::alloc(sizeof(ComObject<TBASE>));
#endif
            return *ppNewInstance != nullptr ? S_OK : E_OUTOFMEMORY;
        }
#else      
        template <class TParameter>
        static HRESULT CreateInstance(_COM_Outptr_ TBASE **ppNewInstance, const TParameter& parameter)
        {
            *ppNewInstance = new ComObject<TBASE>(parameter);
#if defined(_TRACK_MEMORY_COM_TEMPLATE_ALLOC) && defined(DEBUG)
            Allocator::alloc(sizeof(ComObject<TBASE>));
#endif
            return *ppNewInstance != nullptr ? S_OK : E_OUTOFMEMORY;
        }

        template <class TParameter>
        static HRESULT CreateInstance(_COM_Outptr_ TBASE **ppNewInstance, TParameter& parameter)
        {
            *ppNewInstance = new ComObject<TBASE>(parameter);
#if defined(_TRACK_MEMORY_COM_TEMPLATE_ALLOC) && defined(DEBUG)
            Allocator::alloc(sizeof(ComObject<TBASE>));
#endif

            return *ppNewInstance != nullptr ? S_OK : E_OUTOFMEMORY;
        }
#endif

        template <class T>
        static HRESULT CreateInstance(_COM_Outptr_ T **ppNewInstance)
        {
            TBASE *pNewInstance = new ComObject<TBASE>();
            *ppNewInstance = static_cast<T *>(pNewInstance);
            return pNewInstance != nullptr ? S_OK : E_OUTOFMEMORY;
        }
    };

    template <class TBASE>
    class ComInspectableObject : public ComObject<TBASE>
    {
    protected:
#if defined(_VARIADIC_SUPPORT)
        template <typename... Arguments>
        ComInspectableObject(_In_ Arguments&&... params) :
            ComObject<TBASE>(params...)
        {
        }
#else
        template <class TParameter>
        ComInspectableObject(const TParameter& parameter) :
            ComObject<TBASE>(parameter)
        {
        }

        template <class TParameter>
        ComInspectableObject(TParameter& parameter) :
            ComObject<TBASE>(parameter)
        {
        }

        ComInspectableObject()
        {
        }
#endif

        virtual ~ComInspectableObject()
        {
#if defined(_TRACK_MEMORY_COM_TEMPLATE_ALLOC) && defined(DEBUG)
            Allocator::release(sizeof(ComInspectableObject<TBASE>));
#endif
        }
    public:

        // foundation::IInspectable implementation
        STDMETHODIMP GetRuntimeClassName(_Out_ HSTRING *pClassName)
        {
            return TBASE::GetRuntimeClassNameImpl(pClassName);
        }

        STDMETHODIMP GetTrustLevel(_Out_ foundation::TrustLevel *trustLvl)
        {
            return TBASE::GetTrustLevelImpl(trustLvl);
        }

        STDMETHODIMP GetIids(_Out_ ULONG *iidCount, _Outptr_ IID **iids)
        {
            return TBASE::GetIIDsImpl(iidCount, iids);
        }
    public:
#if defined(_VARIADIC_SUPPORT)
        template <typename... Arguments>
        static HRESULT CreateInstance(_COM_Outptr_ TBASE **ppNewInstance, _In_ Arguments&&... params)
        {
            *ppNewInstance = new ComInspectableObject<TBASE>(params...);
#if defined(_TRACK_MEMORY_COM_TEMPLATE_ALLOC) && defined(DEBUG)
            Allocator::alloc(sizeof(ComInspectableObject<TBASE>));
#endif
            return *ppNewInstance != nullptr ? S_OK : E_OUTOFMEMORY;
        }
#else
        template <class TParameter>
        static HRESULT CreateInstance(_COM_Outptr_ TBASE **ppNewInstance, const TParameter& parameter)
        {
            *ppNewInstance = new ComInspectableObject<TBASE>(parameter);
            return *ppNewInstance != nullptr ? S_OK : E_OUTOFMEMORY;
        }

        template <class TParameter>
        static HRESULT CreateInstance(_COM_Outptr_ TBASE **ppNewInstance, TParameter& parameter)
        {
            *ppNewInstance = new ComInspectableObject<TBASE>(parameter);
            return *ppNewInstance != nullptr ? S_OK : E_OUTOFMEMORY;
        }
#endif
        template <class T>
        static HRESULT CreateInstance(_COM_Outptr_ T **ppNewInstance)
        {
            TBASE *pNewInstance = new ComInspectableObject<TBASE>();
            *ppNewInstance = static_cast<T *>(pNewInstance);
            return pNewInstance != nullptr ? S_OK : E_OUTOFMEMORY;
        }
    };

    template <class TNonDelegatingInterface, class TOuterInterface, class TBASE>
    class _AggregableComObject :
        public TNonDelegatingInterface,
        public TBASE
    {
    protected:

        _AggregableComObject(_In_ TOuterInterface* pOuter)
        {
            foundation_assert(pOuter != nullptr);
            m_pControllingUnknown = pOuter;
        }

        virtual ~_AggregableComObject()
        {
        }

        virtual bool IsComposed()
        {
            return true;
        }

        virtual TOuterInterface *GetInnerInterface() override
        {
            TNonDelegatingInterface *pNonDelegatingInterface = static_cast<TNonDelegatingInterface *>(this);
            return reinterpret_cast<TOuterInterface *>(pNonDelegatingInterface);
        }

    public:

        // foundation::IUnknown (non-delegating) implementation
        STDMETHODIMP NonDelegatingQueryInterface(REFIID iid, void **ppValue)
        {
            HRESULT hr = S_OK;
            foundation::IUnknown *pInterface = nullptr;

            if (iid == foundation_IID_IUnknown)
            {
                pInterface = reinterpret_cast<foundation::IUnknown*>(static_cast<INonDelegatingUnknown *>(this));
            }
            else
            {
                hr = TBASE::QueryInterfaceImpl(iid, &pInterface);
                if (FAILED(hr))
                {
                    return hr;
                }
            }

            *ppValue = pInterface;
            pInterface->AddRef();
            return S_OK;
        }

        STDMETHODIMP_(_pal_AtomicIntegerType) NonDelegatingAddRef()
        {
            return TBASE::AddRefImpl();
        }

        STDMETHODIMP_(_pal_AtomicIntegerType) NonDelegatingRelease()
        {
            return TBASE::ReleaseImpl();
        }


        // foundation::IUnknown (delegating) implementation
        STDMETHODIMP QueryInterface(REFIID iid, void **ppValue)
        {
            return m_pControllingUnknown->QueryInterface(iid, ppValue);
        }

        STDMETHODIMP_(_pal_AtomicIntegerType) AddRef()
        {
            return m_pControllingUnknown->AddRef();
        }

        STDMETHODIMP_(_pal_AtomicIntegerType) Release()
        {
            return m_pControllingUnknown->Release();
        }


    protected:
        TOuterInterface *GetControllingUnknown() { return m_pControllingUnknown; }
    private:
        TOuterInterface* m_pControllingUnknown;
    };

    template <class TBASE>
    class AggregableComObject :
        public _AggregableComObject<INonDelegatingUnknown, foundation::IUnknown, TBASE>
    {
    protected:

        AggregableComObject(_In_ foundation::IUnknown* pOuter) :
            _AggregableComObject<INonDelegatingUnknown, foundation::IUnknown, TBASE>(pOuter)
        {
#if defined(_TRACK_MEMORY_COM_TEMPLATE_ALLOC) && defined(DEBUG)
            Allocator::alloc(sizeof(AggregableComObject<TBASE>));
#endif
        }

        virtual ~AggregableComObject()
        {
#if defined(_TRACK_MEMORY_COM_TEMPLATE_ALLOC) && defined(DEBUG)
            Allocator::release(sizeof(AggregableComObject<TBASE>));
#endif
        }
    public:

        static HRESULT CreateInstance(_In_ foundation::IUnknown* pOuter, _COM_Outptr_ TBASE **ppNewInstance)
        {
            *ppNewInstance = new AggregableComObject<TBASE>(pOuter);
            return *ppNewInstance != nullptr ? S_OK : E_OUTOFMEMORY;
        }

        static HRESULT CreateInstance(_In_ foundation::IUnknown* pOuter, _COM_Outptr_ foundation::IUnknown **ppNewInstance)
        {
            TBASE *pNewInstance = new AggregableComObject<TBASE>(pOuter);
            *ppNewInstance = static_cast<foundation::IUnknown *>(static_cast<ComBase*>(pNewInstance));
            return *ppNewInstance != nullptr ? S_OK : E_OUTOFMEMORY;
        }

        template <class T>
        static HRESULT CreateInstance(_In_ foundation::IUnknown* pOuter, _COM_Outptr_ T **ppNewInstance)
        {
            TBASE *pNewInstance = new AggregableComObject<TBASE>(pOuter);
            *ppNewInstance = static_cast<T *>(pNewInstance);
            return *ppNewInstance != nullptr ? S_OK : E_OUTOFMEMORY;
        }
    };

    template <class TBASE>
    class AggregableComInspectableObject :
        public _AggregableComObject<INonDelegatingInspectable, foundation::IInspectable, TBASE>
    {
    private:
        typedef _AggregableComObject<INonDelegatingInspectable, foundation::IInspectable, TBASE> _TBase;
    protected:

        AggregableComInspectableObject(_In_ foundation::IInspectable* pOuter) :
            _TBase(pOuter)
        {
#if defined(_TRACK_MEMORY_COM_TEMPLATE_ALLOC) && defined(DEBUG)
            Allocator::alloc(sizeof(AggregableComInspectableObject<TBASE>));
#endif
        }

        virtual ~AggregableComInspectableObject()
        {
#if defined(_TRACK_MEMORY_COM_TEMPLATE_ALLOC) && defined(DEBUG)
            Allocator::release(sizeof(AggregableComInspectableObject<TBASE>));
#endif
        }
    public:
        // foundation::IInspectable (non-delegating) implementation
        STDMETHODIMP NonDelegatingQueryInterface(REFIID iid, _COM_Outptr_ void **ppValue)
        {
            HRESULT hr = S_OK;
            foundation::IUnknown *pInterface = nullptr;

            if (iid == foundation_IID_IUnknown)
            {
                pInterface = reinterpret_cast<foundation::IUnknown *>(static_cast<INonDelegatingUnknown *>(this));
            }
            else if (iid == foundation_IID_IInspectable)
            {
                pInterface = reinterpret_cast<foundation::IInspectable *>(static_cast<INonDelegatingInspectable *>(this));
            }
            else
            {
                hr = TBASE::QueryInterfaceImpl(iid, &pInterface);
                if (FAILED(hr))
                {
                    *ppValue = nullptr;
                    return hr;
                }
            }

            *ppValue = pInterface;
            pInterface->AddRef();
            return S_OK;
        }

        STDMETHODIMP NonDelegatingGetRuntimeClassName(_Out_ HSTRING *pClassName)
        {
            return TBASE::GetRuntimeClassNameImpl(pClassName);
        }

        STDMETHODIMP NonDelegatingGetTrustLevel(_Out_ foundation::TrustLevel *trustLvl)
        {
            return TBASE::GetTrustLevelImpl(trustLvl);
        }

        STDMETHODIMP NonDelegatingGetIids(_Out_ ULONG *iidCount, _Outptr_ IID **iids)
        {
            return TBASE::GetIIDsImpl(iidCount, iids);
        }
        // foundation::IInspectable (delegating) implementation

        STDMETHODIMP GetRuntimeClassName(_Out_ HSTRING *pClassName)
        {
            return _TBase::GetControllingUnknown()->GetRuntimeClassName(pClassName);
        }

        STDMETHODIMP GetTrustLevel(_Out_ foundation::TrustLevel *trustLvl)
        {
            return _TBase::GetControllingUnknown()->GetTrustLevel(trustLvl);
        }

        STDMETHODIMP GetIids(_Out_ ULONG *iidCount, __RPC__deref_out_ecount_full_opt(*iidCount) IID **iids)
        {
            return _TBase::GetControllingUnknown()->GetIids(iidCount, iids);
        }
    public:

        static HRESULT CreateInstance(_In_ foundation::IInspectable* pOuter, _COM_Outptr_ TBASE **ppNewInstance)
        {
            *ppNewInstance = new AggregableComInspectableObject<TBASE>(pOuter);
            return *ppNewInstance != nullptr ? S_OK : E_OUTOFMEMORY;
        }

        static HRESULT CreateInstance(_In_ foundation::IInspectable* pOuter, _COM_Outptr_ foundation::IInspectable **ppNewInstance)
        {
            AggregableComInspectableObject<TBASE> *pNewInstance = new AggregableComInspectableObject<TBASE>(pOuter);
            *ppNewInstance = reinterpret_cast<foundation::IInspectable *>(static_cast<INonDelegatingUnknown *>(pNewInstance));
            return *ppNewInstance != nullptr ? S_OK : E_OUTOFMEMORY;
        }

        template <class T>
        static HRESULT CreateInstance(_In_ foundation::IInspectable* pOuter, _COM_Outptr_ T **ppNewInstance)
        {
            TBASE *pNewInstance = new AggregableComInspectableObject<TBASE>(pOuter);
            *ppNewInstance = static_cast<T *>(pNewInstance);
            return *ppNewInstance != nullptr ? S_OK : E_OUTOFMEMORY;
        }
    };

    template <class TInner = foundation::IInspectable>
    class _InnerContainer
    {
    public:
        TInner *GetInnerRaw() { return _p_inner; }

        void _ReleaseInner()
        {
            // Release the Inner Model
            RELEASE_INTERFACE(_p_inner);
        }

    protected:
        _InnerContainer() :
            _p_inner(nullptr)
        {
        }

        ~_InnerContainer()
        {
            _ReleaseInner();
        }

        TInner **GetInnerAddressOf()
        {
            foundation_assert(_p_inner == nullptr);
            return &_p_inner;
        }

    private:
        TInner*        _p_inner;
    };

    struct _AggregatedBase
    {
    public:
        void _Cleanup()
        {}
    };

    template <class T = _AggregatedBase, class TInner = foundation::IInspectable>
    class AggregatedBase :
        public _InnerContainer<TInner>,
        public T
    {
    public:
        void _ReleaseInner()
        {
            // This call will ensure the destruction of 'm_pInnerModel' will NOT
            // invoke any vtbl on our derived classes
            this->_Cleanup();

            _InnerContainer<TInner>::_ReleaseInner();
        }

    protected:
        virtual ~AggregatedBase()
        {
            _ReleaseInner();
        }
    };

    template <class T, class TBASE>
    class ComAggregateBase :
        public TBASE        // ctl:ComBase type class
    {
    protected:
        virtual ~ComAggregateBase()
        {
#ifdef DEBUG
            // We expect that the Inner Unknown is released on the FinalRelease
            // not in the destructor
            // TODO:
            // This code will assume the Inner was created properly on the _Initialize
            T *pT = static_cast<T*>(this);
            foundation_assert(pT->GetInnerRaw() == nullptr);
#endif
        }

        // Override from TBASE
        void FinalRelease() override
        {
            // Base class
            TBASE::FinalRelease();

            T *pT = static_cast<T*>(this);
            // this will release the Inner Unknown before the destructor happen
            pT->_ReleaseInner();
        }

        HRESULT QueryInterfaceImpl(REFIID iid, _COM_Outptr_ foundation::IUnknown **ppInterface) override
        {
            HRESULT hr = TBASE::QueryInterfaceImpl(iid, ppInterface);
            if (SUCCEEDED(hr))
                return hr;

            T *pT = static_cast<T*>(this);
            foundation_assert(pT->GetInnerRaw());
            return foundation::QueryWeakReference(pT->GetInnerRaw(), iid, ppInterface);
        }
    };

    template <class _InspectableBase>
    class _Aggregated_inspectable_base :
        public _InspectableBase,
        public AggregatedBase <>
    {
    };

    typedef ComAggregateBase<
        _Aggregated_inspectable_base<ComInspectableBase>, 
        _Aggregated_inspectable_base<ComInspectableBase >> ComAggregateInspectableBase;

    template<class TInterface, class TBase, const IID *pIIDInterface>
    class ImplementsBase :
        public TBase,
        public TInterface
    {
    protected:
        HRESULT QueryInterfaceImpl(REFIID iid, _COM_Outptr_ foundation::IUnknown **ppInterface) override
        {
            if (iid == *pIIDInterface)
            {
                *ppInterface = static_cast<TInterface *>(this);
                return S_OK;
            }

            return TBase::QueryInterfaceImpl(iid, ppInterface);
        }
    };

    template<class TInterface, const IID *pIIDInterface>
    class Implements :
        public ImplementsBase<TInterface, ComBase, pIIDInterface>
    {
    public:
        template <class T>
        static HRESULT CreateInstance(_COM_Outptr_ T **ppNewInstance)
        {
            return ComObject<T>::CreateInstance(ppNewInstance);
        }
    };

    template<class TInterface, const IID *pIIDInterface>
    class ImplementsInspectable :
        public ImplementsBase<TInterface, ComInspectableBase, pIIDInterface>
    {
    public:
        template <class T>
        static HRESULT CreateInstance(_COM_Outptr_ T **ppNewInstance)
        {
            return ComInspectableObject<T>::CreateInstance(ppNewInstance);
        }
    };

    template <class T>
    ULONG AddRefInterface(_In_opt_ T* pInterface)
    {
        foundation::IUnknown *pUnk = static_cast<foundation::IUnknown *>(static_cast<typename T::_ComBaseType *>(pInterface));
        ULONG ref = -1;
        if (pUnk)
        {
            ref = pUnk->AddRef();
        }
        return ref;
    }

    template <class T>
    ULONG ReleaseInterface(T*& pInterface)
    {
        foundation::IUnknown *pUnk = static_cast<foundation::IUnknown *>(static_cast<typename T::_ComBaseType *>(pInterface));

        ULONG ref = -1;
        if (pUnk)
        {
            ref = pUnk->Release();
        }

        pInterface = nullptr;
        return ref;
    }
    template <class T>
    foundation::IUnknown *CastToUnknown(T* pInterface)
    {
        return static_cast<foundation::IUnknown *>(static_cast<typename T::_ComBaseType *>(pInterface));
    }

    template <class T>
    static inline HRESULT CreateInstance(_COM_Outptr_ T **ppNewInstance)
    {
        return ComObject<T>::CreateInstance(ppNewInstance);
    }

    template<class TClass, class TInterface, TYPE_NAME_CREATE_VAR_ARGUMENTS>
    static HRESULT CreateInstanceWithInitialize(
        _COM_Outptr_ TInterface **ppNewInstance,
        _In_ DECLARE_CREATE_VAR_ARGUMENTS)
    {
        TClass* pClass;
        IFR_ASSERT(ComObject<TClass>::CreateInstance(&pClass));
        IFR_ASSERT(pClass->_Initialize(PASS_CREATE_VAR_ARGUMENTS));
        *ppNewInstance = static_cast<TInterface*>(pClass);
        return S_OK;

    }

    template <class T, TYPE_NAME_CREATE_VAR_ARGUMENTS>
    static HRESULT CreateInstanceWithInitialize(
        _COM_Outptr_ T **ppNewInstance,
        _In_ DECLARE_CREATE_VAR_ARGUMENTS
        )
    {
        foundation_assert(ppNewInstance != nullptr);
        *ppNewInstance = nullptr;
        
        ComPtr<T,_ComBasePtrImpl> newInstance;
        IFR_ASSERT(ComObject<T>::CreateInstance(newInstance.GetAddressOf()));
        IFR_ASSERT(newInstance->_Initialize(PASS_CREATE_VAR_ARGUMENTS));
        *ppNewInstance = newInstance.Detach();
        return S_OK;
    }

    template <class T>
    class AggregableCreator
    {
    public:
        template <TYPE_NAME_CREATE_VAR_ARGUMENTS>
        static HRESULT CreateInstanceWithInitialize(
            _In_ foundation::IUnknown *pOuter,
            _COM_Outptr_ foundation::IUnknown **ppNewInstance,
            _In_ DECLARE_CREATE_VAR_ARGUMENTS)
        {
            AggregableComObject<T> *pT = nullptr;
            IFR_ASSERT(AggregableComObject<T>::CreateInstance(pOuter, &pT));
            IFR_ASSERT(pT->_Initialize(PASS_CREATE_VAR_ARGUMENTS));
            *ppNewInstance = reinterpret_cast<foundation::IUnknown *>(static_cast<INonDelegatingUnknown *>(pT));
            return S_OK;
        }
    };

    namespace inspectable
    {
        template <class T>
        ULONG AddRefInterface(T* pInterface)
        {
            foundation::IUnknown *pUnk = static_cast<foundation::IUnknown *>(static_cast<typename T::_ComInspectableBaseType *>(pInterface));
            ULONG ref = -1;

            if (pUnk)
            {
                ref = pUnk->AddRef();
            }
            return ref;
        }

        template <class T>
        ULONG ReleaseInterface(T*& pInterface)
        {
            foundation::IUnknown *pUnk = static_cast<foundation::IUnknown *>(static_cast<typename T::_ComInspectableBaseType *>(pInterface));
            ULONG ref = -1;

            if (pUnk)
            {
                ref = pUnk->Release();
            }

            pInterface = nullptr;
            return ref;
        }


        template <class T>
        inline foundation::IUnknown *CastToUnknown(T* pInterface)
        {
            return static_cast<foundation::IUnknown *>(static_cast<typename T::_ComInspectableBaseType *>(pInterface));
        }

        template <class T>
        inline foundation::IInspectable *CastToInspectable(T* pInterface)
        {
            return static_cast<foundation::IInspectable *>(static_cast<typename T::_ComInspectableBaseType *>(pInterface));
        }

        template <class T>
        static inline HRESULT CreateInstance(_COM_Outptr_ T **ppNewInstance)
        {
            return ComInspectableObject<T>::CreateInstance(ppNewInstance);
        }

        template <class T, TYPE_NAME_CREATE_VAR_ARGUMENTS>
        static HRESULT CreateInstanceWithInitialize(
            _COM_Outptr_ T **ppNewInstance,
            _In_ DECLARE_CREATE_VAR_ARGUMENTS
            )
        {
            foundation_assert(ppNewInstance != nullptr);
            *ppNewInstance = nullptr;
            ComPtr<T,_ComBasePtrImpl> newInstance;
            IFR_ASSERT(ComInspectableObject<T>::CreateInstance(newInstance.GetAddressOf()));
            _IFR_(newInstance->_Initialize(PASS_CREATE_VAR_ARGUMENTS));
            *ppNewInstance = newInstance.Detach();
            return S_OK;
        }

        template<class TClass, class TInterface, TYPE_NAME_CREATE_VAR_ARGUMENTS>
        static HRESULT CreateInstanceWithInitialize(
            _COM_Outptr_ TInterface **ppNewInstance,
            _In_ DECLARE_CREATE_VAR_ARGUMENTS)
        {
            foundation_assert(ppNewInstance != nullptr);
            *ppNewInstance = nullptr;
            TClass * pClass;
            IFR_ASSERT(ComInspectableObject<TClass>::CreateInstance(&pClass));
            _IFR_(pClass->_Initialize(PASS_CREATE_VAR_ARGUMENTS));
            *ppNewInstance = static_cast<TInterface *>(pClass);
            return S_OK;
        }

        template<class T, TYPE_NAME_CREATE_VAR_ARGUMENTS>
        HRESULT  CreateInstanceWithInitialize(
            _In_ foundation::IInspectable *pOuter,
            _COM_Outptr_ foundation::IInspectable **ppNewInstance,
            _In_ DECLARE_CREATE_VAR_ARGUMENTS)
        {
            foundation_assert(ppNewInstance != nullptr);
            *ppNewInstance = nullptr;

            HRESULT hr;
            if (pOuter)
            {
                AggregableComInspectableObject<T> *pInstance;
                IFHR_ASSERT(AggregableComInspectableObject<T>::CreateInstance(pOuter, &pInstance));
                hr = pInstance->_Initialize(PASS_CREATE_VAR_ARGUMENTS);
                _IFHR_(hr);
                *ppNewInstance = reinterpret_cast<foundation::IInspectable*>(static_cast<INonDelegatingUnknown *>(pInstance));
            }
            else
            {
                ComInspectableObject<T> *pInstance;
                IFHR_ASSERT(ComInspectableObject<T>::CreateInstance(&pInstance));
                hr = pInstance->_Initialize(PASS_CREATE_VAR_ARGUMENTS);
                _IFHR_(hr);
                *ppNewInstance = reinterpret_cast<foundation::IInspectable*>(CastToInspectable(pInstance));
            }
            return S_OK;
        }

        template <class T>
        class AggregableCreator
        {
        public:
            template <TYPE_NAME_CREATE_VAR_ARGUMENTS>
            static HRESULT CreateInstanceWithInitialize(
                _In_ foundation::IInspectable *pOuter,
                _COM_Outptr_ foundation::IInspectable **ppNewInstance,
                _In_ DECLARE_CREATE_VAR_ARGUMENTS)
            {
                foundation_assert(ppNewInstance != nullptr);
                *ppNewInstance = nullptr;
                AggregableComInspectableObject<T> *pT = nullptr;
                IFR_ASSERT(AggregableComInspectableObject<T>::CreateInstance(pOuter, &pT));
                _IFR_(pT->_Initialize(PASS_CREATE_VAR_ARGUMENTS));
                *ppNewInstance = reinterpret_cast<foundation::IInspectable *>(static_cast<INonDelegatingUnknown *>(pT));
                return S_OK;
            }
        };
    }
}
}

#define ADDREF_CLASS(p_class) \
        foundation::ctl::AddRefInterface(p_class)

#define RELEASE_CLASS(p_class) \
        foundation::ctl::ReleaseInterface(p_class)

#define ADDREF_INSPECTABLE_CLASS(p_class) \
        foundation::ctl::inspectable::AddRefInterface(p_class)

#define RELEASE_INSPECTABLE_CLASS(p_class) \
        foundation::ctl::inspectable::ReleaseInterface(p_class)

#define BEGIN_IID_MAP \
   private:\
      IID* GetLocalImplementedIIDs(_Out_ INT *count)\
      {\
         static IID localIIDs[] = \
         {
#define IID_ENTRY(iid)               iid,
#define IID_INTERFACE_ENTRY(Q)       Q::GetIID(),

#if defined(_WINDOWS)
#define UUIDOF_INTERFACE_ENTRY(i)                   __uuidof(i),
#endif

#define END_IID_MAP(BASE) \
         };\
         \
         *count = sizeof(localIIDs) / sizeof(IID);\
         \
         return localIIDs;\
      }\
      \
      INT GetLocalImplementedIIDsCount()\
      {\
         INT count = 0;\
         \
         GetLocalImplementedIIDs(&count);\
         \
         return count;\
      }\
      \
   protected:\
   \
      INT GetImplementedIIDsCount()\
      {\
         return GetLocalImplementedIIDsCount() + BASE::GetImplementedIIDsCount();\
      }\
      \
      virtual void CopyIIDsToArray(INT first, IID *pResult)\
      {\
         INT count = 0;\
         IID *pLocalIIDs = GetLocalImplementedIIDs(&count);\
         INT current = 0;\
         \
         for (current = 0; current < count; current++)\
                  {\
            pResult[first + current] = pLocalIIDs[current];\
                  }\
         \
         BASE::CopyIIDsToArray(first + current, pResult);\
      }

// Warning: returns the element COUNT, not the SIZE in bytes, as the name might imply.
#define _ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

// Return the number of characters in a null-terminated string (not including the null)
#define SZ_COUNT(S) (_ARRAY_SIZE(S) - 1)

#ifdef INSPECTABLE_CLASS
#undef INSPECTABLE_CLASS
#endif

#define INSPECTABLE_CLASS(NAME, TRUSTLEVEL) \
      protected:\
      HRESULT GetRuntimeClassNameImpl(_Out_ HSTRING *className)\
      {\
        return _pal_CreateString(NAME, SZ_COUNT(NAME), className);\
      }\
      \
      HRESULT GetTrustLevelImpl(_Out_ foundation::TrustLevel *trustLvl)\
      {\
         *trustLvl = TRUSTLEVEL;\
         return S_OK; \
      }

