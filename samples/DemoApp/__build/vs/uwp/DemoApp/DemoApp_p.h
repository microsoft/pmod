

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.01.0622 */
/* at Mon Jan 18 19:14:07 2038
 */
/* Compiler settings for C:\Users\rodrigov\AppData\Local\Temp\DemoApp.idl-567385cf:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.01.0622 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */



/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 500
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif /* __RPCNDR_H_VERSION__ */

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __DemoApp_p_h__
#define __DemoApp_p_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#if defined(__cplusplus)
#if defined(__MIDL_USE_C_ENUM)
#define MIDL_ENUM enum
#else
#define MIDL_ENUM enum class
#endif
#endif


/* Forward Declarations */ 

#ifndef ____FIIterator_1_DemoApp__CIItem_FWD_DEFINED__
#define ____FIIterator_1_DemoApp__CIItem_FWD_DEFINED__
typedef interface __FIIterator_1_DemoApp__CIItem __FIIterator_1_DemoApp__CIItem;

#endif 	/* ____FIIterator_1_DemoApp__CIItem_FWD_DEFINED__ */


#ifndef ____FIIterable_1_DemoApp__CIItem_FWD_DEFINED__
#define ____FIIterable_1_DemoApp__CIItem_FWD_DEFINED__
typedef interface __FIIterable_1_DemoApp__CIItem __FIIterable_1_DemoApp__CIItem;

#endif 	/* ____FIIterable_1_DemoApp__CIItem_FWD_DEFINED__ */


#ifndef ____FIVectorView_1_DemoApp__CIItem_FWD_DEFINED__
#define ____FIVectorView_1_DemoApp__CIItem_FWD_DEFINED__
typedef interface __FIVectorView_1_DemoApp__CIItem __FIVectorView_1_DemoApp__CIItem;

#endif 	/* ____FIVectorView_1_DemoApp__CIItem_FWD_DEFINED__ */


#ifndef ____FIVector_1_DemoApp__CIItem_FWD_DEFINED__
#define ____FIVector_1_DemoApp__CIItem_FWD_DEFINED__
typedef interface __FIVector_1_DemoApp__CIItem __FIVector_1_DemoApp__CIItem;

#endif 	/* ____FIVector_1_DemoApp__CIItem_FWD_DEFINED__ */


#ifndef ____FIAsyncOperationCompletedHandler_1_DemoApp__CIItem_FWD_DEFINED__
#define ____FIAsyncOperationCompletedHandler_1_DemoApp__CIItem_FWD_DEFINED__
typedef interface __FIAsyncOperationCompletedHandler_1_DemoApp__CIItem __FIAsyncOperationCompletedHandler_1_DemoApp__CIItem;

#endif 	/* ____FIAsyncOperationCompletedHandler_1_DemoApp__CIItem_FWD_DEFINED__ */


#ifndef ____FIAsyncOperation_1_DemoApp__CIItem_FWD_DEFINED__
#define ____FIAsyncOperation_1_DemoApp__CIItem_FWD_DEFINED__
typedef interface __FIAsyncOperation_1_DemoApp__CIItem __FIAsyncOperation_1_DemoApp__CIItem;

#endif 	/* ____FIAsyncOperation_1_DemoApp__CIItem_FWD_DEFINED__ */


#ifndef ____x_ABI_CDemoApp_CIItem_FWD_DEFINED__
#define ____x_ABI_CDemoApp_CIItem_FWD_DEFINED__
typedef interface __x_ABI_CDemoApp_CIItem __x_ABI_CDemoApp_CIItem;

#endif 	/* ____x_ABI_CDemoApp_CIItem_FWD_DEFINED__ */


#ifndef ____x_ABI_CDemoApp_CIAppDemo_FWD_DEFINED__
#define ____x_ABI_CDemoApp_CIAppDemo_FWD_DEFINED__
typedef interface __x_ABI_CDemoApp_CIAppDemo __x_ABI_CDemoApp_CIAppDemo;

#endif 	/* ____x_ABI_CDemoApp_CIAppDemo_FWD_DEFINED__ */


#ifndef ____x_ABI_CDemoApp_CIAppDemoFactory_FWD_DEFINED__
#define ____x_ABI_CDemoApp_CIAppDemoFactory_FWD_DEFINED__
typedef interface __x_ABI_CDemoApp_CIAppDemoFactory __x_ABI_CDemoApp_CIAppDemoFactory;

#endif 	/* ____x_ABI_CDemoApp_CIAppDemoFactory_FWD_DEFINED__ */


#ifndef ____x_ABI_CDemoApp_CICommand__string__void__canExecute_FWD_DEFINED__
#define ____x_ABI_CDemoApp_CICommand__string__void__canExecute_FWD_DEFINED__
typedef interface __x_ABI_CDemoApp_CICommand__string__void__canExecute __x_ABI_CDemoApp_CICommand__string__void__canExecute;

#endif 	/* ____x_ABI_CDemoApp_CICommand__string__void__canExecute_FWD_DEFINED__ */


#ifndef ____x_ABI_CDemoApp_CICommand__IItem__void__canExecute_FWD_DEFINED__
#define ____x_ABI_CDemoApp_CICommand__IItem__void__canExecute_FWD_DEFINED__
typedef interface __x_ABI_CDemoApp_CICommand__IItem__void__canExecute __x_ABI_CDemoApp_CICommand__IItem__void__canExecute;

#endif 	/* ____x_ABI_CDemoApp_CICommand__IItem__void__canExecute_FWD_DEFINED__ */


#ifndef ____x_ABI_CDemoApp_CICommand__string__void_FWD_DEFINED__
#define ____x_ABI_CDemoApp_CICommand__string__void_FWD_DEFINED__
typedef interface __x_ABI_CDemoApp_CICommand__string__void __x_ABI_CDemoApp_CICommand__string__void;

#endif 	/* ____x_ABI_CDemoApp_CICommand__string__void_FWD_DEFINED__ */


#ifndef ____x_ABI_CDemoApp_CIClassStatics_FWD_DEFINED__
#define ____x_ABI_CDemoApp_CIClassStatics_FWD_DEFINED__
typedef interface __x_ABI_CDemoApp_CIClassStatics __x_ABI_CDemoApp_CIClassStatics;

#endif 	/* ____x_ABI_CDemoApp_CIClassStatics_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "inspectable.h"
#include "eventtoken.h"
#include "windows.foundation.h"
#include "Microsoft.PropertyModel.h"

#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_DemoApp_0000_0000 */
/* [local] */ 
















#pragma warning (push)
#pragma warning (disable:4668) 
#pragma warning (disable:4001) 
#pragma once 
#pragma warning (pop)







extern RPC_IF_HANDLE __MIDL_itf_DemoApp_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_DemoApp_0000_0000_v0_0_s_ifspec;

/* interface __MIDL_itf_DemoApp2Eidl_0000_2369 */




extern RPC_IF_HANDLE __MIDL_itf_DemoApp2Eidl_0000_2369_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_DemoApp2Eidl_0000_2369_v0_0_s_ifspec;

/* interface __MIDL_itf_DemoApp_0000_0001 */
/* [local] */ 

#ifndef DEF___FIIterator_1_DemoApp__CIItem
#define DEF___FIIterator_1_DemoApp__CIItem
#if !defined(__cplusplus) || defined(RO_NO_TEMPLATE_NAME)


extern RPC_IF_HANDLE __MIDL_itf_DemoApp_0000_0001_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_DemoApp_0000_0001_v0_0_s_ifspec;

#ifndef ____FIIterator_1_DemoApp__CIItem_INTERFACE_DEFINED__
#define ____FIIterator_1_DemoApp__CIItem_INTERFACE_DEFINED__

/* interface __FIIterator_1_DemoApp__CIItem */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID___FIIterator_1_DemoApp__CIItem;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("b8f1f5c5-a1c9-5144-a44b-0f8271a1cbfb")
    __FIIterator_1_DemoApp__CIItem : public IInspectable
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Current( 
            /* [retval][out] */ __x_ABI_CDemoApp_CIItem **current) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_HasCurrent( 
            /* [retval][out] */ boolean *hasCurrent) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE MoveNext( 
            /* [retval][out] */ boolean *hasCurrent) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMany( 
            /* [in] */ unsigned int capacity,
            /* [size_is][length_is][out] */ __x_ABI_CDemoApp_CIItem **items,
            /* [retval][out] */ unsigned int *actual) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct __FIIterator_1_DemoApp__CIItemVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __FIIterator_1_DemoApp__CIItem * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __FIIterator_1_DemoApp__CIItem * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __FIIterator_1_DemoApp__CIItem * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetIids )( 
            __FIIterator_1_DemoApp__CIItem * This,
            /* [out] */ ULONG *iidCount,
            /* [size_is][size_is][out] */ IID **iids);
        
        HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )( 
            __FIIterator_1_DemoApp__CIItem * This,
            /* [out] */ HSTRING *className);
        
        HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )( 
            __FIIterator_1_DemoApp__CIItem * This,
            /* [out] */ TrustLevel *trustLevel);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )( 
            __FIIterator_1_DemoApp__CIItem * This,
            /* [retval][out] */ __x_ABI_CDemoApp_CIItem **current);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_HasCurrent )( 
            __FIIterator_1_DemoApp__CIItem * This,
            /* [retval][out] */ boolean *hasCurrent);
        
        HRESULT ( STDMETHODCALLTYPE *MoveNext )( 
            __FIIterator_1_DemoApp__CIItem * This,
            /* [retval][out] */ boolean *hasCurrent);
        
        HRESULT ( STDMETHODCALLTYPE *GetMany )( 
            __FIIterator_1_DemoApp__CIItem * This,
            /* [in] */ unsigned int capacity,
            /* [size_is][length_is][out] */ __x_ABI_CDemoApp_CIItem **items,
            /* [retval][out] */ unsigned int *actual);
        
        END_INTERFACE
    } __FIIterator_1_DemoApp__CIItemVtbl;

    interface __FIIterator_1_DemoApp__CIItem
    {
        CONST_VTBL struct __FIIterator_1_DemoApp__CIItemVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define __FIIterator_1_DemoApp__CIItem_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIIterator_1_DemoApp__CIItem_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIIterator_1_DemoApp__CIItem_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIIterator_1_DemoApp__CIItem_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIIterator_1_DemoApp__CIItem_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIIterator_1_DemoApp__CIItem_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIIterator_1_DemoApp__CIItem_get_Current(This,current)	\
    ( (This)->lpVtbl -> get_Current(This,current) ) 

#define __FIIterator_1_DemoApp__CIItem_get_HasCurrent(This,hasCurrent)	\
    ( (This)->lpVtbl -> get_HasCurrent(This,hasCurrent) ) 

#define __FIIterator_1_DemoApp__CIItem_MoveNext(This,hasCurrent)	\
    ( (This)->lpVtbl -> MoveNext(This,hasCurrent) ) 

#define __FIIterator_1_DemoApp__CIItem_GetMany(This,capacity,items,actual)	\
    ( (This)->lpVtbl -> GetMany(This,capacity,items,actual) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* ____FIIterator_1_DemoApp__CIItem_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_DemoApp_0000_0002 */
/* [local] */ 

#endif /* pinterface */
#endif /* DEF___FIIterator_1_DemoApp__CIItem */


extern RPC_IF_HANDLE __MIDL_itf_DemoApp_0000_0002_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_DemoApp_0000_0002_v0_0_s_ifspec;

/* interface __MIDL_itf_DemoApp2Eidl_0000_2370 */




extern RPC_IF_HANDLE __MIDL_itf_DemoApp2Eidl_0000_2370_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_DemoApp2Eidl_0000_2370_v0_0_s_ifspec;

/* interface __MIDL_itf_DemoApp_0000_0003 */
/* [local] */ 

#ifndef DEF___FIIterable_1_DemoApp__CIItem
#define DEF___FIIterable_1_DemoApp__CIItem
#if !defined(__cplusplus) || defined(RO_NO_TEMPLATE_NAME)


extern RPC_IF_HANDLE __MIDL_itf_DemoApp_0000_0003_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_DemoApp_0000_0003_v0_0_s_ifspec;

#ifndef ____FIIterable_1_DemoApp__CIItem_INTERFACE_DEFINED__
#define ____FIIterable_1_DemoApp__CIItem_INTERFACE_DEFINED__

/* interface __FIIterable_1_DemoApp__CIItem */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID___FIIterable_1_DemoApp__CIItem;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("f7df8180-fa4a-5e60-a8ac-1dd53fe48f33")
    __FIIterable_1_DemoApp__CIItem : public IInspectable
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE First( 
            /* [retval][out] */ __FIIterator_1_DemoApp__CIItem **first) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct __FIIterable_1_DemoApp__CIItemVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __FIIterable_1_DemoApp__CIItem * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __FIIterable_1_DemoApp__CIItem * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __FIIterable_1_DemoApp__CIItem * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetIids )( 
            __FIIterable_1_DemoApp__CIItem * This,
            /* [out] */ ULONG *iidCount,
            /* [size_is][size_is][out] */ IID **iids);
        
        HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )( 
            __FIIterable_1_DemoApp__CIItem * This,
            /* [out] */ HSTRING *className);
        
        HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )( 
            __FIIterable_1_DemoApp__CIItem * This,
            /* [out] */ TrustLevel *trustLevel);
        
        HRESULT ( STDMETHODCALLTYPE *First )( 
            __FIIterable_1_DemoApp__CIItem * This,
            /* [retval][out] */ __FIIterator_1_DemoApp__CIItem **first);
        
        END_INTERFACE
    } __FIIterable_1_DemoApp__CIItemVtbl;

    interface __FIIterable_1_DemoApp__CIItem
    {
        CONST_VTBL struct __FIIterable_1_DemoApp__CIItemVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define __FIIterable_1_DemoApp__CIItem_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIIterable_1_DemoApp__CIItem_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIIterable_1_DemoApp__CIItem_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIIterable_1_DemoApp__CIItem_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIIterable_1_DemoApp__CIItem_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIIterable_1_DemoApp__CIItem_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIIterable_1_DemoApp__CIItem_First(This,first)	\
    ( (This)->lpVtbl -> First(This,first) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* ____FIIterable_1_DemoApp__CIItem_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_DemoApp_0000_0004 */
/* [local] */ 

#endif /* pinterface */
#endif /* DEF___FIIterable_1_DemoApp__CIItem */


extern RPC_IF_HANDLE __MIDL_itf_DemoApp_0000_0004_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_DemoApp_0000_0004_v0_0_s_ifspec;

/* interface __MIDL_itf_DemoApp2Eidl_0000_2371 */




extern RPC_IF_HANDLE __MIDL_itf_DemoApp2Eidl_0000_2371_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_DemoApp2Eidl_0000_2371_v0_0_s_ifspec;

/* interface __MIDL_itf_DemoApp_0000_0005 */
/* [local] */ 

#ifndef DEF___FIVectorView_1_DemoApp__CIItem
#define DEF___FIVectorView_1_DemoApp__CIItem
#if !defined(__cplusplus) || defined(RO_NO_TEMPLATE_NAME)


extern RPC_IF_HANDLE __MIDL_itf_DemoApp_0000_0005_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_DemoApp_0000_0005_v0_0_s_ifspec;

#ifndef ____FIVectorView_1_DemoApp__CIItem_INTERFACE_DEFINED__
#define ____FIVectorView_1_DemoApp__CIItem_INTERFACE_DEFINED__

/* interface __FIVectorView_1_DemoApp__CIItem */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID___FIVectorView_1_DemoApp__CIItem;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4b30ed61-5d25-5fc0-8b75-2e1bb756b53a")
    __FIVectorView_1_DemoApp__CIItem : public IInspectable
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetAt( 
            /* [in] */ unsigned int index,
            /* [retval][out] */ __x_ABI_CDemoApp_CIItem **item) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Size( 
            /* [retval][out] */ unsigned int *size) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IndexOf( 
            /* [in] */ __x_ABI_CDemoApp_CIItem *item,
            /* [out] */ unsigned int *index,
            /* [retval][out] */ boolean *found) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMany( 
            /* [in] */ unsigned int startIndex,
            /* [in] */ unsigned int capacity,
            /* [size_is][length_is][out] */ __x_ABI_CDemoApp_CIItem **items,
            /* [retval][out] */ unsigned int *actual) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct __FIVectorView_1_DemoApp__CIItemVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __FIVectorView_1_DemoApp__CIItem * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __FIVectorView_1_DemoApp__CIItem * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __FIVectorView_1_DemoApp__CIItem * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetIids )( 
            __FIVectorView_1_DemoApp__CIItem * This,
            /* [out] */ ULONG *iidCount,
            /* [size_is][size_is][out] */ IID **iids);
        
        HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )( 
            __FIVectorView_1_DemoApp__CIItem * This,
            /* [out] */ HSTRING *className);
        
        HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )( 
            __FIVectorView_1_DemoApp__CIItem * This,
            /* [out] */ TrustLevel *trustLevel);
        
        HRESULT ( STDMETHODCALLTYPE *GetAt )( 
            __FIVectorView_1_DemoApp__CIItem * This,
            /* [in] */ unsigned int index,
            /* [retval][out] */ __x_ABI_CDemoApp_CIItem **item);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Size )( 
            __FIVectorView_1_DemoApp__CIItem * This,
            /* [retval][out] */ unsigned int *size);
        
        HRESULT ( STDMETHODCALLTYPE *IndexOf )( 
            __FIVectorView_1_DemoApp__CIItem * This,
            /* [in] */ __x_ABI_CDemoApp_CIItem *item,
            /* [out] */ unsigned int *index,
            /* [retval][out] */ boolean *found);
        
        HRESULT ( STDMETHODCALLTYPE *GetMany )( 
            __FIVectorView_1_DemoApp__CIItem * This,
            /* [in] */ unsigned int startIndex,
            /* [in] */ unsigned int capacity,
            /* [size_is][length_is][out] */ __x_ABI_CDemoApp_CIItem **items,
            /* [retval][out] */ unsigned int *actual);
        
        END_INTERFACE
    } __FIVectorView_1_DemoApp__CIItemVtbl;

    interface __FIVectorView_1_DemoApp__CIItem
    {
        CONST_VTBL struct __FIVectorView_1_DemoApp__CIItemVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define __FIVectorView_1_DemoApp__CIItem_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIVectorView_1_DemoApp__CIItem_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIVectorView_1_DemoApp__CIItem_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIVectorView_1_DemoApp__CIItem_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIVectorView_1_DemoApp__CIItem_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIVectorView_1_DemoApp__CIItem_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIVectorView_1_DemoApp__CIItem_GetAt(This,index,item)	\
    ( (This)->lpVtbl -> GetAt(This,index,item) ) 

#define __FIVectorView_1_DemoApp__CIItem_get_Size(This,size)	\
    ( (This)->lpVtbl -> get_Size(This,size) ) 

#define __FIVectorView_1_DemoApp__CIItem_IndexOf(This,item,index,found)	\
    ( (This)->lpVtbl -> IndexOf(This,item,index,found) ) 

#define __FIVectorView_1_DemoApp__CIItem_GetMany(This,startIndex,capacity,items,actual)	\
    ( (This)->lpVtbl -> GetMany(This,startIndex,capacity,items,actual) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* ____FIVectorView_1_DemoApp__CIItem_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_DemoApp_0000_0006 */
/* [local] */ 

#endif /* pinterface */
#endif /* DEF___FIVectorView_1_DemoApp__CIItem */


extern RPC_IF_HANDLE __MIDL_itf_DemoApp_0000_0006_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_DemoApp_0000_0006_v0_0_s_ifspec;

/* interface __MIDL_itf_DemoApp2Eidl_0000_2372 */




extern RPC_IF_HANDLE __MIDL_itf_DemoApp2Eidl_0000_2372_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_DemoApp2Eidl_0000_2372_v0_0_s_ifspec;

/* interface __MIDL_itf_DemoApp_0000_0007 */
/* [local] */ 

#ifndef DEF___FIVector_1_DemoApp__CIItem
#define DEF___FIVector_1_DemoApp__CIItem
#if !defined(__cplusplus) || defined(RO_NO_TEMPLATE_NAME)


extern RPC_IF_HANDLE __MIDL_itf_DemoApp_0000_0007_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_DemoApp_0000_0007_v0_0_s_ifspec;

#ifndef ____FIVector_1_DemoApp__CIItem_INTERFACE_DEFINED__
#define ____FIVector_1_DemoApp__CIItem_INTERFACE_DEFINED__

/* interface __FIVector_1_DemoApp__CIItem */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID___FIVector_1_DemoApp__CIItem;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3b798196-49d9-5a4e-ad33-70135805a87b")
    __FIVector_1_DemoApp__CIItem : public IInspectable
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetAt( 
            /* [in] */ unsigned int index,
            /* [retval][out] */ __x_ABI_CDemoApp_CIItem **item) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Size( 
            /* [retval][out] */ unsigned int *size) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetView( 
            /* [retval][out] */ __FIVectorView_1_DemoApp__CIItem **view) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IndexOf( 
            /* [in] */ __x_ABI_CDemoApp_CIItem *item,
            /* [out] */ unsigned int *index,
            /* [retval][out] */ boolean *found) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetAt( 
            /* [in] */ unsigned int index,
            /* [in] */ __x_ABI_CDemoApp_CIItem *item) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE InsertAt( 
            /* [in] */ unsigned int index,
            /* [in] */ __x_ABI_CDemoApp_CIItem *item) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveAt( 
            /* [in] */ unsigned int index) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Append( 
            /* [in] */ __x_ABI_CDemoApp_CIItem *item) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveAtEnd( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clear( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMany( 
            /* [in] */ unsigned int startIndex,
            /* [in] */ unsigned int capacity,
            /* [size_is][length_is][out] */ __x_ABI_CDemoApp_CIItem **items,
            /* [retval][out] */ unsigned int *actual) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ReplaceAll( 
            /* [in] */ unsigned int count,
            /* [size_is][in] */ __x_ABI_CDemoApp_CIItem **value) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct __FIVector_1_DemoApp__CIItemVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __FIVector_1_DemoApp__CIItem * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __FIVector_1_DemoApp__CIItem * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __FIVector_1_DemoApp__CIItem * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetIids )( 
            __FIVector_1_DemoApp__CIItem * This,
            /* [out] */ ULONG *iidCount,
            /* [size_is][size_is][out] */ IID **iids);
        
        HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )( 
            __FIVector_1_DemoApp__CIItem * This,
            /* [out] */ HSTRING *className);
        
        HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )( 
            __FIVector_1_DemoApp__CIItem * This,
            /* [out] */ TrustLevel *trustLevel);
        
        HRESULT ( STDMETHODCALLTYPE *GetAt )( 
            __FIVector_1_DemoApp__CIItem * This,
            /* [in] */ unsigned int index,
            /* [retval][out] */ __x_ABI_CDemoApp_CIItem **item);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Size )( 
            __FIVector_1_DemoApp__CIItem * This,
            /* [retval][out] */ unsigned int *size);
        
        HRESULT ( STDMETHODCALLTYPE *GetView )( 
            __FIVector_1_DemoApp__CIItem * This,
            /* [retval][out] */ __FIVectorView_1_DemoApp__CIItem **view);
        
        HRESULT ( STDMETHODCALLTYPE *IndexOf )( 
            __FIVector_1_DemoApp__CIItem * This,
            /* [in] */ __x_ABI_CDemoApp_CIItem *item,
            /* [out] */ unsigned int *index,
            /* [retval][out] */ boolean *found);
        
        HRESULT ( STDMETHODCALLTYPE *SetAt )( 
            __FIVector_1_DemoApp__CIItem * This,
            /* [in] */ unsigned int index,
            /* [in] */ __x_ABI_CDemoApp_CIItem *item);
        
        HRESULT ( STDMETHODCALLTYPE *InsertAt )( 
            __FIVector_1_DemoApp__CIItem * This,
            /* [in] */ unsigned int index,
            /* [in] */ __x_ABI_CDemoApp_CIItem *item);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveAt )( 
            __FIVector_1_DemoApp__CIItem * This,
            /* [in] */ unsigned int index);
        
        HRESULT ( STDMETHODCALLTYPE *Append )( 
            __FIVector_1_DemoApp__CIItem * This,
            /* [in] */ __x_ABI_CDemoApp_CIItem *item);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveAtEnd )( 
            __FIVector_1_DemoApp__CIItem * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clear )( 
            __FIVector_1_DemoApp__CIItem * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetMany )( 
            __FIVector_1_DemoApp__CIItem * This,
            /* [in] */ unsigned int startIndex,
            /* [in] */ unsigned int capacity,
            /* [size_is][length_is][out] */ __x_ABI_CDemoApp_CIItem **items,
            /* [retval][out] */ unsigned int *actual);
        
        HRESULT ( STDMETHODCALLTYPE *ReplaceAll )( 
            __FIVector_1_DemoApp__CIItem * This,
            /* [in] */ unsigned int count,
            /* [size_is][in] */ __x_ABI_CDemoApp_CIItem **value);
        
        END_INTERFACE
    } __FIVector_1_DemoApp__CIItemVtbl;

    interface __FIVector_1_DemoApp__CIItem
    {
        CONST_VTBL struct __FIVector_1_DemoApp__CIItemVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define __FIVector_1_DemoApp__CIItem_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIVector_1_DemoApp__CIItem_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIVector_1_DemoApp__CIItem_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIVector_1_DemoApp__CIItem_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIVector_1_DemoApp__CIItem_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIVector_1_DemoApp__CIItem_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIVector_1_DemoApp__CIItem_GetAt(This,index,item)	\
    ( (This)->lpVtbl -> GetAt(This,index,item) ) 

#define __FIVector_1_DemoApp__CIItem_get_Size(This,size)	\
    ( (This)->lpVtbl -> get_Size(This,size) ) 

#define __FIVector_1_DemoApp__CIItem_GetView(This,view)	\
    ( (This)->lpVtbl -> GetView(This,view) ) 

#define __FIVector_1_DemoApp__CIItem_IndexOf(This,item,index,found)	\
    ( (This)->lpVtbl -> IndexOf(This,item,index,found) ) 

#define __FIVector_1_DemoApp__CIItem_SetAt(This,index,item)	\
    ( (This)->lpVtbl -> SetAt(This,index,item) ) 

#define __FIVector_1_DemoApp__CIItem_InsertAt(This,index,item)	\
    ( (This)->lpVtbl -> InsertAt(This,index,item) ) 

#define __FIVector_1_DemoApp__CIItem_RemoveAt(This,index)	\
    ( (This)->lpVtbl -> RemoveAt(This,index) ) 

#define __FIVector_1_DemoApp__CIItem_Append(This,item)	\
    ( (This)->lpVtbl -> Append(This,item) ) 

#define __FIVector_1_DemoApp__CIItem_RemoveAtEnd(This)	\
    ( (This)->lpVtbl -> RemoveAtEnd(This) ) 

#define __FIVector_1_DemoApp__CIItem_Clear(This)	\
    ( (This)->lpVtbl -> Clear(This) ) 

#define __FIVector_1_DemoApp__CIItem_GetMany(This,startIndex,capacity,items,actual)	\
    ( (This)->lpVtbl -> GetMany(This,startIndex,capacity,items,actual) ) 

#define __FIVector_1_DemoApp__CIItem_ReplaceAll(This,count,value)	\
    ( (This)->lpVtbl -> ReplaceAll(This,count,value) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* ____FIVector_1_DemoApp__CIItem_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_DemoApp_0000_0008 */
/* [local] */ 

#endif /* pinterface */
#endif /* DEF___FIVector_1_DemoApp__CIItem */


extern RPC_IF_HANDLE __MIDL_itf_DemoApp_0000_0008_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_DemoApp_0000_0008_v0_0_s_ifspec;

/* interface __MIDL_itf_DemoApp2Eidl_0000_2373 */




extern RPC_IF_HANDLE __MIDL_itf_DemoApp2Eidl_0000_2373_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_DemoApp2Eidl_0000_2373_v0_0_s_ifspec;

/* interface __MIDL_itf_DemoApp_0000_0009 */
/* [local] */ 

#ifndef DEF___FIAsyncOperationCompletedHandler_1_DemoApp__CIItem
#define DEF___FIAsyncOperationCompletedHandler_1_DemoApp__CIItem
#if !defined(__cplusplus) || defined(RO_NO_TEMPLATE_NAME)



extern RPC_IF_HANDLE __MIDL_itf_DemoApp_0000_0009_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_DemoApp_0000_0009_v0_0_s_ifspec;

#ifndef ____FIAsyncOperationCompletedHandler_1_DemoApp__CIItem_INTERFACE_DEFINED__
#define ____FIAsyncOperationCompletedHandler_1_DemoApp__CIItem_INTERFACE_DEFINED__

/* interface __FIAsyncOperationCompletedHandler_1_DemoApp__CIItem */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID___FIAsyncOperationCompletedHandler_1_DemoApp__CIItem;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("eeb5e064-301f-5c77-97d6-b244de9eaed2")
    __FIAsyncOperationCompletedHandler_1_DemoApp__CIItem : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Invoke( 
            /* [in] */ __FIAsyncOperation_1_DemoApp__CIItem *asyncInfo,
            /* [in] */ AsyncStatus status) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct __FIAsyncOperationCompletedHandler_1_DemoApp__CIItemVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __FIAsyncOperationCompletedHandler_1_DemoApp__CIItem * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __FIAsyncOperationCompletedHandler_1_DemoApp__CIItem * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __FIAsyncOperationCompletedHandler_1_DemoApp__CIItem * This);
        
        HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            __FIAsyncOperationCompletedHandler_1_DemoApp__CIItem * This,
            /* [in] */ __FIAsyncOperation_1_DemoApp__CIItem *asyncInfo,
            /* [in] */ AsyncStatus status);
        
        END_INTERFACE
    } __FIAsyncOperationCompletedHandler_1_DemoApp__CIItemVtbl;

    interface __FIAsyncOperationCompletedHandler_1_DemoApp__CIItem
    {
        CONST_VTBL struct __FIAsyncOperationCompletedHandler_1_DemoApp__CIItemVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define __FIAsyncOperationCompletedHandler_1_DemoApp__CIItem_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIAsyncOperationCompletedHandler_1_DemoApp__CIItem_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIAsyncOperationCompletedHandler_1_DemoApp__CIItem_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIAsyncOperationCompletedHandler_1_DemoApp__CIItem_Invoke(This,asyncInfo,status)	\
    ( (This)->lpVtbl -> Invoke(This,asyncInfo,status) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* ____FIAsyncOperationCompletedHandler_1_DemoApp__CIItem_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_DemoApp_0000_0010 */
/* [local] */ 

#endif /* pinterface */
#endif /* DEF___FIAsyncOperationCompletedHandler_1_DemoApp__CIItem */


extern RPC_IF_HANDLE __MIDL_itf_DemoApp_0000_0010_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_DemoApp_0000_0010_v0_0_s_ifspec;

/* interface __MIDL_itf_DemoApp2Eidl_0000_2374 */




extern RPC_IF_HANDLE __MIDL_itf_DemoApp2Eidl_0000_2374_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_DemoApp2Eidl_0000_2374_v0_0_s_ifspec;

/* interface __MIDL_itf_DemoApp_0000_0011 */
/* [local] */ 

#ifndef DEF___FIAsyncOperation_1_DemoApp__CIItem
#define DEF___FIAsyncOperation_1_DemoApp__CIItem
#if !defined(__cplusplus) || defined(RO_NO_TEMPLATE_NAME)


extern RPC_IF_HANDLE __MIDL_itf_DemoApp_0000_0011_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_DemoApp_0000_0011_v0_0_s_ifspec;

#ifndef ____FIAsyncOperation_1_DemoApp__CIItem_INTERFACE_DEFINED__
#define ____FIAsyncOperation_1_DemoApp__CIItem_INTERFACE_DEFINED__

/* interface __FIAsyncOperation_1_DemoApp__CIItem */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID___FIAsyncOperation_1_DemoApp__CIItem;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("88580592-12cc-5b76-8115-5a852ab97721")
    __FIAsyncOperation_1_DemoApp__CIItem : public IInspectable
    {
    public:
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_Completed( 
            /* [in] */ __FIAsyncOperationCompletedHandler_1_DemoApp__CIItem *handler) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Completed( 
            /* [retval][out] */ __FIAsyncOperationCompletedHandler_1_DemoApp__CIItem **handler) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetResults( 
            /* [retval][out] */ __x_ABI_CDemoApp_CIItem **results) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct __FIAsyncOperation_1_DemoApp__CIItemVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __FIAsyncOperation_1_DemoApp__CIItem * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __FIAsyncOperation_1_DemoApp__CIItem * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __FIAsyncOperation_1_DemoApp__CIItem * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetIids )( 
            __FIAsyncOperation_1_DemoApp__CIItem * This,
            /* [out] */ ULONG *iidCount,
            /* [size_is][size_is][out] */ IID **iids);
        
        HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )( 
            __FIAsyncOperation_1_DemoApp__CIItem * This,
            /* [out] */ HSTRING *className);
        
        HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )( 
            __FIAsyncOperation_1_DemoApp__CIItem * This,
            /* [out] */ TrustLevel *trustLevel);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_Completed )( 
            __FIAsyncOperation_1_DemoApp__CIItem * This,
            /* [in] */ __FIAsyncOperationCompletedHandler_1_DemoApp__CIItem *handler);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Completed )( 
            __FIAsyncOperation_1_DemoApp__CIItem * This,
            /* [retval][out] */ __FIAsyncOperationCompletedHandler_1_DemoApp__CIItem **handler);
        
        HRESULT ( STDMETHODCALLTYPE *GetResults )( 
            __FIAsyncOperation_1_DemoApp__CIItem * This,
            /* [retval][out] */ __x_ABI_CDemoApp_CIItem **results);
        
        END_INTERFACE
    } __FIAsyncOperation_1_DemoApp__CIItemVtbl;

    interface __FIAsyncOperation_1_DemoApp__CIItem
    {
        CONST_VTBL struct __FIAsyncOperation_1_DemoApp__CIItemVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define __FIAsyncOperation_1_DemoApp__CIItem_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIAsyncOperation_1_DemoApp__CIItem_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIAsyncOperation_1_DemoApp__CIItem_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIAsyncOperation_1_DemoApp__CIItem_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIAsyncOperation_1_DemoApp__CIItem_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIAsyncOperation_1_DemoApp__CIItem_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIAsyncOperation_1_DemoApp__CIItem_put_Completed(This,handler)	\
    ( (This)->lpVtbl -> put_Completed(This,handler) ) 

#define __FIAsyncOperation_1_DemoApp__CIItem_get_Completed(This,handler)	\
    ( (This)->lpVtbl -> get_Completed(This,handler) ) 

#define __FIAsyncOperation_1_DemoApp__CIItem_GetResults(This,results)	\
    ( (This)->lpVtbl -> GetResults(This,results) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* ____FIAsyncOperation_1_DemoApp__CIItem_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_DemoApp_0000_0012 */
/* [local] */ 

#endif /* pinterface */
#endif /* DEF___FIAsyncOperation_1_DemoApp__CIItem */


extern RPC_IF_HANDLE __MIDL_itf_DemoApp_0000_0012_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_DemoApp_0000_0012_v0_0_s_ifspec;

#ifndef ____x_ABI_CDemoApp_CIItem_INTERFACE_DEFINED__
#define ____x_ABI_CDemoApp_CIItem_INTERFACE_DEFINED__

/* interface __x_ABI_CDemoApp_CIItem */
/* [uuid][object] */ 


EXTERN_C const IID IID___x_ABI_CDemoApp_CIItem;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("dcf68566-c166-3085-7af5-c1d10493a6a8")
    __x_ABI_CDemoApp_CIItem : public IInspectable
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [out][retval] */ HSTRING *pValue) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_Name( 
            /* [in] */ HSTRING value) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_ChangeName( 
            /* [out][retval] */ __x_ABI_CDemoApp_CICommand__string__void **pValue) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct __x_ABI_CDemoApp_CIItemVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __x_ABI_CDemoApp_CIItem * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __x_ABI_CDemoApp_CIItem * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __x_ABI_CDemoApp_CIItem * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetIids )( 
            __x_ABI_CDemoApp_CIItem * This,
            /* [out] */ ULONG *iidCount,
            /* [size_is][size_is][out] */ IID **iids);
        
        HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )( 
            __x_ABI_CDemoApp_CIItem * This,
            /* [out] */ HSTRING *className);
        
        HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )( 
            __x_ABI_CDemoApp_CIItem * This,
            /* [out] */ TrustLevel *trustLevel);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Name )( 
            __x_ABI_CDemoApp_CIItem * This,
            /* [out][retval] */ HSTRING *pValue);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_Name )( 
            __x_ABI_CDemoApp_CIItem * This,
            /* [in] */ HSTRING value);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_ChangeName )( 
            __x_ABI_CDemoApp_CIItem * This,
            /* [out][retval] */ __x_ABI_CDemoApp_CICommand__string__void **pValue);
        
        END_INTERFACE
    } __x_ABI_CDemoApp_CIItemVtbl;

    interface __x_ABI_CDemoApp_CIItem
    {
        CONST_VTBL struct __x_ABI_CDemoApp_CIItemVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define __x_ABI_CDemoApp_CIItem_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __x_ABI_CDemoApp_CIItem_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __x_ABI_CDemoApp_CIItem_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __x_ABI_CDemoApp_CIItem_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __x_ABI_CDemoApp_CIItem_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __x_ABI_CDemoApp_CIItem_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __x_ABI_CDemoApp_CIItem_get_Name(This,pValue)	\
    ( (This)->lpVtbl -> get_Name(This,pValue) ) 

#define __x_ABI_CDemoApp_CIItem_put_Name(This,value)	\
    ( (This)->lpVtbl -> put_Name(This,value) ) 

#define __x_ABI_CDemoApp_CIItem_get_ChangeName(This,pValue)	\
    ( (This)->lpVtbl -> get_ChangeName(This,pValue) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* ____x_ABI_CDemoApp_CIItem_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_DemoApp_0000_0013 */
/* [local] */ 

/* [v1_enum] */ 
enum __x_ABI_CDemoApp_CItemProperties
    {
        ItemProperties_Name	= 1,
        ItemProperties_ChangeName	= 2
    } ;
typedef enum __x_ABI_CDemoApp_CItemProperties __x_ABI_CDemoApp_CItemProperties;



extern RPC_IF_HANDLE __MIDL_itf_DemoApp_0000_0013_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_DemoApp_0000_0013_v0_0_s_ifspec;

#ifndef ____x_ABI_CDemoApp_CIAppDemo_INTERFACE_DEFINED__
#define ____x_ABI_CDemoApp_CIAppDemo_INTERFACE_DEFINED__

/* interface __x_ABI_CDemoApp_CIAppDemo */
/* [uuid][object] */ 


EXTERN_C const IID IID___x_ABI_CDemoApp_CIAppDemo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("23de167b-4676-a2be-51bd-2eae8d6d5ca3")
    __x_ABI_CDemoApp_CIAppDemo : public IInspectable
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Items( 
            /* [out][retval] */ __FIVector_1_DemoApp__CIItem **pValue) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_AddItem( 
            /* [out][retval] */ __x_ABI_CDemoApp_CICommand__string__void__canExecute **pValue) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_AddNextItem( 
            /* [out][retval] */ __x_ABI_CMicrosoft_CPropertyModel_CISimpleCommand **pValue) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_RemoveItem( 
            /* [out][retval] */ __x_ABI_CDemoApp_CICommand__IItem__void__canExecute **pValue) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_ClearItems( 
            /* [out][retval] */ __x_ABI_CMicrosoft_CPropertyModel_CISimpleCommand **pValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateItemAsync( 
            /* [in] */ HSTRING name,
            /* [out][retval] */ __FIAsyncOperation_1_DemoApp__CIItem **result) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct __x_ABI_CDemoApp_CIAppDemoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __x_ABI_CDemoApp_CIAppDemo * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __x_ABI_CDemoApp_CIAppDemo * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __x_ABI_CDemoApp_CIAppDemo * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetIids )( 
            __x_ABI_CDemoApp_CIAppDemo * This,
            /* [out] */ ULONG *iidCount,
            /* [size_is][size_is][out] */ IID **iids);
        
        HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )( 
            __x_ABI_CDemoApp_CIAppDemo * This,
            /* [out] */ HSTRING *className);
        
        HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )( 
            __x_ABI_CDemoApp_CIAppDemo * This,
            /* [out] */ TrustLevel *trustLevel);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Items )( 
            __x_ABI_CDemoApp_CIAppDemo * This,
            /* [out][retval] */ __FIVector_1_DemoApp__CIItem **pValue);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_AddItem )( 
            __x_ABI_CDemoApp_CIAppDemo * This,
            /* [out][retval] */ __x_ABI_CDemoApp_CICommand__string__void__canExecute **pValue);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_AddNextItem )( 
            __x_ABI_CDemoApp_CIAppDemo * This,
            /* [out][retval] */ __x_ABI_CMicrosoft_CPropertyModel_CISimpleCommand **pValue);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_RemoveItem )( 
            __x_ABI_CDemoApp_CIAppDemo * This,
            /* [out][retval] */ __x_ABI_CDemoApp_CICommand__IItem__void__canExecute **pValue);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_ClearItems )( 
            __x_ABI_CDemoApp_CIAppDemo * This,
            /* [out][retval] */ __x_ABI_CMicrosoft_CPropertyModel_CISimpleCommand **pValue);
        
        HRESULT ( STDMETHODCALLTYPE *CreateItemAsync )( 
            __x_ABI_CDemoApp_CIAppDemo * This,
            /* [in] */ HSTRING name,
            /* [out][retval] */ __FIAsyncOperation_1_DemoApp__CIItem **result);
        
        END_INTERFACE
    } __x_ABI_CDemoApp_CIAppDemoVtbl;

    interface __x_ABI_CDemoApp_CIAppDemo
    {
        CONST_VTBL struct __x_ABI_CDemoApp_CIAppDemoVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define __x_ABI_CDemoApp_CIAppDemo_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __x_ABI_CDemoApp_CIAppDemo_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __x_ABI_CDemoApp_CIAppDemo_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __x_ABI_CDemoApp_CIAppDemo_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __x_ABI_CDemoApp_CIAppDemo_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __x_ABI_CDemoApp_CIAppDemo_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __x_ABI_CDemoApp_CIAppDemo_get_Items(This,pValue)	\
    ( (This)->lpVtbl -> get_Items(This,pValue) ) 

#define __x_ABI_CDemoApp_CIAppDemo_get_AddItem(This,pValue)	\
    ( (This)->lpVtbl -> get_AddItem(This,pValue) ) 

#define __x_ABI_CDemoApp_CIAppDemo_get_AddNextItem(This,pValue)	\
    ( (This)->lpVtbl -> get_AddNextItem(This,pValue) ) 

#define __x_ABI_CDemoApp_CIAppDemo_get_RemoveItem(This,pValue)	\
    ( (This)->lpVtbl -> get_RemoveItem(This,pValue) ) 

#define __x_ABI_CDemoApp_CIAppDemo_get_ClearItems(This,pValue)	\
    ( (This)->lpVtbl -> get_ClearItems(This,pValue) ) 

#define __x_ABI_CDemoApp_CIAppDemo_CreateItemAsync(This,name,result)	\
    ( (This)->lpVtbl -> CreateItemAsync(This,name,result) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* ____x_ABI_CDemoApp_CIAppDemo_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_DemoApp_0000_0014 */
/* [local] */ 

/* [v1_enum] */ 
enum __x_ABI_CDemoApp_CAppDemoProperties
    {
        AppDemoProperties_Items	= 1,
        AppDemoProperties_AddItem	= 2,
        AppDemoProperties_AddNextItem	= 3,
        AppDemoProperties_RemoveItem	= 4,
        AppDemoProperties_ClearItems	= 5
    } ;
typedef enum __x_ABI_CDemoApp_CAppDemoProperties __x_ABI_CDemoApp_CAppDemoProperties;

/* [v1_enum] */ 
enum __x_ABI_CDemoApp_CAppDemoMethods
    {
        AppDemoMethods_CreateItemAsync	= 1
    } ;
typedef enum __x_ABI_CDemoApp_CAppDemoMethods __x_ABI_CDemoApp_CAppDemoMethods;



extern RPC_IF_HANDLE __MIDL_itf_DemoApp_0000_0014_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_DemoApp_0000_0014_v0_0_s_ifspec;

#ifndef ____x_ABI_CDemoApp_CIAppDemoFactory_INTERFACE_DEFINED__
#define ____x_ABI_CDemoApp_CIAppDemoFactory_INTERFACE_DEFINED__

/* interface __x_ABI_CDemoApp_CIAppDemoFactory */
/* [uuid][object] */ 


EXTERN_C const IID IID___x_ABI_CDemoApp_CIAppDemoFactory;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0d8e7447-4c0a-3020-8348-69f6a4295941")
    __x_ABI_CDemoApp_CIAppDemoFactory : public IInspectable
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE CreateAppDemo( 
            /* [in] */ IInspectable *workingDisptacher,
            /* [out][retval] */ __x_ABI_CDemoApp_CIAppDemo **result) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct __x_ABI_CDemoApp_CIAppDemoFactoryVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __x_ABI_CDemoApp_CIAppDemoFactory * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __x_ABI_CDemoApp_CIAppDemoFactory * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __x_ABI_CDemoApp_CIAppDemoFactory * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetIids )( 
            __x_ABI_CDemoApp_CIAppDemoFactory * This,
            /* [out] */ ULONG *iidCount,
            /* [size_is][size_is][out] */ IID **iids);
        
        HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )( 
            __x_ABI_CDemoApp_CIAppDemoFactory * This,
            /* [out] */ HSTRING *className);
        
        HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )( 
            __x_ABI_CDemoApp_CIAppDemoFactory * This,
            /* [out] */ TrustLevel *trustLevel);
        
        HRESULT ( STDMETHODCALLTYPE *CreateAppDemo )( 
            __x_ABI_CDemoApp_CIAppDemoFactory * This,
            /* [in] */ IInspectable *workingDisptacher,
            /* [out][retval] */ __x_ABI_CDemoApp_CIAppDemo **result);
        
        END_INTERFACE
    } __x_ABI_CDemoApp_CIAppDemoFactoryVtbl;

    interface __x_ABI_CDemoApp_CIAppDemoFactory
    {
        CONST_VTBL struct __x_ABI_CDemoApp_CIAppDemoFactoryVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define __x_ABI_CDemoApp_CIAppDemoFactory_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __x_ABI_CDemoApp_CIAppDemoFactory_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __x_ABI_CDemoApp_CIAppDemoFactory_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __x_ABI_CDemoApp_CIAppDemoFactory_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __x_ABI_CDemoApp_CIAppDemoFactory_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __x_ABI_CDemoApp_CIAppDemoFactory_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __x_ABI_CDemoApp_CIAppDemoFactory_CreateAppDemo(This,workingDisptacher,result)	\
    ( (This)->lpVtbl -> CreateAppDemo(This,workingDisptacher,result) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* ____x_ABI_CDemoApp_CIAppDemoFactory_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_DemoApp_0000_0015 */
/* [local] */ 

/* [v1_enum] */ 
enum __x_ABI_CDemoApp_CAppDemoFactoryMethods
    {
        AppDemoFactoryMethods_CreateAppDemo	= 1
    } ;
typedef enum __x_ABI_CDemoApp_CAppDemoFactoryMethods __x_ABI_CDemoApp_CAppDemoFactoryMethods;



extern RPC_IF_HANDLE __MIDL_itf_DemoApp_0000_0015_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_DemoApp_0000_0015_v0_0_s_ifspec;

#ifndef ____x_ABI_CDemoApp_CICommand__string__void__canExecute_INTERFACE_DEFINED__
#define ____x_ABI_CDemoApp_CICommand__string__void__canExecute_INTERFACE_DEFINED__

/* interface __x_ABI_CDemoApp_CICommand__string__void__canExecute */
/* [uuid][object] */ 


EXTERN_C const IID IID___x_ABI_CDemoApp_CICommand__string__void__canExecute;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("af646b57-9b24-78f0-5606-6e0c425140d9")
    __x_ABI_CDemoApp_CICommand__string__void__canExecute : public IInspectable
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Execute( 
            /* [in] */ HSTRING parameter) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CanExecute( 
            /* [in] */ HSTRING parameter,
            /* [out][retval] */ boolean *value) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct __x_ABI_CDemoApp_CICommand__string__void__canExecuteVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __x_ABI_CDemoApp_CICommand__string__void__canExecute * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __x_ABI_CDemoApp_CICommand__string__void__canExecute * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __x_ABI_CDemoApp_CICommand__string__void__canExecute * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetIids )( 
            __x_ABI_CDemoApp_CICommand__string__void__canExecute * This,
            /* [out] */ ULONG *iidCount,
            /* [size_is][size_is][out] */ IID **iids);
        
        HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )( 
            __x_ABI_CDemoApp_CICommand__string__void__canExecute * This,
            /* [out] */ HSTRING *className);
        
        HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )( 
            __x_ABI_CDemoApp_CICommand__string__void__canExecute * This,
            /* [out] */ TrustLevel *trustLevel);
        
        HRESULT ( STDMETHODCALLTYPE *Execute )( 
            __x_ABI_CDemoApp_CICommand__string__void__canExecute * This,
            /* [in] */ HSTRING parameter);
        
        HRESULT ( STDMETHODCALLTYPE *CanExecute )( 
            __x_ABI_CDemoApp_CICommand__string__void__canExecute * This,
            /* [in] */ HSTRING parameter,
            /* [out][retval] */ boolean *value);
        
        END_INTERFACE
    } __x_ABI_CDemoApp_CICommand__string__void__canExecuteVtbl;

    interface __x_ABI_CDemoApp_CICommand__string__void__canExecute
    {
        CONST_VTBL struct __x_ABI_CDemoApp_CICommand__string__void__canExecuteVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define __x_ABI_CDemoApp_CICommand__string__void__canExecute_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __x_ABI_CDemoApp_CICommand__string__void__canExecute_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __x_ABI_CDemoApp_CICommand__string__void__canExecute_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __x_ABI_CDemoApp_CICommand__string__void__canExecute_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __x_ABI_CDemoApp_CICommand__string__void__canExecute_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __x_ABI_CDemoApp_CICommand__string__void__canExecute_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __x_ABI_CDemoApp_CICommand__string__void__canExecute_Execute(This,parameter)	\
    ( (This)->lpVtbl -> Execute(This,parameter) ) 

#define __x_ABI_CDemoApp_CICommand__string__void__canExecute_CanExecute(This,parameter,value)	\
    ( (This)->lpVtbl -> CanExecute(This,parameter,value) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* ____x_ABI_CDemoApp_CICommand__string__void__canExecute_INTERFACE_DEFINED__ */


#ifndef ____x_ABI_CDemoApp_CICommand__IItem__void__canExecute_INTERFACE_DEFINED__
#define ____x_ABI_CDemoApp_CICommand__IItem__void__canExecute_INTERFACE_DEFINED__

/* interface __x_ABI_CDemoApp_CICommand__IItem__void__canExecute */
/* [uuid][object] */ 


EXTERN_C const IID IID___x_ABI_CDemoApp_CICommand__IItem__void__canExecute;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2329a181-d915-c0f0-f335-ec81ca649a28")
    __x_ABI_CDemoApp_CICommand__IItem__void__canExecute : public IInspectable
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Execute( 
            /* [in] */ __x_ABI_CDemoApp_CIItem *parameter) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CanExecute( 
            /* [in] */ __x_ABI_CDemoApp_CIItem *parameter,
            /* [out][retval] */ boolean *value) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct __x_ABI_CDemoApp_CICommand__IItem__void__canExecuteVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __x_ABI_CDemoApp_CICommand__IItem__void__canExecute * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __x_ABI_CDemoApp_CICommand__IItem__void__canExecute * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __x_ABI_CDemoApp_CICommand__IItem__void__canExecute * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetIids )( 
            __x_ABI_CDemoApp_CICommand__IItem__void__canExecute * This,
            /* [out] */ ULONG *iidCount,
            /* [size_is][size_is][out] */ IID **iids);
        
        HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )( 
            __x_ABI_CDemoApp_CICommand__IItem__void__canExecute * This,
            /* [out] */ HSTRING *className);
        
        HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )( 
            __x_ABI_CDemoApp_CICommand__IItem__void__canExecute * This,
            /* [out] */ TrustLevel *trustLevel);
        
        HRESULT ( STDMETHODCALLTYPE *Execute )( 
            __x_ABI_CDemoApp_CICommand__IItem__void__canExecute * This,
            /* [in] */ __x_ABI_CDemoApp_CIItem *parameter);
        
        HRESULT ( STDMETHODCALLTYPE *CanExecute )( 
            __x_ABI_CDemoApp_CICommand__IItem__void__canExecute * This,
            /* [in] */ __x_ABI_CDemoApp_CIItem *parameter,
            /* [out][retval] */ boolean *value);
        
        END_INTERFACE
    } __x_ABI_CDemoApp_CICommand__IItem__void__canExecuteVtbl;

    interface __x_ABI_CDemoApp_CICommand__IItem__void__canExecute
    {
        CONST_VTBL struct __x_ABI_CDemoApp_CICommand__IItem__void__canExecuteVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define __x_ABI_CDemoApp_CICommand__IItem__void__canExecute_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __x_ABI_CDemoApp_CICommand__IItem__void__canExecute_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __x_ABI_CDemoApp_CICommand__IItem__void__canExecute_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __x_ABI_CDemoApp_CICommand__IItem__void__canExecute_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __x_ABI_CDemoApp_CICommand__IItem__void__canExecute_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __x_ABI_CDemoApp_CICommand__IItem__void__canExecute_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __x_ABI_CDemoApp_CICommand__IItem__void__canExecute_Execute(This,parameter)	\
    ( (This)->lpVtbl -> Execute(This,parameter) ) 

#define __x_ABI_CDemoApp_CICommand__IItem__void__canExecute_CanExecute(This,parameter,value)	\
    ( (This)->lpVtbl -> CanExecute(This,parameter,value) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* ____x_ABI_CDemoApp_CICommand__IItem__void__canExecute_INTERFACE_DEFINED__ */


#ifndef ____x_ABI_CDemoApp_CICommand__string__void_INTERFACE_DEFINED__
#define ____x_ABI_CDemoApp_CICommand__string__void_INTERFACE_DEFINED__

/* interface __x_ABI_CDemoApp_CICommand__string__void */
/* [uuid][object] */ 


EXTERN_C const IID IID___x_ABI_CDemoApp_CICommand__string__void;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("05257b2c-3512-59ec-0138-ccb7ce461740")
    __x_ABI_CDemoApp_CICommand__string__void : public IInspectable
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Execute( 
            /* [in] */ HSTRING parameter) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CanExecute( 
            /* [in] */ HSTRING parameter,
            /* [out][retval] */ boolean *value) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct __x_ABI_CDemoApp_CICommand__string__voidVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __x_ABI_CDemoApp_CICommand__string__void * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __x_ABI_CDemoApp_CICommand__string__void * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __x_ABI_CDemoApp_CICommand__string__void * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetIids )( 
            __x_ABI_CDemoApp_CICommand__string__void * This,
            /* [out] */ ULONG *iidCount,
            /* [size_is][size_is][out] */ IID **iids);
        
        HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )( 
            __x_ABI_CDemoApp_CICommand__string__void * This,
            /* [out] */ HSTRING *className);
        
        HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )( 
            __x_ABI_CDemoApp_CICommand__string__void * This,
            /* [out] */ TrustLevel *trustLevel);
        
        HRESULT ( STDMETHODCALLTYPE *Execute )( 
            __x_ABI_CDemoApp_CICommand__string__void * This,
            /* [in] */ HSTRING parameter);
        
        HRESULT ( STDMETHODCALLTYPE *CanExecute )( 
            __x_ABI_CDemoApp_CICommand__string__void * This,
            /* [in] */ HSTRING parameter,
            /* [out][retval] */ boolean *value);
        
        END_INTERFACE
    } __x_ABI_CDemoApp_CICommand__string__voidVtbl;

    interface __x_ABI_CDemoApp_CICommand__string__void
    {
        CONST_VTBL struct __x_ABI_CDemoApp_CICommand__string__voidVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define __x_ABI_CDemoApp_CICommand__string__void_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __x_ABI_CDemoApp_CICommand__string__void_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __x_ABI_CDemoApp_CICommand__string__void_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __x_ABI_CDemoApp_CICommand__string__void_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __x_ABI_CDemoApp_CICommand__string__void_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __x_ABI_CDemoApp_CICommand__string__void_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __x_ABI_CDemoApp_CICommand__string__void_Execute(This,parameter)	\
    ( (This)->lpVtbl -> Execute(This,parameter) ) 

#define __x_ABI_CDemoApp_CICommand__string__void_CanExecute(This,parameter,value)	\
    ( (This)->lpVtbl -> CanExecute(This,parameter,value) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* ____x_ABI_CDemoApp_CICommand__string__void_INTERFACE_DEFINED__ */


#ifndef ____x_ABI_CDemoApp_CIClassStatics_INTERFACE_DEFINED__
#define ____x_ABI_CDemoApp_CIClassStatics_INTERFACE_DEFINED__

/* interface __x_ABI_CDemoApp_CIClassStatics */
/* [uuid][object] */ 


EXTERN_C const IID IID___x_ABI_CDemoApp_CIClassStatics;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("193B1CAA-C3B7-47DE-9F84-67412A7EF1D8")
    __x_ABI_CDemoApp_CIClassStatics : public IInspectable
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Register( void) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct __x_ABI_CDemoApp_CIClassStaticsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __x_ABI_CDemoApp_CIClassStatics * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __x_ABI_CDemoApp_CIClassStatics * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __x_ABI_CDemoApp_CIClassStatics * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetIids )( 
            __x_ABI_CDemoApp_CIClassStatics * This,
            /* [out] */ ULONG *iidCount,
            /* [size_is][size_is][out] */ IID **iids);
        
        HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )( 
            __x_ABI_CDemoApp_CIClassStatics * This,
            /* [out] */ HSTRING *className);
        
        HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )( 
            __x_ABI_CDemoApp_CIClassStatics * This,
            /* [out] */ TrustLevel *trustLevel);
        
        HRESULT ( STDMETHODCALLTYPE *Register )( 
            __x_ABI_CDemoApp_CIClassStatics * This);
        
        END_INTERFACE
    } __x_ABI_CDemoApp_CIClassStaticsVtbl;

    interface __x_ABI_CDemoApp_CIClassStatics
    {
        CONST_VTBL struct __x_ABI_CDemoApp_CIClassStaticsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define __x_ABI_CDemoApp_CIClassStatics_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __x_ABI_CDemoApp_CIClassStatics_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __x_ABI_CDemoApp_CIClassStatics_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __x_ABI_CDemoApp_CIClassStatics_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __x_ABI_CDemoApp_CIClassStatics_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __x_ABI_CDemoApp_CIClassStatics_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __x_ABI_CDemoApp_CIClassStatics_Register(This)	\
    ( (This)->lpVtbl -> Register(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* ____x_ABI_CDemoApp_CIClassStatics_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  HSTRING_UserSize(     unsigned long *, unsigned long            , HSTRING * ); 
unsigned char * __RPC_USER  HSTRING_UserMarshal(  unsigned long *, unsigned char *, HSTRING * ); 
unsigned char * __RPC_USER  HSTRING_UserUnmarshal(unsigned long *, unsigned char *, HSTRING * ); 
void                      __RPC_USER  HSTRING_UserFree(     unsigned long *, HSTRING * ); 

unsigned long             __RPC_USER  HSTRING_UserSize64(     unsigned long *, unsigned long            , HSTRING * ); 
unsigned char * __RPC_USER  HSTRING_UserMarshal64(  unsigned long *, unsigned char *, HSTRING * ); 
unsigned char * __RPC_USER  HSTRING_UserUnmarshal64(unsigned long *, unsigned char *, HSTRING * ); 
void                      __RPC_USER  HSTRING_UserFree64(     unsigned long *, HSTRING * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


