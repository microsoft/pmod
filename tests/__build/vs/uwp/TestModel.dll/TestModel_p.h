

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.01.0622 */
/* at Mon Jan 18 19:14:07 2038
 */
/* Compiler settings for C:\Users\rodrigov\AppData\Local\Temp\TestModel.idl-a2d1c9a8:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 8.01.0622 
    protocol : all , ms_ext, c_ext, robust
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

#ifndef __TestModel_p_h__
#define __TestModel_p_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef ____FIIterator_1_TestModel__CITypeProperties_FWD_DEFINED__
#define ____FIIterator_1_TestModel__CITypeProperties_FWD_DEFINED__
typedef interface __FIIterator_1_TestModel__CITypeProperties __FIIterator_1_TestModel__CITypeProperties;

#endif 	/* ____FIIterator_1_TestModel__CITypeProperties_FWD_DEFINED__ */


#ifndef ____FIIterable_1_TestModel__CITypeProperties_FWD_DEFINED__
#define ____FIIterable_1_TestModel__CITypeProperties_FWD_DEFINED__
typedef interface __FIIterable_1_TestModel__CITypeProperties __FIIterable_1_TestModel__CITypeProperties;

#endif 	/* ____FIIterable_1_TestModel__CITypeProperties_FWD_DEFINED__ */


#ifndef ____FIVectorView_1_TestModel__CITypeProperties_FWD_DEFINED__
#define ____FIVectorView_1_TestModel__CITypeProperties_FWD_DEFINED__
typedef interface __FIVectorView_1_TestModel__CITypeProperties __FIVectorView_1_TestModel__CITypeProperties;

#endif 	/* ____FIVectorView_1_TestModel__CITypeProperties_FWD_DEFINED__ */


#ifndef ____FIVector_1_TestModel__CITypeProperties_FWD_DEFINED__
#define ____FIVector_1_TestModel__CITypeProperties_FWD_DEFINED__
typedef interface __FIVector_1_TestModel__CITypeProperties __FIVector_1_TestModel__CITypeProperties;

#endif 	/* ____FIVector_1_TestModel__CITypeProperties_FWD_DEFINED__ */


#ifndef ____FIReference_1_TestModel__CTypeEnum_FWD_DEFINED__
#define ____FIReference_1_TestModel__CTypeEnum_FWD_DEFINED__
typedef interface __FIReference_1_TestModel__CTypeEnum __FIReference_1_TestModel__CTypeEnum;

#endif 	/* ____FIReference_1_TestModel__CTypeEnum_FWD_DEFINED__ */


#ifndef ____FIReference_1_TestModel__CTypeEnum2_FWD_DEFINED__
#define ____FIReference_1_TestModel__CTypeEnum2_FWD_DEFINED__
typedef interface __FIReference_1_TestModel__CTypeEnum2 __FIReference_1_TestModel__CTypeEnum2;

#endif 	/* ____FIReference_1_TestModel__CTypeEnum2_FWD_DEFINED__ */


#ifndef ____FIReference_1_TestModel__CTypeFlags_FWD_DEFINED__
#define ____FIReference_1_TestModel__CTypeFlags_FWD_DEFINED__
typedef interface __FIReference_1_TestModel__CTypeFlags __FIReference_1_TestModel__CTypeFlags;

#endif 	/* ____FIReference_1_TestModel__CTypeFlags_FWD_DEFINED__ */


#ifndef ____FIReference_1_TestModel__CTypeNonFlags_FWD_DEFINED__
#define ____FIReference_1_TestModel__CTypeNonFlags_FWD_DEFINED__
typedef interface __FIReference_1_TestModel__CTypeNonFlags __FIReference_1_TestModel__CTypeNonFlags;

#endif 	/* ____FIReference_1_TestModel__CTypeNonFlags_FWD_DEFINED__ */


#ifndef ____x_ABI_CTestModel_CITypeProperties_FWD_DEFINED__
#define ____x_ABI_CTestModel_CITypeProperties_FWD_DEFINED__
typedef interface __x_ABI_CTestModel_CITypeProperties __x_ABI_CTestModel_CITypeProperties;

#endif 	/* ____x_ABI_CTestModel_CITypeProperties_FWD_DEFINED__ */


#ifndef ____x_ABI_CTestModel_CITypePropertiesTest_FWD_DEFINED__
#define ____x_ABI_CTestModel_CITypePropertiesTest_FWD_DEFINED__
typedef interface __x_ABI_CTestModel_CITypePropertiesTest __x_ABI_CTestModel_CITypePropertiesTest;

#endif 	/* ____x_ABI_CTestModel_CITypePropertiesTest_FWD_DEFINED__ */


#ifndef ____x_ABI_CTestModel_CICommandTypes_FWD_DEFINED__
#define ____x_ABI_CTestModel_CICommandTypes_FWD_DEFINED__
typedef interface __x_ABI_CTestModel_CICommandTypes __x_ABI_CTestModel_CICommandTypes;

#endif 	/* ____x_ABI_CTestModel_CICommandTypes_FWD_DEFINED__ */


#ifndef ____x_ABI_CTestModel_CIMethodBase_FWD_DEFINED__
#define ____x_ABI_CTestModel_CIMethodBase_FWD_DEFINED__
typedef interface __x_ABI_CTestModel_CIMethodBase __x_ABI_CTestModel_CIMethodBase;

#endif 	/* ____x_ABI_CTestModel_CIMethodBase_FWD_DEFINED__ */


#ifndef ____x_ABI_CTestModel_CIMethodDerived_FWD_DEFINED__
#define ____x_ABI_CTestModel_CIMethodDerived_FWD_DEFINED__
typedef interface __x_ABI_CTestModel_CIMethodDerived __x_ABI_CTestModel_CIMethodDerived;

#endif 	/* ____x_ABI_CTestModel_CIMethodDerived_FWD_DEFINED__ */


#ifndef ____x_ABI_CTestModel_CIMethodSupport_FWD_DEFINED__
#define ____x_ABI_CTestModel_CIMethodSupport_FWD_DEFINED__
typedef interface __x_ABI_CTestModel_CIMethodSupport __x_ABI_CTestModel_CIMethodSupport;

#endif 	/* ____x_ABI_CTestModel_CIMethodSupport_FWD_DEFINED__ */


#ifndef ____x_ABI_CTestModel_CITestFactory_FWD_DEFINED__
#define ____x_ABI_CTestModel_CITestFactory_FWD_DEFINED__
typedef interface __x_ABI_CTestModel_CITestFactory __x_ABI_CTestModel_CITestFactory;

#endif 	/* ____x_ABI_CTestModel_CITestFactory_FWD_DEFINED__ */


#ifndef ____x_ABI_CTestModel_CICommand__void__string_FWD_DEFINED__
#define ____x_ABI_CTestModel_CICommand__void__string_FWD_DEFINED__
typedef interface __x_ABI_CTestModel_CICommand__void__string __x_ABI_CTestModel_CICommand__void__string;

#endif 	/* ____x_ABI_CTestModel_CICommand__void__string_FWD_DEFINED__ */


#ifndef ____x_ABI_CTestModel_CICommand__void__string__async_FWD_DEFINED__
#define ____x_ABI_CTestModel_CICommand__void__string__async_FWD_DEFINED__
typedef interface __x_ABI_CTestModel_CICommand__void__string__async __x_ABI_CTestModel_CICommand__void__string__async;

#endif 	/* ____x_ABI_CTestModel_CICommand__void__string__async_FWD_DEFINED__ */


#ifndef ____x_ABI_CTestModel_CIClassStatics_FWD_DEFINED__
#define ____x_ABI_CTestModel_CIClassStatics_FWD_DEFINED__
typedef interface __x_ABI_CTestModel_CIClassStatics __x_ABI_CTestModel_CIClassStatics;

#endif 	/* ____x_ABI_CTestModel_CIClassStatics_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "inspectable.h"
#include "eventtoken.h"
#include "windows.foundation.h"
#include "Microsoft.PropertyModel.h"

#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_TestModel_0000_0000 */
/* [local] */ 






































#pragma warning (push)
#pragma warning (disable:4668) 
#pragma warning (disable:4001) 
#pragma once 
#pragma warning (pop)








/* [v1_enum] */ 
enum __x_ABI_CTestModel_CTypeEnum
    {
        TypeEnum_Type1	= 0x1,
        TypeEnum_Type2	= 0x2,
        TypeEnum_Type3	= 0x3
    } ;
typedef enum __x_ABI_CTestModel_CTypeEnum __x_ABI_CTestModel_CTypeEnum;

/* [v1_enum] */ 
enum __x_ABI_CTestModel_CTypeEnum2
    {
        TypeEnum2_Type4	= 0x4
    } ;
typedef enum __x_ABI_CTestModel_CTypeEnum2 __x_ABI_CTestModel_CTypeEnum2;

/* [v1_enum] */ 
enum __x_ABI_CTestModel_CTypeFlags
    {
        TypeFlags_None	= 0,
        TypeFlags_Value1	= 0x1,
        TypeFlags_Value2	= 0x2,
        TypeFlags_Value3	= 0x4
    } ;
typedef enum __x_ABI_CTestModel_CTypeFlags __x_ABI_CTestModel_CTypeFlags;

/* [v1_enum] */ 
enum __x_ABI_CTestModel_CTypeNonFlags
    {
        TypeNonFlags_Value1	= 0x1,
        TypeNonFlags_Value2	= 0x2,
        TypeNonFlags_Value3	= 0x3
    } ;
typedef enum __x_ABI_CTestModel_CTypeNonFlags __x_ABI_CTestModel_CTypeNonFlags;



extern RPC_IF_HANDLE __MIDL_itf_TestModel_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_TestModel_0000_0000_v0_0_s_ifspec;

/* interface __MIDL_itf_TestModel2Eidl_0000_2396 */




extern RPC_IF_HANDLE __MIDL_itf_TestModel2Eidl_0000_2396_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_TestModel2Eidl_0000_2396_v0_0_s_ifspec;

/* interface __MIDL_itf_TestModel_0000_0001 */
/* [local] */ 

#ifndef DEF___FIIterator_1_TestModel__CITypeProperties
#define DEF___FIIterator_1_TestModel__CITypeProperties
#if !defined(__cplusplus) || defined(RO_NO_TEMPLATE_NAME)


extern RPC_IF_HANDLE __MIDL_itf_TestModel_0000_0001_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_TestModel_0000_0001_v0_0_s_ifspec;

#ifndef ____FIIterator_1_TestModel__CITypeProperties_INTERFACE_DEFINED__
#define ____FIIterator_1_TestModel__CITypeProperties_INTERFACE_DEFINED__

/* interface __FIIterator_1_TestModel__CITypeProperties */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID___FIIterator_1_TestModel__CITypeProperties;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("5c5a2019-d846-540c-b4c3-543ff3c01755")
    __FIIterator_1_TestModel__CITypeProperties : public IInspectable
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Current( 
            /* [retval][out] */ __x_ABI_CTestModel_CITypeProperties **current) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_HasCurrent( 
            /* [retval][out] */ boolean *hasCurrent) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE MoveNext( 
            /* [retval][out] */ boolean *hasCurrent) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMany( 
            /* [in] */ unsigned int capacity,
            /* [size_is][length_is][out] */ __x_ABI_CTestModel_CITypeProperties **items,
            /* [retval][out] */ unsigned int *actual) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct __FIIterator_1_TestModel__CITypePropertiesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __FIIterator_1_TestModel__CITypeProperties * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __FIIterator_1_TestModel__CITypeProperties * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __FIIterator_1_TestModel__CITypeProperties * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetIids )( 
            __FIIterator_1_TestModel__CITypeProperties * This,
            /* [out] */ ULONG *iidCount,
            /* [size_is][size_is][out] */ IID **iids);
        
        HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )( 
            __FIIterator_1_TestModel__CITypeProperties * This,
            /* [out] */ HSTRING *className);
        
        HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )( 
            __FIIterator_1_TestModel__CITypeProperties * This,
            /* [out] */ TrustLevel *trustLevel);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Current )( 
            __FIIterator_1_TestModel__CITypeProperties * This,
            /* [retval][out] */ __x_ABI_CTestModel_CITypeProperties **current);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_HasCurrent )( 
            __FIIterator_1_TestModel__CITypeProperties * This,
            /* [retval][out] */ boolean *hasCurrent);
        
        HRESULT ( STDMETHODCALLTYPE *MoveNext )( 
            __FIIterator_1_TestModel__CITypeProperties * This,
            /* [retval][out] */ boolean *hasCurrent);
        
        HRESULT ( STDMETHODCALLTYPE *GetMany )( 
            __FIIterator_1_TestModel__CITypeProperties * This,
            /* [in] */ unsigned int capacity,
            /* [size_is][length_is][out] */ __x_ABI_CTestModel_CITypeProperties **items,
            /* [retval][out] */ unsigned int *actual);
        
        END_INTERFACE
    } __FIIterator_1_TestModel__CITypePropertiesVtbl;

    interface __FIIterator_1_TestModel__CITypeProperties
    {
        CONST_VTBL struct __FIIterator_1_TestModel__CITypePropertiesVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define __FIIterator_1_TestModel__CITypeProperties_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIIterator_1_TestModel__CITypeProperties_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIIterator_1_TestModel__CITypeProperties_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIIterator_1_TestModel__CITypeProperties_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIIterator_1_TestModel__CITypeProperties_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIIterator_1_TestModel__CITypeProperties_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIIterator_1_TestModel__CITypeProperties_get_Current(This,current)	\
    ( (This)->lpVtbl -> get_Current(This,current) ) 

#define __FIIterator_1_TestModel__CITypeProperties_get_HasCurrent(This,hasCurrent)	\
    ( (This)->lpVtbl -> get_HasCurrent(This,hasCurrent) ) 

#define __FIIterator_1_TestModel__CITypeProperties_MoveNext(This,hasCurrent)	\
    ( (This)->lpVtbl -> MoveNext(This,hasCurrent) ) 

#define __FIIterator_1_TestModel__CITypeProperties_GetMany(This,capacity,items,actual)	\
    ( (This)->lpVtbl -> GetMany(This,capacity,items,actual) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* ____FIIterator_1_TestModel__CITypeProperties_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_TestModel_0000_0002 */
/* [local] */ 

#endif /* pinterface */
#endif /* DEF___FIIterator_1_TestModel__CITypeProperties */


extern RPC_IF_HANDLE __MIDL_itf_TestModel_0000_0002_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_TestModel_0000_0002_v0_0_s_ifspec;

/* interface __MIDL_itf_TestModel2Eidl_0000_2397 */




extern RPC_IF_HANDLE __MIDL_itf_TestModel2Eidl_0000_2397_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_TestModel2Eidl_0000_2397_v0_0_s_ifspec;

/* interface __MIDL_itf_TestModel_0000_0003 */
/* [local] */ 

#ifndef DEF___FIIterable_1_TestModel__CITypeProperties
#define DEF___FIIterable_1_TestModel__CITypeProperties
#if !defined(__cplusplus) || defined(RO_NO_TEMPLATE_NAME)


extern RPC_IF_HANDLE __MIDL_itf_TestModel_0000_0003_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_TestModel_0000_0003_v0_0_s_ifspec;

#ifndef ____FIIterable_1_TestModel__CITypeProperties_INTERFACE_DEFINED__
#define ____FIIterable_1_TestModel__CITypeProperties_INTERFACE_DEFINED__

/* interface __FIIterable_1_TestModel__CITypeProperties */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID___FIIterable_1_TestModel__CITypeProperties;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("956aac92-0281-5bfb-9577-96f415acd96e")
    __FIIterable_1_TestModel__CITypeProperties : public IInspectable
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE First( 
            /* [retval][out] */ __FIIterator_1_TestModel__CITypeProperties **first) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct __FIIterable_1_TestModel__CITypePropertiesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __FIIterable_1_TestModel__CITypeProperties * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __FIIterable_1_TestModel__CITypeProperties * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __FIIterable_1_TestModel__CITypeProperties * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetIids )( 
            __FIIterable_1_TestModel__CITypeProperties * This,
            /* [out] */ ULONG *iidCount,
            /* [size_is][size_is][out] */ IID **iids);
        
        HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )( 
            __FIIterable_1_TestModel__CITypeProperties * This,
            /* [out] */ HSTRING *className);
        
        HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )( 
            __FIIterable_1_TestModel__CITypeProperties * This,
            /* [out] */ TrustLevel *trustLevel);
        
        HRESULT ( STDMETHODCALLTYPE *First )( 
            __FIIterable_1_TestModel__CITypeProperties * This,
            /* [retval][out] */ __FIIterator_1_TestModel__CITypeProperties **first);
        
        END_INTERFACE
    } __FIIterable_1_TestModel__CITypePropertiesVtbl;

    interface __FIIterable_1_TestModel__CITypeProperties
    {
        CONST_VTBL struct __FIIterable_1_TestModel__CITypePropertiesVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define __FIIterable_1_TestModel__CITypeProperties_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIIterable_1_TestModel__CITypeProperties_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIIterable_1_TestModel__CITypeProperties_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIIterable_1_TestModel__CITypeProperties_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIIterable_1_TestModel__CITypeProperties_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIIterable_1_TestModel__CITypeProperties_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIIterable_1_TestModel__CITypeProperties_First(This,first)	\
    ( (This)->lpVtbl -> First(This,first) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* ____FIIterable_1_TestModel__CITypeProperties_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_TestModel_0000_0004 */
/* [local] */ 

#endif /* pinterface */
#endif /* DEF___FIIterable_1_TestModel__CITypeProperties */


extern RPC_IF_HANDLE __MIDL_itf_TestModel_0000_0004_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_TestModel_0000_0004_v0_0_s_ifspec;

/* interface __MIDL_itf_TestModel2Eidl_0000_2398 */




extern RPC_IF_HANDLE __MIDL_itf_TestModel2Eidl_0000_2398_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_TestModel2Eidl_0000_2398_v0_0_s_ifspec;

/* interface __MIDL_itf_TestModel_0000_0005 */
/* [local] */ 

#ifndef DEF___FIVectorView_1_TestModel__CITypeProperties
#define DEF___FIVectorView_1_TestModel__CITypeProperties
#if !defined(__cplusplus) || defined(RO_NO_TEMPLATE_NAME)


extern RPC_IF_HANDLE __MIDL_itf_TestModel_0000_0005_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_TestModel_0000_0005_v0_0_s_ifspec;

#ifndef ____FIVectorView_1_TestModel__CITypeProperties_INTERFACE_DEFINED__
#define ____FIVectorView_1_TestModel__CITypeProperties_INTERFACE_DEFINED__

/* interface __FIVectorView_1_TestModel__CITypeProperties */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID___FIVectorView_1_TestModel__CITypeProperties;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("033f2bf3-dc24-53a2-9aa9-9a4888db8d58")
    __FIVectorView_1_TestModel__CITypeProperties : public IInspectable
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetAt( 
            /* [in] */ unsigned int index,
            /* [retval][out] */ __x_ABI_CTestModel_CITypeProperties **item) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Size( 
            /* [retval][out] */ unsigned int *size) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IndexOf( 
            /* [in] */ __x_ABI_CTestModel_CITypeProperties *item,
            /* [out] */ unsigned int *index,
            /* [retval][out] */ boolean *found) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMany( 
            /* [in] */ unsigned int startIndex,
            /* [in] */ unsigned int capacity,
            /* [size_is][length_is][out] */ __x_ABI_CTestModel_CITypeProperties **items,
            /* [retval][out] */ unsigned int *actual) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct __FIVectorView_1_TestModel__CITypePropertiesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __FIVectorView_1_TestModel__CITypeProperties * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __FIVectorView_1_TestModel__CITypeProperties * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __FIVectorView_1_TestModel__CITypeProperties * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetIids )( 
            __FIVectorView_1_TestModel__CITypeProperties * This,
            /* [out] */ ULONG *iidCount,
            /* [size_is][size_is][out] */ IID **iids);
        
        HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )( 
            __FIVectorView_1_TestModel__CITypeProperties * This,
            /* [out] */ HSTRING *className);
        
        HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )( 
            __FIVectorView_1_TestModel__CITypeProperties * This,
            /* [out] */ TrustLevel *trustLevel);
        
        HRESULT ( STDMETHODCALLTYPE *GetAt )( 
            __FIVectorView_1_TestModel__CITypeProperties * This,
            /* [in] */ unsigned int index,
            /* [retval][out] */ __x_ABI_CTestModel_CITypeProperties **item);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Size )( 
            __FIVectorView_1_TestModel__CITypeProperties * This,
            /* [retval][out] */ unsigned int *size);
        
        HRESULT ( STDMETHODCALLTYPE *IndexOf )( 
            __FIVectorView_1_TestModel__CITypeProperties * This,
            /* [in] */ __x_ABI_CTestModel_CITypeProperties *item,
            /* [out] */ unsigned int *index,
            /* [retval][out] */ boolean *found);
        
        HRESULT ( STDMETHODCALLTYPE *GetMany )( 
            __FIVectorView_1_TestModel__CITypeProperties * This,
            /* [in] */ unsigned int startIndex,
            /* [in] */ unsigned int capacity,
            /* [size_is][length_is][out] */ __x_ABI_CTestModel_CITypeProperties **items,
            /* [retval][out] */ unsigned int *actual);
        
        END_INTERFACE
    } __FIVectorView_1_TestModel__CITypePropertiesVtbl;

    interface __FIVectorView_1_TestModel__CITypeProperties
    {
        CONST_VTBL struct __FIVectorView_1_TestModel__CITypePropertiesVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define __FIVectorView_1_TestModel__CITypeProperties_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIVectorView_1_TestModel__CITypeProperties_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIVectorView_1_TestModel__CITypeProperties_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIVectorView_1_TestModel__CITypeProperties_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIVectorView_1_TestModel__CITypeProperties_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIVectorView_1_TestModel__CITypeProperties_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIVectorView_1_TestModel__CITypeProperties_GetAt(This,index,item)	\
    ( (This)->lpVtbl -> GetAt(This,index,item) ) 

#define __FIVectorView_1_TestModel__CITypeProperties_get_Size(This,size)	\
    ( (This)->lpVtbl -> get_Size(This,size) ) 

#define __FIVectorView_1_TestModel__CITypeProperties_IndexOf(This,item,index,found)	\
    ( (This)->lpVtbl -> IndexOf(This,item,index,found) ) 

#define __FIVectorView_1_TestModel__CITypeProperties_GetMany(This,startIndex,capacity,items,actual)	\
    ( (This)->lpVtbl -> GetMany(This,startIndex,capacity,items,actual) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* ____FIVectorView_1_TestModel__CITypeProperties_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_TestModel_0000_0006 */
/* [local] */ 

#endif /* pinterface */
#endif /* DEF___FIVectorView_1_TestModel__CITypeProperties */


extern RPC_IF_HANDLE __MIDL_itf_TestModel_0000_0006_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_TestModel_0000_0006_v0_0_s_ifspec;

/* interface __MIDL_itf_TestModel2Eidl_0000_2399 */




extern RPC_IF_HANDLE __MIDL_itf_TestModel2Eidl_0000_2399_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_TestModel2Eidl_0000_2399_v0_0_s_ifspec;

/* interface __MIDL_itf_TestModel_0000_0007 */
/* [local] */ 

#ifndef DEF___FIVector_1_TestModel__CITypeProperties
#define DEF___FIVector_1_TestModel__CITypeProperties
#if !defined(__cplusplus) || defined(RO_NO_TEMPLATE_NAME)


extern RPC_IF_HANDLE __MIDL_itf_TestModel_0000_0007_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_TestModel_0000_0007_v0_0_s_ifspec;

#ifndef ____FIVector_1_TestModel__CITypeProperties_INTERFACE_DEFINED__
#define ____FIVector_1_TestModel__CITypeProperties_INTERFACE_DEFINED__

/* interface __FIVector_1_TestModel__CITypeProperties */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID___FIVector_1_TestModel__CITypeProperties;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("7156edfe-e112-5963-9948-2bcf3f195d2d")
    __FIVector_1_TestModel__CITypeProperties : public IInspectable
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetAt( 
            /* [in] */ unsigned int index,
            /* [retval][out] */ __x_ABI_CTestModel_CITypeProperties **item) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Size( 
            /* [retval][out] */ unsigned int *size) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetView( 
            /* [retval][out] */ __FIVectorView_1_TestModel__CITypeProperties **view) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IndexOf( 
            /* [in] */ __x_ABI_CTestModel_CITypeProperties *item,
            /* [out] */ unsigned int *index,
            /* [retval][out] */ boolean *found) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetAt( 
            /* [in] */ unsigned int index,
            /* [in] */ __x_ABI_CTestModel_CITypeProperties *item) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE InsertAt( 
            /* [in] */ unsigned int index,
            /* [in] */ __x_ABI_CTestModel_CITypeProperties *item) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveAt( 
            /* [in] */ unsigned int index) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Append( 
            /* [in] */ __x_ABI_CTestModel_CITypeProperties *item) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveAtEnd( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clear( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMany( 
            /* [in] */ unsigned int startIndex,
            /* [in] */ unsigned int capacity,
            /* [size_is][length_is][out] */ __x_ABI_CTestModel_CITypeProperties **items,
            /* [retval][out] */ unsigned int *actual) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ReplaceAll( 
            /* [in] */ unsigned int count,
            /* [size_is][in] */ __x_ABI_CTestModel_CITypeProperties **value) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct __FIVector_1_TestModel__CITypePropertiesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __FIVector_1_TestModel__CITypeProperties * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __FIVector_1_TestModel__CITypeProperties * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __FIVector_1_TestModel__CITypeProperties * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetIids )( 
            __FIVector_1_TestModel__CITypeProperties * This,
            /* [out] */ ULONG *iidCount,
            /* [size_is][size_is][out] */ IID **iids);
        
        HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )( 
            __FIVector_1_TestModel__CITypeProperties * This,
            /* [out] */ HSTRING *className);
        
        HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )( 
            __FIVector_1_TestModel__CITypeProperties * This,
            /* [out] */ TrustLevel *trustLevel);
        
        HRESULT ( STDMETHODCALLTYPE *GetAt )( 
            __FIVector_1_TestModel__CITypeProperties * This,
            /* [in] */ unsigned int index,
            /* [retval][out] */ __x_ABI_CTestModel_CITypeProperties **item);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Size )( 
            __FIVector_1_TestModel__CITypeProperties * This,
            /* [retval][out] */ unsigned int *size);
        
        HRESULT ( STDMETHODCALLTYPE *GetView )( 
            __FIVector_1_TestModel__CITypeProperties * This,
            /* [retval][out] */ __FIVectorView_1_TestModel__CITypeProperties **view);
        
        HRESULT ( STDMETHODCALLTYPE *IndexOf )( 
            __FIVector_1_TestModel__CITypeProperties * This,
            /* [in] */ __x_ABI_CTestModel_CITypeProperties *item,
            /* [out] */ unsigned int *index,
            /* [retval][out] */ boolean *found);
        
        HRESULT ( STDMETHODCALLTYPE *SetAt )( 
            __FIVector_1_TestModel__CITypeProperties * This,
            /* [in] */ unsigned int index,
            /* [in] */ __x_ABI_CTestModel_CITypeProperties *item);
        
        HRESULT ( STDMETHODCALLTYPE *InsertAt )( 
            __FIVector_1_TestModel__CITypeProperties * This,
            /* [in] */ unsigned int index,
            /* [in] */ __x_ABI_CTestModel_CITypeProperties *item);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveAt )( 
            __FIVector_1_TestModel__CITypeProperties * This,
            /* [in] */ unsigned int index);
        
        HRESULT ( STDMETHODCALLTYPE *Append )( 
            __FIVector_1_TestModel__CITypeProperties * This,
            /* [in] */ __x_ABI_CTestModel_CITypeProperties *item);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveAtEnd )( 
            __FIVector_1_TestModel__CITypeProperties * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clear )( 
            __FIVector_1_TestModel__CITypeProperties * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetMany )( 
            __FIVector_1_TestModel__CITypeProperties * This,
            /* [in] */ unsigned int startIndex,
            /* [in] */ unsigned int capacity,
            /* [size_is][length_is][out] */ __x_ABI_CTestModel_CITypeProperties **items,
            /* [retval][out] */ unsigned int *actual);
        
        HRESULT ( STDMETHODCALLTYPE *ReplaceAll )( 
            __FIVector_1_TestModel__CITypeProperties * This,
            /* [in] */ unsigned int count,
            /* [size_is][in] */ __x_ABI_CTestModel_CITypeProperties **value);
        
        END_INTERFACE
    } __FIVector_1_TestModel__CITypePropertiesVtbl;

    interface __FIVector_1_TestModel__CITypeProperties
    {
        CONST_VTBL struct __FIVector_1_TestModel__CITypePropertiesVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define __FIVector_1_TestModel__CITypeProperties_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIVector_1_TestModel__CITypeProperties_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIVector_1_TestModel__CITypeProperties_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIVector_1_TestModel__CITypeProperties_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIVector_1_TestModel__CITypeProperties_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIVector_1_TestModel__CITypeProperties_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIVector_1_TestModel__CITypeProperties_GetAt(This,index,item)	\
    ( (This)->lpVtbl -> GetAt(This,index,item) ) 

#define __FIVector_1_TestModel__CITypeProperties_get_Size(This,size)	\
    ( (This)->lpVtbl -> get_Size(This,size) ) 

#define __FIVector_1_TestModel__CITypeProperties_GetView(This,view)	\
    ( (This)->lpVtbl -> GetView(This,view) ) 

#define __FIVector_1_TestModel__CITypeProperties_IndexOf(This,item,index,found)	\
    ( (This)->lpVtbl -> IndexOf(This,item,index,found) ) 

#define __FIVector_1_TestModel__CITypeProperties_SetAt(This,index,item)	\
    ( (This)->lpVtbl -> SetAt(This,index,item) ) 

#define __FIVector_1_TestModel__CITypeProperties_InsertAt(This,index,item)	\
    ( (This)->lpVtbl -> InsertAt(This,index,item) ) 

#define __FIVector_1_TestModel__CITypeProperties_RemoveAt(This,index)	\
    ( (This)->lpVtbl -> RemoveAt(This,index) ) 

#define __FIVector_1_TestModel__CITypeProperties_Append(This,item)	\
    ( (This)->lpVtbl -> Append(This,item) ) 

#define __FIVector_1_TestModel__CITypeProperties_RemoveAtEnd(This)	\
    ( (This)->lpVtbl -> RemoveAtEnd(This) ) 

#define __FIVector_1_TestModel__CITypeProperties_Clear(This)	\
    ( (This)->lpVtbl -> Clear(This) ) 

#define __FIVector_1_TestModel__CITypeProperties_GetMany(This,startIndex,capacity,items,actual)	\
    ( (This)->lpVtbl -> GetMany(This,startIndex,capacity,items,actual) ) 

#define __FIVector_1_TestModel__CITypeProperties_ReplaceAll(This,count,value)	\
    ( (This)->lpVtbl -> ReplaceAll(This,count,value) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* ____FIVector_1_TestModel__CITypeProperties_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_TestModel_0000_0008 */
/* [local] */ 

#endif /* pinterface */
#endif /* DEF___FIVector_1_TestModel__CITypeProperties */


extern RPC_IF_HANDLE __MIDL_itf_TestModel_0000_0008_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_TestModel_0000_0008_v0_0_s_ifspec;

/* interface __MIDL_itf_TestModel2Eidl_0000_2400 */




extern RPC_IF_HANDLE __MIDL_itf_TestModel2Eidl_0000_2400_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_TestModel2Eidl_0000_2400_v0_0_s_ifspec;

/* interface __MIDL_itf_TestModel_0000_0009 */
/* [local] */ 

#ifndef DEF___FIReference_1_TestModel__CTypeEnum
#define DEF___FIReference_1_TestModel__CTypeEnum
#if !defined(__cplusplus) || defined(RO_NO_TEMPLATE_NAME)


extern RPC_IF_HANDLE __MIDL_itf_TestModel_0000_0009_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_TestModel_0000_0009_v0_0_s_ifspec;

#ifndef ____FIReference_1_TestModel__CTypeEnum_INTERFACE_DEFINED__
#define ____FIReference_1_TestModel__CTypeEnum_INTERFACE_DEFINED__

/* interface __FIReference_1_TestModel__CTypeEnum */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID___FIReference_1_TestModel__CTypeEnum;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("ff8dcbe8-2be0-5053-bb93-5d8a35537412")
    __FIReference_1_TestModel__CTypeEnum : public IInspectable
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Value( 
            /* [retval][out] */ enum __x_ABI_CTestModel_CTypeEnum *value) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct __FIReference_1_TestModel__CTypeEnumVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __FIReference_1_TestModel__CTypeEnum * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __FIReference_1_TestModel__CTypeEnum * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __FIReference_1_TestModel__CTypeEnum * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetIids )( 
            __FIReference_1_TestModel__CTypeEnum * This,
            /* [out] */ ULONG *iidCount,
            /* [size_is][size_is][out] */ IID **iids);
        
        HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )( 
            __FIReference_1_TestModel__CTypeEnum * This,
            /* [out] */ HSTRING *className);
        
        HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )( 
            __FIReference_1_TestModel__CTypeEnum * This,
            /* [out] */ TrustLevel *trustLevel);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Value )( 
            __FIReference_1_TestModel__CTypeEnum * This,
            /* [retval][out] */ enum __x_ABI_CTestModel_CTypeEnum *value);
        
        END_INTERFACE
    } __FIReference_1_TestModel__CTypeEnumVtbl;

    interface __FIReference_1_TestModel__CTypeEnum
    {
        CONST_VTBL struct __FIReference_1_TestModel__CTypeEnumVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define __FIReference_1_TestModel__CTypeEnum_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIReference_1_TestModel__CTypeEnum_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIReference_1_TestModel__CTypeEnum_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIReference_1_TestModel__CTypeEnum_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIReference_1_TestModel__CTypeEnum_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIReference_1_TestModel__CTypeEnum_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIReference_1_TestModel__CTypeEnum_get_Value(This,value)	\
    ( (This)->lpVtbl -> get_Value(This,value) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* ____FIReference_1_TestModel__CTypeEnum_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_TestModel_0000_0010 */
/* [local] */ 

#endif /* pinterface */
#endif /* DEF___FIReference_1_TestModel__CTypeEnum */


extern RPC_IF_HANDLE __MIDL_itf_TestModel_0000_0010_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_TestModel_0000_0010_v0_0_s_ifspec;

/* interface __MIDL_itf_TestModel2Eidl_0000_2401 */




extern RPC_IF_HANDLE __MIDL_itf_TestModel2Eidl_0000_2401_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_TestModel2Eidl_0000_2401_v0_0_s_ifspec;

/* interface __MIDL_itf_TestModel_0000_0011 */
/* [local] */ 

#ifndef DEF___FIReference_1_TestModel__CTypeEnum2
#define DEF___FIReference_1_TestModel__CTypeEnum2
#if !defined(__cplusplus) || defined(RO_NO_TEMPLATE_NAME)


extern RPC_IF_HANDLE __MIDL_itf_TestModel_0000_0011_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_TestModel_0000_0011_v0_0_s_ifspec;

#ifndef ____FIReference_1_TestModel__CTypeEnum2_INTERFACE_DEFINED__
#define ____FIReference_1_TestModel__CTypeEnum2_INTERFACE_DEFINED__

/* interface __FIReference_1_TestModel__CTypeEnum2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID___FIReference_1_TestModel__CTypeEnum2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("f19323ca-3375-5acf-9547-e6f4436e3294")
    __FIReference_1_TestModel__CTypeEnum2 : public IInspectable
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Value( 
            /* [retval][out] */ enum __x_ABI_CTestModel_CTypeEnum2 *value) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct __FIReference_1_TestModel__CTypeEnum2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __FIReference_1_TestModel__CTypeEnum2 * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __FIReference_1_TestModel__CTypeEnum2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __FIReference_1_TestModel__CTypeEnum2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetIids )( 
            __FIReference_1_TestModel__CTypeEnum2 * This,
            /* [out] */ ULONG *iidCount,
            /* [size_is][size_is][out] */ IID **iids);
        
        HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )( 
            __FIReference_1_TestModel__CTypeEnum2 * This,
            /* [out] */ HSTRING *className);
        
        HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )( 
            __FIReference_1_TestModel__CTypeEnum2 * This,
            /* [out] */ TrustLevel *trustLevel);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Value )( 
            __FIReference_1_TestModel__CTypeEnum2 * This,
            /* [retval][out] */ enum __x_ABI_CTestModel_CTypeEnum2 *value);
        
        END_INTERFACE
    } __FIReference_1_TestModel__CTypeEnum2Vtbl;

    interface __FIReference_1_TestModel__CTypeEnum2
    {
        CONST_VTBL struct __FIReference_1_TestModel__CTypeEnum2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define __FIReference_1_TestModel__CTypeEnum2_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIReference_1_TestModel__CTypeEnum2_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIReference_1_TestModel__CTypeEnum2_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIReference_1_TestModel__CTypeEnum2_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIReference_1_TestModel__CTypeEnum2_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIReference_1_TestModel__CTypeEnum2_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIReference_1_TestModel__CTypeEnum2_get_Value(This,value)	\
    ( (This)->lpVtbl -> get_Value(This,value) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* ____FIReference_1_TestModel__CTypeEnum2_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_TestModel_0000_0012 */
/* [local] */ 

#endif /* pinterface */
#endif /* DEF___FIReference_1_TestModel__CTypeEnum2 */


extern RPC_IF_HANDLE __MIDL_itf_TestModel_0000_0012_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_TestModel_0000_0012_v0_0_s_ifspec;

/* interface __MIDL_itf_TestModel2Eidl_0000_2402 */




extern RPC_IF_HANDLE __MIDL_itf_TestModel2Eidl_0000_2402_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_TestModel2Eidl_0000_2402_v0_0_s_ifspec;

/* interface __MIDL_itf_TestModel_0000_0013 */
/* [local] */ 

#ifndef DEF___FIReference_1_TestModel__CTypeFlags
#define DEF___FIReference_1_TestModel__CTypeFlags
#if !defined(__cplusplus) || defined(RO_NO_TEMPLATE_NAME)


extern RPC_IF_HANDLE __MIDL_itf_TestModel_0000_0013_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_TestModel_0000_0013_v0_0_s_ifspec;

#ifndef ____FIReference_1_TestModel__CTypeFlags_INTERFACE_DEFINED__
#define ____FIReference_1_TestModel__CTypeFlags_INTERFACE_DEFINED__

/* interface __FIReference_1_TestModel__CTypeFlags */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID___FIReference_1_TestModel__CTypeFlags;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("fcda0b0f-1c0a-54c3-97b3-5a32742737a2")
    __FIReference_1_TestModel__CTypeFlags : public IInspectable
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Value( 
            /* [retval][out] */ enum __x_ABI_CTestModel_CTypeFlags *value) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct __FIReference_1_TestModel__CTypeFlagsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __FIReference_1_TestModel__CTypeFlags * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __FIReference_1_TestModel__CTypeFlags * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __FIReference_1_TestModel__CTypeFlags * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetIids )( 
            __FIReference_1_TestModel__CTypeFlags * This,
            /* [out] */ ULONG *iidCount,
            /* [size_is][size_is][out] */ IID **iids);
        
        HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )( 
            __FIReference_1_TestModel__CTypeFlags * This,
            /* [out] */ HSTRING *className);
        
        HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )( 
            __FIReference_1_TestModel__CTypeFlags * This,
            /* [out] */ TrustLevel *trustLevel);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Value )( 
            __FIReference_1_TestModel__CTypeFlags * This,
            /* [retval][out] */ enum __x_ABI_CTestModel_CTypeFlags *value);
        
        END_INTERFACE
    } __FIReference_1_TestModel__CTypeFlagsVtbl;

    interface __FIReference_1_TestModel__CTypeFlags
    {
        CONST_VTBL struct __FIReference_1_TestModel__CTypeFlagsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define __FIReference_1_TestModel__CTypeFlags_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIReference_1_TestModel__CTypeFlags_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIReference_1_TestModel__CTypeFlags_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIReference_1_TestModel__CTypeFlags_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIReference_1_TestModel__CTypeFlags_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIReference_1_TestModel__CTypeFlags_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIReference_1_TestModel__CTypeFlags_get_Value(This,value)	\
    ( (This)->lpVtbl -> get_Value(This,value) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* ____FIReference_1_TestModel__CTypeFlags_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_TestModel_0000_0014 */
/* [local] */ 

#endif /* pinterface */
#endif /* DEF___FIReference_1_TestModel__CTypeFlags */


extern RPC_IF_HANDLE __MIDL_itf_TestModel_0000_0014_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_TestModel_0000_0014_v0_0_s_ifspec;

/* interface __MIDL_itf_TestModel2Eidl_0000_2403 */




extern RPC_IF_HANDLE __MIDL_itf_TestModel2Eidl_0000_2403_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_TestModel2Eidl_0000_2403_v0_0_s_ifspec;

/* interface __MIDL_itf_TestModel_0000_0015 */
/* [local] */ 

#ifndef DEF___FIReference_1_TestModel__CTypeNonFlags
#define DEF___FIReference_1_TestModel__CTypeNonFlags
#if !defined(__cplusplus) || defined(RO_NO_TEMPLATE_NAME)


extern RPC_IF_HANDLE __MIDL_itf_TestModel_0000_0015_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_TestModel_0000_0015_v0_0_s_ifspec;

#ifndef ____FIReference_1_TestModel__CTypeNonFlags_INTERFACE_DEFINED__
#define ____FIReference_1_TestModel__CTypeNonFlags_INTERFACE_DEFINED__

/* interface __FIReference_1_TestModel__CTypeNonFlags */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID___FIReference_1_TestModel__CTypeNonFlags;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("83b8d6d6-f824-53ae-8b24-611434ecb8bb")
    __FIReference_1_TestModel__CTypeNonFlags : public IInspectable
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Value( 
            /* [retval][out] */ enum __x_ABI_CTestModel_CTypeNonFlags *value) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct __FIReference_1_TestModel__CTypeNonFlagsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __FIReference_1_TestModel__CTypeNonFlags * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __FIReference_1_TestModel__CTypeNonFlags * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __FIReference_1_TestModel__CTypeNonFlags * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetIids )( 
            __FIReference_1_TestModel__CTypeNonFlags * This,
            /* [out] */ ULONG *iidCount,
            /* [size_is][size_is][out] */ IID **iids);
        
        HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )( 
            __FIReference_1_TestModel__CTypeNonFlags * This,
            /* [out] */ HSTRING *className);
        
        HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )( 
            __FIReference_1_TestModel__CTypeNonFlags * This,
            /* [out] */ TrustLevel *trustLevel);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_Value )( 
            __FIReference_1_TestModel__CTypeNonFlags * This,
            /* [retval][out] */ enum __x_ABI_CTestModel_CTypeNonFlags *value);
        
        END_INTERFACE
    } __FIReference_1_TestModel__CTypeNonFlagsVtbl;

    interface __FIReference_1_TestModel__CTypeNonFlags
    {
        CONST_VTBL struct __FIReference_1_TestModel__CTypeNonFlagsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define __FIReference_1_TestModel__CTypeNonFlags_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __FIReference_1_TestModel__CTypeNonFlags_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __FIReference_1_TestModel__CTypeNonFlags_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __FIReference_1_TestModel__CTypeNonFlags_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __FIReference_1_TestModel__CTypeNonFlags_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __FIReference_1_TestModel__CTypeNonFlags_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __FIReference_1_TestModel__CTypeNonFlags_get_Value(This,value)	\
    ( (This)->lpVtbl -> get_Value(This,value) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* ____FIReference_1_TestModel__CTypeNonFlags_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_TestModel_0000_0016 */
/* [local] */ 

#endif /* pinterface */
#endif /* DEF___FIReference_1_TestModel__CTypeNonFlags */


extern RPC_IF_HANDLE __MIDL_itf_TestModel_0000_0016_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_TestModel_0000_0016_v0_0_s_ifspec;

#ifndef ____x_ABI_CTestModel_CITypeProperties_INTERFACE_DEFINED__
#define ____x_ABI_CTestModel_CITypeProperties_INTERFACE_DEFINED__

/* interface __x_ABI_CTestModel_CITypeProperties */
/* [uuid][object] */ 


EXTERN_C const IID IID___x_ABI_CTestModel_CITypeProperties;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4b7af9e8-d363-832e-9306-b6a6f135b0aa")
    __x_ABI_CTestModel_CITypeProperties : public IInspectable
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_TypeFlags( 
            /* [out][retval] */ __x_ABI_CTestModel_CTypeFlags *pValue) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_TypeFlags( 
            /* [in] */ __x_ABI_CTestModel_CTypeFlags value) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_TypeNonFlags( 
            /* [out][retval] */ __x_ABI_CTestModel_CTypeNonFlags *pValue) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_TypeNonFlags( 
            /* [in] */ __x_ABI_CTestModel_CTypeNonFlags value) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_BooleanProperty( 
            /* [out][retval] */ boolean *pValue) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_BooleanProperty( 
            /* [in] */ boolean value) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_ShortProperty( 
            /* [out][retval] */ INT16 *pValue) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_ShortProperty( 
            /* [in] */ INT16 value) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_UShortProperty( 
            /* [out][retval] */ unsigned short *pValue) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_UShortProperty( 
            /* [in] */ unsigned short value) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_IntProperty( 
            /* [out][retval] */ INT32 *pValue) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_IntProperty( 
            /* [in] */ INT32 value) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_UIntProperty( 
            /* [out][retval] */ unsigned int *pValue) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_UIntProperty( 
            /* [in] */ unsigned int value) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_LongProperty( 
            /* [out][retval] */ INT64 *pValue) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_LongProperty( 
            /* [in] */ INT64 value) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_ULongProperty( 
            /* [out][retval] */ UINT64 *pValue) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_ULongProperty( 
            /* [in] */ UINT64 value) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_StringProperty( 
            /* [out][retval] */ HSTRING *pValue) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_StringProperty( 
            /* [in] */ HSTRING value) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_ByteProperty( 
            /* [out][retval] */ BYTE *pValue) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_ByteProperty( 
            /* [in] */ BYTE value) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_DoubleProperty( 
            /* [out][retval] */ DOUBLE *pValue) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_DoubleProperty( 
            /* [in] */ DOUBLE value) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_FloatProperty( 
            /* [out][retval] */ FLOAT *pValue) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_FloatProperty( 
            /* [in] */ FLOAT value) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_GuidProperty( 
            /* [out][retval] */ GUID *pValue) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_GuidProperty( 
            /* [in] */ GUID value) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_ObjectProperty( 
            /* [out][retval] */ IInspectable **pValue) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_ObjectProperty( 
            /* [in] */ IInspectable *value) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_DateTimeProperty( 
            /* [out][retval] */ __x_ABI_CWindows_CFoundation_CDateTime *pValue) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_DateTimeProperty( 
            /* [in] */ __x_ABI_CWindows_CFoundation_CDateTime value) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_StringArrayProperty( 
            /* [out] */ UINT32 *length,
            /* [out][size_is][size_is][retval] */ HSTRING **pValue) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_StringArrayProperty( 
            /* [in] */ UINT32 length,
            /* [in][size_is] */ HSTRING *value) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_ObjectArrayProperty( 
            /* [out] */ UINT32 *length,
            /* [out][size_is][size_is][retval] */ IInspectable ***pValue) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_ObjectArrayProperty( 
            /* [in] */ UINT32 length,
            /* [in][size_is] */ IInspectable **value) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_DictionaryProperty( 
            /* [out][retval] */ __x_ABI_CMicrosoft_CPropertyModel_CIDictionary **pValue) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_DictionaryProperty( 
            /* [in] */ __x_ABI_CMicrosoft_CPropertyModel_CIDictionary *value) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct __x_ABI_CTestModel_CITypePropertiesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __x_ABI_CTestModel_CITypeProperties * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __x_ABI_CTestModel_CITypeProperties * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __x_ABI_CTestModel_CITypeProperties * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetIids )( 
            __x_ABI_CTestModel_CITypeProperties * This,
            /* [out] */ ULONG *iidCount,
            /* [size_is][size_is][out] */ IID **iids);
        
        HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )( 
            __x_ABI_CTestModel_CITypeProperties * This,
            /* [out] */ HSTRING *className);
        
        HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )( 
            __x_ABI_CTestModel_CITypeProperties * This,
            /* [out] */ TrustLevel *trustLevel);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_TypeFlags )( 
            __x_ABI_CTestModel_CITypeProperties * This,
            /* [out][retval] */ __x_ABI_CTestModel_CTypeFlags *pValue);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_TypeFlags )( 
            __x_ABI_CTestModel_CITypeProperties * This,
            /* [in] */ __x_ABI_CTestModel_CTypeFlags value);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_TypeNonFlags )( 
            __x_ABI_CTestModel_CITypeProperties * This,
            /* [out][retval] */ __x_ABI_CTestModel_CTypeNonFlags *pValue);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_TypeNonFlags )( 
            __x_ABI_CTestModel_CITypeProperties * This,
            /* [in] */ __x_ABI_CTestModel_CTypeNonFlags value);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_BooleanProperty )( 
            __x_ABI_CTestModel_CITypeProperties * This,
            /* [out][retval] */ boolean *pValue);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_BooleanProperty )( 
            __x_ABI_CTestModel_CITypeProperties * This,
            /* [in] */ boolean value);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_ShortProperty )( 
            __x_ABI_CTestModel_CITypeProperties * This,
            /* [out][retval] */ INT16 *pValue);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_ShortProperty )( 
            __x_ABI_CTestModel_CITypeProperties * This,
            /* [in] */ INT16 value);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_UShortProperty )( 
            __x_ABI_CTestModel_CITypeProperties * This,
            /* [out][retval] */ unsigned short *pValue);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_UShortProperty )( 
            __x_ABI_CTestModel_CITypeProperties * This,
            /* [in] */ unsigned short value);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_IntProperty )( 
            __x_ABI_CTestModel_CITypeProperties * This,
            /* [out][retval] */ INT32 *pValue);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_IntProperty )( 
            __x_ABI_CTestModel_CITypeProperties * This,
            /* [in] */ INT32 value);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_UIntProperty )( 
            __x_ABI_CTestModel_CITypeProperties * This,
            /* [out][retval] */ unsigned int *pValue);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_UIntProperty )( 
            __x_ABI_CTestModel_CITypeProperties * This,
            /* [in] */ unsigned int value);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_LongProperty )( 
            __x_ABI_CTestModel_CITypeProperties * This,
            /* [out][retval] */ INT64 *pValue);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_LongProperty )( 
            __x_ABI_CTestModel_CITypeProperties * This,
            /* [in] */ INT64 value);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_ULongProperty )( 
            __x_ABI_CTestModel_CITypeProperties * This,
            /* [out][retval] */ UINT64 *pValue);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_ULongProperty )( 
            __x_ABI_CTestModel_CITypeProperties * This,
            /* [in] */ UINT64 value);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_StringProperty )( 
            __x_ABI_CTestModel_CITypeProperties * This,
            /* [out][retval] */ HSTRING *pValue);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_StringProperty )( 
            __x_ABI_CTestModel_CITypeProperties * This,
            /* [in] */ HSTRING value);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_ByteProperty )( 
            __x_ABI_CTestModel_CITypeProperties * This,
            /* [out][retval] */ BYTE *pValue);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_ByteProperty )( 
            __x_ABI_CTestModel_CITypeProperties * This,
            /* [in] */ BYTE value);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_DoubleProperty )( 
            __x_ABI_CTestModel_CITypeProperties * This,
            /* [out][retval] */ DOUBLE *pValue);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_DoubleProperty )( 
            __x_ABI_CTestModel_CITypeProperties * This,
            /* [in] */ DOUBLE value);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_FloatProperty )( 
            __x_ABI_CTestModel_CITypeProperties * This,
            /* [out][retval] */ FLOAT *pValue);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_FloatProperty )( 
            __x_ABI_CTestModel_CITypeProperties * This,
            /* [in] */ FLOAT value);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_GuidProperty )( 
            __x_ABI_CTestModel_CITypeProperties * This,
            /* [out][retval] */ GUID *pValue);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_GuidProperty )( 
            __x_ABI_CTestModel_CITypeProperties * This,
            /* [in] */ GUID value);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_ObjectProperty )( 
            __x_ABI_CTestModel_CITypeProperties * This,
            /* [out][retval] */ IInspectable **pValue);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_ObjectProperty )( 
            __x_ABI_CTestModel_CITypeProperties * This,
            /* [in] */ IInspectable *value);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_DateTimeProperty )( 
            __x_ABI_CTestModel_CITypeProperties * This,
            /* [out][retval] */ __x_ABI_CWindows_CFoundation_CDateTime *pValue);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_DateTimeProperty )( 
            __x_ABI_CTestModel_CITypeProperties * This,
            /* [in] */ __x_ABI_CWindows_CFoundation_CDateTime value);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_StringArrayProperty )( 
            __x_ABI_CTestModel_CITypeProperties * This,
            /* [out] */ UINT32 *length,
            /* [out][size_is][size_is][retval] */ HSTRING **pValue);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_StringArrayProperty )( 
            __x_ABI_CTestModel_CITypeProperties * This,
            /* [in] */ UINT32 length,
            /* [in][size_is] */ HSTRING *value);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_ObjectArrayProperty )( 
            __x_ABI_CTestModel_CITypeProperties * This,
            /* [out] */ UINT32 *length,
            /* [out][size_is][size_is][retval] */ IInspectable ***pValue);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_ObjectArrayProperty )( 
            __x_ABI_CTestModel_CITypeProperties * This,
            /* [in] */ UINT32 length,
            /* [in][size_is] */ IInspectable **value);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_DictionaryProperty )( 
            __x_ABI_CTestModel_CITypeProperties * This,
            /* [out][retval] */ __x_ABI_CMicrosoft_CPropertyModel_CIDictionary **pValue);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE *put_DictionaryProperty )( 
            __x_ABI_CTestModel_CITypeProperties * This,
            /* [in] */ __x_ABI_CMicrosoft_CPropertyModel_CIDictionary *value);
        
        END_INTERFACE
    } __x_ABI_CTestModel_CITypePropertiesVtbl;

    interface __x_ABI_CTestModel_CITypeProperties
    {
        CONST_VTBL struct __x_ABI_CTestModel_CITypePropertiesVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define __x_ABI_CTestModel_CITypeProperties_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __x_ABI_CTestModel_CITypeProperties_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __x_ABI_CTestModel_CITypeProperties_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __x_ABI_CTestModel_CITypeProperties_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __x_ABI_CTestModel_CITypeProperties_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __x_ABI_CTestModel_CITypeProperties_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __x_ABI_CTestModel_CITypeProperties_get_TypeFlags(This,pValue)	\
    ( (This)->lpVtbl -> get_TypeFlags(This,pValue) ) 

#define __x_ABI_CTestModel_CITypeProperties_put_TypeFlags(This,value)	\
    ( (This)->lpVtbl -> put_TypeFlags(This,value) ) 

#define __x_ABI_CTestModel_CITypeProperties_get_TypeNonFlags(This,pValue)	\
    ( (This)->lpVtbl -> get_TypeNonFlags(This,pValue) ) 

#define __x_ABI_CTestModel_CITypeProperties_put_TypeNonFlags(This,value)	\
    ( (This)->lpVtbl -> put_TypeNonFlags(This,value) ) 

#define __x_ABI_CTestModel_CITypeProperties_get_BooleanProperty(This,pValue)	\
    ( (This)->lpVtbl -> get_BooleanProperty(This,pValue) ) 

#define __x_ABI_CTestModel_CITypeProperties_put_BooleanProperty(This,value)	\
    ( (This)->lpVtbl -> put_BooleanProperty(This,value) ) 

#define __x_ABI_CTestModel_CITypeProperties_get_ShortProperty(This,pValue)	\
    ( (This)->lpVtbl -> get_ShortProperty(This,pValue) ) 

#define __x_ABI_CTestModel_CITypeProperties_put_ShortProperty(This,value)	\
    ( (This)->lpVtbl -> put_ShortProperty(This,value) ) 

#define __x_ABI_CTestModel_CITypeProperties_get_UShortProperty(This,pValue)	\
    ( (This)->lpVtbl -> get_UShortProperty(This,pValue) ) 

#define __x_ABI_CTestModel_CITypeProperties_put_UShortProperty(This,value)	\
    ( (This)->lpVtbl -> put_UShortProperty(This,value) ) 

#define __x_ABI_CTestModel_CITypeProperties_get_IntProperty(This,pValue)	\
    ( (This)->lpVtbl -> get_IntProperty(This,pValue) ) 

#define __x_ABI_CTestModel_CITypeProperties_put_IntProperty(This,value)	\
    ( (This)->lpVtbl -> put_IntProperty(This,value) ) 

#define __x_ABI_CTestModel_CITypeProperties_get_UIntProperty(This,pValue)	\
    ( (This)->lpVtbl -> get_UIntProperty(This,pValue) ) 

#define __x_ABI_CTestModel_CITypeProperties_put_UIntProperty(This,value)	\
    ( (This)->lpVtbl -> put_UIntProperty(This,value) ) 

#define __x_ABI_CTestModel_CITypeProperties_get_LongProperty(This,pValue)	\
    ( (This)->lpVtbl -> get_LongProperty(This,pValue) ) 

#define __x_ABI_CTestModel_CITypeProperties_put_LongProperty(This,value)	\
    ( (This)->lpVtbl -> put_LongProperty(This,value) ) 

#define __x_ABI_CTestModel_CITypeProperties_get_ULongProperty(This,pValue)	\
    ( (This)->lpVtbl -> get_ULongProperty(This,pValue) ) 

#define __x_ABI_CTestModel_CITypeProperties_put_ULongProperty(This,value)	\
    ( (This)->lpVtbl -> put_ULongProperty(This,value) ) 

#define __x_ABI_CTestModel_CITypeProperties_get_StringProperty(This,pValue)	\
    ( (This)->lpVtbl -> get_StringProperty(This,pValue) ) 

#define __x_ABI_CTestModel_CITypeProperties_put_StringProperty(This,value)	\
    ( (This)->lpVtbl -> put_StringProperty(This,value) ) 

#define __x_ABI_CTestModel_CITypeProperties_get_ByteProperty(This,pValue)	\
    ( (This)->lpVtbl -> get_ByteProperty(This,pValue) ) 

#define __x_ABI_CTestModel_CITypeProperties_put_ByteProperty(This,value)	\
    ( (This)->lpVtbl -> put_ByteProperty(This,value) ) 

#define __x_ABI_CTestModel_CITypeProperties_get_DoubleProperty(This,pValue)	\
    ( (This)->lpVtbl -> get_DoubleProperty(This,pValue) ) 

#define __x_ABI_CTestModel_CITypeProperties_put_DoubleProperty(This,value)	\
    ( (This)->lpVtbl -> put_DoubleProperty(This,value) ) 

#define __x_ABI_CTestModel_CITypeProperties_get_FloatProperty(This,pValue)	\
    ( (This)->lpVtbl -> get_FloatProperty(This,pValue) ) 

#define __x_ABI_CTestModel_CITypeProperties_put_FloatProperty(This,value)	\
    ( (This)->lpVtbl -> put_FloatProperty(This,value) ) 

#define __x_ABI_CTestModel_CITypeProperties_get_GuidProperty(This,pValue)	\
    ( (This)->lpVtbl -> get_GuidProperty(This,pValue) ) 

#define __x_ABI_CTestModel_CITypeProperties_put_GuidProperty(This,value)	\
    ( (This)->lpVtbl -> put_GuidProperty(This,value) ) 

#define __x_ABI_CTestModel_CITypeProperties_get_ObjectProperty(This,pValue)	\
    ( (This)->lpVtbl -> get_ObjectProperty(This,pValue) ) 

#define __x_ABI_CTestModel_CITypeProperties_put_ObjectProperty(This,value)	\
    ( (This)->lpVtbl -> put_ObjectProperty(This,value) ) 

#define __x_ABI_CTestModel_CITypeProperties_get_DateTimeProperty(This,pValue)	\
    ( (This)->lpVtbl -> get_DateTimeProperty(This,pValue) ) 

#define __x_ABI_CTestModel_CITypeProperties_put_DateTimeProperty(This,value)	\
    ( (This)->lpVtbl -> put_DateTimeProperty(This,value) ) 

#define __x_ABI_CTestModel_CITypeProperties_get_StringArrayProperty(This,length,pValue)	\
    ( (This)->lpVtbl -> get_StringArrayProperty(This,length,pValue) ) 

#define __x_ABI_CTestModel_CITypeProperties_put_StringArrayProperty(This,length,value)	\
    ( (This)->lpVtbl -> put_StringArrayProperty(This,length,value) ) 

#define __x_ABI_CTestModel_CITypeProperties_get_ObjectArrayProperty(This,length,pValue)	\
    ( (This)->lpVtbl -> get_ObjectArrayProperty(This,length,pValue) ) 

#define __x_ABI_CTestModel_CITypeProperties_put_ObjectArrayProperty(This,length,value)	\
    ( (This)->lpVtbl -> put_ObjectArrayProperty(This,length,value) ) 

#define __x_ABI_CTestModel_CITypeProperties_get_DictionaryProperty(This,pValue)	\
    ( (This)->lpVtbl -> get_DictionaryProperty(This,pValue) ) 

#define __x_ABI_CTestModel_CITypeProperties_put_DictionaryProperty(This,value)	\
    ( (This)->lpVtbl -> put_DictionaryProperty(This,value) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* ____x_ABI_CTestModel_CITypeProperties_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_TestModel_0000_0017 */
/* [local] */ 

/* [v1_enum] */ 
enum __x_ABI_CTestModel_CTypePropertiesProperties
    {
        TypePropertiesProperties_TypeFlags	= 1,
        TypePropertiesProperties_TypeNonFlags	= 2,
        TypePropertiesProperties_BooleanProperty	= 3,
        TypePropertiesProperties_ShortProperty	= 4,
        TypePropertiesProperties_UShortProperty	= 5,
        TypePropertiesProperties_IntProperty	= 6,
        TypePropertiesProperties_UIntProperty	= 7,
        TypePropertiesProperties_LongProperty	= 8,
        TypePropertiesProperties_ULongProperty	= 9,
        TypePropertiesProperties_StringProperty	= 10,
        TypePropertiesProperties_ByteProperty	= 11,
        TypePropertiesProperties_DoubleProperty	= 12,
        TypePropertiesProperties_FloatProperty	= 13,
        TypePropertiesProperties_GuidProperty	= 14,
        TypePropertiesProperties_ObjectProperty	= 15,
        TypePropertiesProperties_DateTimeProperty	= 16,
        TypePropertiesProperties_StringArrayProperty	= 17,
        TypePropertiesProperties_ObjectArrayProperty	= 18,
        TypePropertiesProperties_DictionaryProperty	= 19
    } ;
typedef enum __x_ABI_CTestModel_CTypePropertiesProperties __x_ABI_CTestModel_CTypePropertiesProperties;



extern RPC_IF_HANDLE __MIDL_itf_TestModel_0000_0017_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_TestModel_0000_0017_v0_0_s_ifspec;

#ifndef ____x_ABI_CTestModel_CITypePropertiesTest_INTERFACE_DEFINED__
#define ____x_ABI_CTestModel_CITypePropertiesTest_INTERFACE_DEFINED__

/* interface __x_ABI_CTestModel_CITypePropertiesTest */
/* [uuid][object] */ 


EXTERN_C const IID IID___x_ABI_CTestModel_CITypePropertiesTest;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("252468e7-b600-b18b-271b-6a2a4c5d47f8")
    __x_ABI_CTestModel_CITypePropertiesTest : public IInspectable
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_TypeProperties( 
            /* [out][retval] */ __x_ABI_CTestModel_CITypeProperties **pValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ChangePropertyAsync( 
            /* [in] */ unsigned int propertyId,
            /* [in] */ IInspectable *propertyValue,
            /* [out][retval] */ __FIAsyncOperation_1_IInspectable **result) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct __x_ABI_CTestModel_CITypePropertiesTestVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __x_ABI_CTestModel_CITypePropertiesTest * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __x_ABI_CTestModel_CITypePropertiesTest * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __x_ABI_CTestModel_CITypePropertiesTest * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetIids )( 
            __x_ABI_CTestModel_CITypePropertiesTest * This,
            /* [out] */ ULONG *iidCount,
            /* [size_is][size_is][out] */ IID **iids);
        
        HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )( 
            __x_ABI_CTestModel_CITypePropertiesTest * This,
            /* [out] */ HSTRING *className);
        
        HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )( 
            __x_ABI_CTestModel_CITypePropertiesTest * This,
            /* [out] */ TrustLevel *trustLevel);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_TypeProperties )( 
            __x_ABI_CTestModel_CITypePropertiesTest * This,
            /* [out][retval] */ __x_ABI_CTestModel_CITypeProperties **pValue);
        
        HRESULT ( STDMETHODCALLTYPE *ChangePropertyAsync )( 
            __x_ABI_CTestModel_CITypePropertiesTest * This,
            /* [in] */ unsigned int propertyId,
            /* [in] */ IInspectable *propertyValue,
            /* [out][retval] */ __FIAsyncOperation_1_IInspectable **result);
        
        END_INTERFACE
    } __x_ABI_CTestModel_CITypePropertiesTestVtbl;

    interface __x_ABI_CTestModel_CITypePropertiesTest
    {
        CONST_VTBL struct __x_ABI_CTestModel_CITypePropertiesTestVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define __x_ABI_CTestModel_CITypePropertiesTest_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __x_ABI_CTestModel_CITypePropertiesTest_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __x_ABI_CTestModel_CITypePropertiesTest_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __x_ABI_CTestModel_CITypePropertiesTest_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __x_ABI_CTestModel_CITypePropertiesTest_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __x_ABI_CTestModel_CITypePropertiesTest_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __x_ABI_CTestModel_CITypePropertiesTest_get_TypeProperties(This,pValue)	\
    ( (This)->lpVtbl -> get_TypeProperties(This,pValue) ) 

#define __x_ABI_CTestModel_CITypePropertiesTest_ChangePropertyAsync(This,propertyId,propertyValue,result)	\
    ( (This)->lpVtbl -> ChangePropertyAsync(This,propertyId,propertyValue,result) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* ____x_ABI_CTestModel_CITypePropertiesTest_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_TestModel_0000_0018 */
/* [local] */ 

/* [v1_enum] */ 
enum __x_ABI_CTestModel_CTypePropertiesTestProperties
    {
        TypePropertiesTestProperties_TypeProperties	= 1
    } ;
typedef enum __x_ABI_CTestModel_CTypePropertiesTestProperties __x_ABI_CTestModel_CTypePropertiesTestProperties;

/* [v1_enum] */ 
enum __x_ABI_CTestModel_CTypePropertiesTestMethods
    {
        TypePropertiesTestMethods_ChangePropertyAsync	= 1
    } ;
typedef enum __x_ABI_CTestModel_CTypePropertiesTestMethods __x_ABI_CTestModel_CTypePropertiesTestMethods;



extern RPC_IF_HANDLE __MIDL_itf_TestModel_0000_0018_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_TestModel_0000_0018_v0_0_s_ifspec;

#ifndef ____x_ABI_CTestModel_CICommandTypes_INTERFACE_DEFINED__
#define ____x_ABI_CTestModel_CICommandTypes_INTERFACE_DEFINED__

/* interface __x_ABI_CTestModel_CICommandTypes */
/* [uuid][object] */ 


EXTERN_C const IID IID___x_ABI_CTestModel_CICommandTypes;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0e3afbae-7aa1-839f-e654-a4b7d7b8394c")
    __x_ABI_CTestModel_CICommandTypes : public IInspectable
    {
    public:
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_GetString( 
            /* [out][retval] */ __x_ABI_CTestModel_CICommand__void__string **pValue) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_GetStringAsync( 
            /* [out][retval] */ __x_ABI_CTestModel_CICommand__void__string__async **pValue) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct __x_ABI_CTestModel_CICommandTypesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __x_ABI_CTestModel_CICommandTypes * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __x_ABI_CTestModel_CICommandTypes * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __x_ABI_CTestModel_CICommandTypes * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetIids )( 
            __x_ABI_CTestModel_CICommandTypes * This,
            /* [out] */ ULONG *iidCount,
            /* [size_is][size_is][out] */ IID **iids);
        
        HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )( 
            __x_ABI_CTestModel_CICommandTypes * This,
            /* [out] */ HSTRING *className);
        
        HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )( 
            __x_ABI_CTestModel_CICommandTypes * This,
            /* [out] */ TrustLevel *trustLevel);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_GetString )( 
            __x_ABI_CTestModel_CICommandTypes * This,
            /* [out][retval] */ __x_ABI_CTestModel_CICommand__void__string **pValue);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE *get_GetStringAsync )( 
            __x_ABI_CTestModel_CICommandTypes * This,
            /* [out][retval] */ __x_ABI_CTestModel_CICommand__void__string__async **pValue);
        
        END_INTERFACE
    } __x_ABI_CTestModel_CICommandTypesVtbl;

    interface __x_ABI_CTestModel_CICommandTypes
    {
        CONST_VTBL struct __x_ABI_CTestModel_CICommandTypesVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define __x_ABI_CTestModel_CICommandTypes_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __x_ABI_CTestModel_CICommandTypes_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __x_ABI_CTestModel_CICommandTypes_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __x_ABI_CTestModel_CICommandTypes_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __x_ABI_CTestModel_CICommandTypes_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __x_ABI_CTestModel_CICommandTypes_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __x_ABI_CTestModel_CICommandTypes_get_GetString(This,pValue)	\
    ( (This)->lpVtbl -> get_GetString(This,pValue) ) 

#define __x_ABI_CTestModel_CICommandTypes_get_GetStringAsync(This,pValue)	\
    ( (This)->lpVtbl -> get_GetStringAsync(This,pValue) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* ____x_ABI_CTestModel_CICommandTypes_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_TestModel_0000_0019 */
/* [local] */ 

/* [v1_enum] */ 
enum __x_ABI_CTestModel_CCommandTypesProperties
    {
        CommandTypesProperties_GetString	= 1,
        CommandTypesProperties_GetStringAsync	= 2
    } ;
typedef enum __x_ABI_CTestModel_CCommandTypesProperties __x_ABI_CTestModel_CCommandTypesProperties;



extern RPC_IF_HANDLE __MIDL_itf_TestModel_0000_0019_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_TestModel_0000_0019_v0_0_s_ifspec;

#ifndef ____x_ABI_CTestModel_CIMethodBase_INTERFACE_DEFINED__
#define ____x_ABI_CTestModel_CIMethodBase_INTERFACE_DEFINED__

/* interface __x_ABI_CTestModel_CIMethodBase */
/* [uuid][object] */ 


EXTERN_C const IID IID___x_ABI_CTestModel_CIMethodBase;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("72d1e860-e7f4-db8e-97a8-5965eea04de5")
    __x_ABI_CTestModel_CIMethodBase : public IInspectable
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Method1( void) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct __x_ABI_CTestModel_CIMethodBaseVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __x_ABI_CTestModel_CIMethodBase * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __x_ABI_CTestModel_CIMethodBase * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __x_ABI_CTestModel_CIMethodBase * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetIids )( 
            __x_ABI_CTestModel_CIMethodBase * This,
            /* [out] */ ULONG *iidCount,
            /* [size_is][size_is][out] */ IID **iids);
        
        HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )( 
            __x_ABI_CTestModel_CIMethodBase * This,
            /* [out] */ HSTRING *className);
        
        HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )( 
            __x_ABI_CTestModel_CIMethodBase * This,
            /* [out] */ TrustLevel *trustLevel);
        
        HRESULT ( STDMETHODCALLTYPE *Method1 )( 
            __x_ABI_CTestModel_CIMethodBase * This);
        
        END_INTERFACE
    } __x_ABI_CTestModel_CIMethodBaseVtbl;

    interface __x_ABI_CTestModel_CIMethodBase
    {
        CONST_VTBL struct __x_ABI_CTestModel_CIMethodBaseVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define __x_ABI_CTestModel_CIMethodBase_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __x_ABI_CTestModel_CIMethodBase_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __x_ABI_CTestModel_CIMethodBase_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __x_ABI_CTestModel_CIMethodBase_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __x_ABI_CTestModel_CIMethodBase_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __x_ABI_CTestModel_CIMethodBase_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __x_ABI_CTestModel_CIMethodBase_Method1(This)	\
    ( (This)->lpVtbl -> Method1(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* ____x_ABI_CTestModel_CIMethodBase_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_TestModel_0000_0020 */
/* [local] */ 

/* [v1_enum] */ 
enum __x_ABI_CTestModel_CMethodBaseMethods
    {
        MethodBaseMethods_Method1	= 1
    } ;
typedef enum __x_ABI_CTestModel_CMethodBaseMethods __x_ABI_CTestModel_CMethodBaseMethods;



extern RPC_IF_HANDLE __MIDL_itf_TestModel_0000_0020_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_TestModel_0000_0020_v0_0_s_ifspec;

#ifndef ____x_ABI_CTestModel_CIMethodDerived_INTERFACE_DEFINED__
#define ____x_ABI_CTestModel_CIMethodDerived_INTERFACE_DEFINED__

/* interface __x_ABI_CTestModel_CIMethodDerived */
/* [uuid][object] */ 


EXTERN_C const IID IID___x_ABI_CTestModel_CIMethodDerived;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("bc1f2b83-8819-4cd4-11b4-8d23bb80de44")
    __x_ABI_CTestModel_CIMethodDerived : public IInspectable
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Method2( void) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct __x_ABI_CTestModel_CIMethodDerivedVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __x_ABI_CTestModel_CIMethodDerived * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __x_ABI_CTestModel_CIMethodDerived * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __x_ABI_CTestModel_CIMethodDerived * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetIids )( 
            __x_ABI_CTestModel_CIMethodDerived * This,
            /* [out] */ ULONG *iidCount,
            /* [size_is][size_is][out] */ IID **iids);
        
        HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )( 
            __x_ABI_CTestModel_CIMethodDerived * This,
            /* [out] */ HSTRING *className);
        
        HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )( 
            __x_ABI_CTestModel_CIMethodDerived * This,
            /* [out] */ TrustLevel *trustLevel);
        
        HRESULT ( STDMETHODCALLTYPE *Method2 )( 
            __x_ABI_CTestModel_CIMethodDerived * This);
        
        END_INTERFACE
    } __x_ABI_CTestModel_CIMethodDerivedVtbl;

    interface __x_ABI_CTestModel_CIMethodDerived
    {
        CONST_VTBL struct __x_ABI_CTestModel_CIMethodDerivedVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define __x_ABI_CTestModel_CIMethodDerived_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __x_ABI_CTestModel_CIMethodDerived_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __x_ABI_CTestModel_CIMethodDerived_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __x_ABI_CTestModel_CIMethodDerived_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __x_ABI_CTestModel_CIMethodDerived_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __x_ABI_CTestModel_CIMethodDerived_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __x_ABI_CTestModel_CIMethodDerived_Method2(This)	\
    ( (This)->lpVtbl -> Method2(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* ____x_ABI_CTestModel_CIMethodDerived_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_TestModel_0000_0021 */
/* [local] */ 

/* [v1_enum] */ 
enum __x_ABI_CTestModel_CMethodDerivedMethods
    {
        MethodDerivedMethods_Method2	= 2
    } ;
typedef enum __x_ABI_CTestModel_CMethodDerivedMethods __x_ABI_CTestModel_CMethodDerivedMethods;



extern RPC_IF_HANDLE __MIDL_itf_TestModel_0000_0021_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_TestModel_0000_0021_v0_0_s_ifspec;

#ifndef ____x_ABI_CTestModel_CIMethodSupport_INTERFACE_DEFINED__
#define ____x_ABI_CTestModel_CIMethodSupport_INTERFACE_DEFINED__

/* interface __x_ABI_CTestModel_CIMethodSupport */
/* [uuid][object] */ 


EXTERN_C const IID IID___x_ABI_CTestModel_CIMethodSupport;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("76826ae6-ae4a-93f2-ec60-15d8c3741474")
    __x_ABI_CTestModel_CIMethodSupport : public IInspectable
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE VoidEmpty( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Void_Int( 
            /* [in] */ INT32 value) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Void_Object( 
            /* [in] */ __x_ABI_CMicrosoft_CPropertyModel_CIObject *pObject) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Void_Dictionary( 
            /* [in] */ __x_ABI_CMicrosoft_CPropertyModel_CIDictionary *dictionary) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ReturnInt( 
            /* [out][retval] */ INT32 *result) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ReturnInt_Int( 
            /* [in] */ INT32 value,
            /* [out][retval] */ INT32 *result) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ReturnDictionary( 
            /* [out][retval] */ __x_ABI_CMicrosoft_CPropertyModel_CIDictionary **result) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ReturnObjectAsync( 
            /* [out][retval] */ __FIAsyncOperation_1_Microsoft__CPropertyModel__CIObject **result) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ReturnDictionaryAsync( 
            /* [out][retval] */ __FIAsyncOperation_1_Microsoft__CPropertyModel__CIDictionary **result) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE VoidAsync( 
            /* [out][retval] */ __FIAsyncOperation_1_IInspectable **result) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE VoidAsync_Int( 
            /* [in] */ INT32 value,
            /* [out][retval] */ __FIAsyncOperation_1_int **result) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ReturnInt_Async( 
            /* [out][retval] */ __FIAsyncOperation_1_int **result) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Void_ObjectAsync( 
            /* [in] */ __x_ABI_CMicrosoft_CPropertyModel_CIObject *pObject,
            /* [out][retval] */ __FIAsyncOperation_1_IInspectable **result) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Void_DictionaryAsync( 
            /* [in] */ __x_ABI_CMicrosoft_CPropertyModel_CIDictionary *dictionary,
            /* [out][retval] */ __FIAsyncOperation_1_IInspectable **result) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct __x_ABI_CTestModel_CIMethodSupportVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __x_ABI_CTestModel_CIMethodSupport * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __x_ABI_CTestModel_CIMethodSupport * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __x_ABI_CTestModel_CIMethodSupport * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetIids )( 
            __x_ABI_CTestModel_CIMethodSupport * This,
            /* [out] */ ULONG *iidCount,
            /* [size_is][size_is][out] */ IID **iids);
        
        HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )( 
            __x_ABI_CTestModel_CIMethodSupport * This,
            /* [out] */ HSTRING *className);
        
        HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )( 
            __x_ABI_CTestModel_CIMethodSupport * This,
            /* [out] */ TrustLevel *trustLevel);
        
        HRESULT ( STDMETHODCALLTYPE *VoidEmpty )( 
            __x_ABI_CTestModel_CIMethodSupport * This);
        
        HRESULT ( STDMETHODCALLTYPE *Void_Int )( 
            __x_ABI_CTestModel_CIMethodSupport * This,
            /* [in] */ INT32 value);
        
        HRESULT ( STDMETHODCALLTYPE *Void_Object )( 
            __x_ABI_CTestModel_CIMethodSupport * This,
            /* [in] */ __x_ABI_CMicrosoft_CPropertyModel_CIObject *pObject);
        
        HRESULT ( STDMETHODCALLTYPE *Void_Dictionary )( 
            __x_ABI_CTestModel_CIMethodSupport * This,
            /* [in] */ __x_ABI_CMicrosoft_CPropertyModel_CIDictionary *dictionary);
        
        HRESULT ( STDMETHODCALLTYPE *ReturnInt )( 
            __x_ABI_CTestModel_CIMethodSupport * This,
            /* [out][retval] */ INT32 *result);
        
        HRESULT ( STDMETHODCALLTYPE *ReturnInt_Int )( 
            __x_ABI_CTestModel_CIMethodSupport * This,
            /* [in] */ INT32 value,
            /* [out][retval] */ INT32 *result);
        
        HRESULT ( STDMETHODCALLTYPE *ReturnDictionary )( 
            __x_ABI_CTestModel_CIMethodSupport * This,
            /* [out][retval] */ __x_ABI_CMicrosoft_CPropertyModel_CIDictionary **result);
        
        HRESULT ( STDMETHODCALLTYPE *ReturnObjectAsync )( 
            __x_ABI_CTestModel_CIMethodSupport * This,
            /* [out][retval] */ __FIAsyncOperation_1_Microsoft__CPropertyModel__CIObject **result);
        
        HRESULT ( STDMETHODCALLTYPE *ReturnDictionaryAsync )( 
            __x_ABI_CTestModel_CIMethodSupport * This,
            /* [out][retval] */ __FIAsyncOperation_1_Microsoft__CPropertyModel__CIDictionary **result);
        
        HRESULT ( STDMETHODCALLTYPE *VoidAsync )( 
            __x_ABI_CTestModel_CIMethodSupport * This,
            /* [out][retval] */ __FIAsyncOperation_1_IInspectable **result);
        
        HRESULT ( STDMETHODCALLTYPE *VoidAsync_Int )( 
            __x_ABI_CTestModel_CIMethodSupport * This,
            /* [in] */ INT32 value,
            /* [out][retval] */ __FIAsyncOperation_1_int **result);
        
        HRESULT ( STDMETHODCALLTYPE *ReturnInt_Async )( 
            __x_ABI_CTestModel_CIMethodSupport * This,
            /* [out][retval] */ __FIAsyncOperation_1_int **result);
        
        HRESULT ( STDMETHODCALLTYPE *Void_ObjectAsync )( 
            __x_ABI_CTestModel_CIMethodSupport * This,
            /* [in] */ __x_ABI_CMicrosoft_CPropertyModel_CIObject *pObject,
            /* [out][retval] */ __FIAsyncOperation_1_IInspectable **result);
        
        HRESULT ( STDMETHODCALLTYPE *Void_DictionaryAsync )( 
            __x_ABI_CTestModel_CIMethodSupport * This,
            /* [in] */ __x_ABI_CMicrosoft_CPropertyModel_CIDictionary *dictionary,
            /* [out][retval] */ __FIAsyncOperation_1_IInspectable **result);
        
        END_INTERFACE
    } __x_ABI_CTestModel_CIMethodSupportVtbl;

    interface __x_ABI_CTestModel_CIMethodSupport
    {
        CONST_VTBL struct __x_ABI_CTestModel_CIMethodSupportVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define __x_ABI_CTestModel_CIMethodSupport_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __x_ABI_CTestModel_CIMethodSupport_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __x_ABI_CTestModel_CIMethodSupport_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __x_ABI_CTestModel_CIMethodSupport_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __x_ABI_CTestModel_CIMethodSupport_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __x_ABI_CTestModel_CIMethodSupport_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __x_ABI_CTestModel_CIMethodSupport_VoidEmpty(This)	\
    ( (This)->lpVtbl -> VoidEmpty(This) ) 

#define __x_ABI_CTestModel_CIMethodSupport_Void_Int(This,value)	\
    ( (This)->lpVtbl -> Void_Int(This,value) ) 

#define __x_ABI_CTestModel_CIMethodSupport_Void_Object(This,pObject)	\
    ( (This)->lpVtbl -> Void_Object(This,pObject) ) 

#define __x_ABI_CTestModel_CIMethodSupport_Void_Dictionary(This,dictionary)	\
    ( (This)->lpVtbl -> Void_Dictionary(This,dictionary) ) 

#define __x_ABI_CTestModel_CIMethodSupport_ReturnInt(This,result)	\
    ( (This)->lpVtbl -> ReturnInt(This,result) ) 

#define __x_ABI_CTestModel_CIMethodSupport_ReturnInt_Int(This,value,result)	\
    ( (This)->lpVtbl -> ReturnInt_Int(This,value,result) ) 

#define __x_ABI_CTestModel_CIMethodSupport_ReturnDictionary(This,result)	\
    ( (This)->lpVtbl -> ReturnDictionary(This,result) ) 

#define __x_ABI_CTestModel_CIMethodSupport_ReturnObjectAsync(This,result)	\
    ( (This)->lpVtbl -> ReturnObjectAsync(This,result) ) 

#define __x_ABI_CTestModel_CIMethodSupport_ReturnDictionaryAsync(This,result)	\
    ( (This)->lpVtbl -> ReturnDictionaryAsync(This,result) ) 

#define __x_ABI_CTestModel_CIMethodSupport_VoidAsync(This,result)	\
    ( (This)->lpVtbl -> VoidAsync(This,result) ) 

#define __x_ABI_CTestModel_CIMethodSupport_VoidAsync_Int(This,value,result)	\
    ( (This)->lpVtbl -> VoidAsync_Int(This,value,result) ) 

#define __x_ABI_CTestModel_CIMethodSupport_ReturnInt_Async(This,result)	\
    ( (This)->lpVtbl -> ReturnInt_Async(This,result) ) 

#define __x_ABI_CTestModel_CIMethodSupport_Void_ObjectAsync(This,pObject,result)	\
    ( (This)->lpVtbl -> Void_ObjectAsync(This,pObject,result) ) 

#define __x_ABI_CTestModel_CIMethodSupport_Void_DictionaryAsync(This,dictionary,result)	\
    ( (This)->lpVtbl -> Void_DictionaryAsync(This,dictionary,result) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* ____x_ABI_CTestModel_CIMethodSupport_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_TestModel_0000_0022 */
/* [local] */ 

/* [v1_enum] */ 
enum __x_ABI_CTestModel_CMethodSupportMethods
    {
        MethodSupportMethods_VoidEmpty	= 1,
        MethodSupportMethods_Void_Int	= 2,
        MethodSupportMethods_Void_Object	= 3,
        MethodSupportMethods_Void_Dictionary	= 4,
        MethodSupportMethods_ReturnInt	= 5,
        MethodSupportMethods_ReturnInt_Int	= 6,
        MethodSupportMethods_ReturnDictionary	= 7,
        MethodSupportMethods_ReturnObjectAsync	= 8,
        MethodSupportMethods_ReturnDictionaryAsync	= 9,
        MethodSupportMethods_VoidAsync	= 10,
        MethodSupportMethods_VoidAsync_Int	= 11,
        MethodSupportMethods_ReturnInt_Async	= 12,
        MethodSupportMethods_Void_ObjectAsync	= 13,
        MethodSupportMethods_Void_DictionaryAsync	= 14
    } ;
typedef enum __x_ABI_CTestModel_CMethodSupportMethods __x_ABI_CTestModel_CMethodSupportMethods;



extern RPC_IF_HANDLE __MIDL_itf_TestModel_0000_0022_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_TestModel_0000_0022_v0_0_s_ifspec;

#ifndef ____x_ABI_CTestModel_CITestFactory_INTERFACE_DEFINED__
#define ____x_ABI_CTestModel_CITestFactory_INTERFACE_DEFINED__

/* interface __x_ABI_CTestModel_CITestFactory */
/* [uuid][object] */ 


EXTERN_C const IID IID___x_ABI_CTestModel_CITestFactory;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("aa8b56f6-b3ad-9a0e-e1f2-7051271b5125")
    __x_ABI_CTestModel_CITestFactory : public IInspectable
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE CreateTypeProperties( 
            /* [out][retval] */ __x_ABI_CTestModel_CITypeProperties **result) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateMethodSupport( 
            /* [in] */ IInspectable *workingDispatcher,
            /* [out][retval] */ __x_ABI_CTestModel_CIMethodSupport **result) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateTypePropertiesTest( 
            /* [in] */ IInspectable *workingDispatcher,
            /* [out][retval] */ __x_ABI_CTestModel_CITypePropertiesTest **result) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct __x_ABI_CTestModel_CITestFactoryVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __x_ABI_CTestModel_CITestFactory * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __x_ABI_CTestModel_CITestFactory * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __x_ABI_CTestModel_CITestFactory * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetIids )( 
            __x_ABI_CTestModel_CITestFactory * This,
            /* [out] */ ULONG *iidCount,
            /* [size_is][size_is][out] */ IID **iids);
        
        HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )( 
            __x_ABI_CTestModel_CITestFactory * This,
            /* [out] */ HSTRING *className);
        
        HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )( 
            __x_ABI_CTestModel_CITestFactory * This,
            /* [out] */ TrustLevel *trustLevel);
        
        HRESULT ( STDMETHODCALLTYPE *CreateTypeProperties )( 
            __x_ABI_CTestModel_CITestFactory * This,
            /* [out][retval] */ __x_ABI_CTestModel_CITypeProperties **result);
        
        HRESULT ( STDMETHODCALLTYPE *CreateMethodSupport )( 
            __x_ABI_CTestModel_CITestFactory * This,
            /* [in] */ IInspectable *workingDispatcher,
            /* [out][retval] */ __x_ABI_CTestModel_CIMethodSupport **result);
        
        HRESULT ( STDMETHODCALLTYPE *CreateTypePropertiesTest )( 
            __x_ABI_CTestModel_CITestFactory * This,
            /* [in] */ IInspectable *workingDispatcher,
            /* [out][retval] */ __x_ABI_CTestModel_CITypePropertiesTest **result);
        
        END_INTERFACE
    } __x_ABI_CTestModel_CITestFactoryVtbl;

    interface __x_ABI_CTestModel_CITestFactory
    {
        CONST_VTBL struct __x_ABI_CTestModel_CITestFactoryVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define __x_ABI_CTestModel_CITestFactory_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __x_ABI_CTestModel_CITestFactory_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __x_ABI_CTestModel_CITestFactory_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __x_ABI_CTestModel_CITestFactory_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __x_ABI_CTestModel_CITestFactory_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __x_ABI_CTestModel_CITestFactory_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __x_ABI_CTestModel_CITestFactory_CreateTypeProperties(This,result)	\
    ( (This)->lpVtbl -> CreateTypeProperties(This,result) ) 

#define __x_ABI_CTestModel_CITestFactory_CreateMethodSupport(This,workingDispatcher,result)	\
    ( (This)->lpVtbl -> CreateMethodSupport(This,workingDispatcher,result) ) 

#define __x_ABI_CTestModel_CITestFactory_CreateTypePropertiesTest(This,workingDispatcher,result)	\
    ( (This)->lpVtbl -> CreateTypePropertiesTest(This,workingDispatcher,result) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* ____x_ABI_CTestModel_CITestFactory_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_TestModel_0000_0023 */
/* [local] */ 

/* [v1_enum] */ 
enum __x_ABI_CTestModel_CTestFactoryMethods
    {
        TestFactoryMethods_CreateTypeProperties	= 1,
        TestFactoryMethods_CreateMethodSupport	= 2,
        TestFactoryMethods_CreateTypePropertiesTest	= 3
    } ;
typedef enum __x_ABI_CTestModel_CTestFactoryMethods __x_ABI_CTestModel_CTestFactoryMethods;



extern RPC_IF_HANDLE __MIDL_itf_TestModel_0000_0023_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_TestModel_0000_0023_v0_0_s_ifspec;

#ifndef ____x_ABI_CTestModel_CICommand__void__string_INTERFACE_DEFINED__
#define ____x_ABI_CTestModel_CICommand__void__string_INTERFACE_DEFINED__

/* interface __x_ABI_CTestModel_CICommand__void__string */
/* [uuid][object] */ 


EXTERN_C const IID IID___x_ABI_CTestModel_CICommand__void__string;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6c23aa96-3070-ac82-826d-d00bd82b8a5a")
    __x_ABI_CTestModel_CICommand__void__string : public IInspectable
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Execute( 
            /* [out][retval] */ HSTRING *pValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CanExecute( 
            /* [out][retval] */ boolean *value) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct __x_ABI_CTestModel_CICommand__void__stringVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __x_ABI_CTestModel_CICommand__void__string * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __x_ABI_CTestModel_CICommand__void__string * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __x_ABI_CTestModel_CICommand__void__string * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetIids )( 
            __x_ABI_CTestModel_CICommand__void__string * This,
            /* [out] */ ULONG *iidCount,
            /* [size_is][size_is][out] */ IID **iids);
        
        HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )( 
            __x_ABI_CTestModel_CICommand__void__string * This,
            /* [out] */ HSTRING *className);
        
        HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )( 
            __x_ABI_CTestModel_CICommand__void__string * This,
            /* [out] */ TrustLevel *trustLevel);
        
        HRESULT ( STDMETHODCALLTYPE *Execute )( 
            __x_ABI_CTestModel_CICommand__void__string * This,
            /* [out][retval] */ HSTRING *pValue);
        
        HRESULT ( STDMETHODCALLTYPE *CanExecute )( 
            __x_ABI_CTestModel_CICommand__void__string * This,
            /* [out][retval] */ boolean *value);
        
        END_INTERFACE
    } __x_ABI_CTestModel_CICommand__void__stringVtbl;

    interface __x_ABI_CTestModel_CICommand__void__string
    {
        CONST_VTBL struct __x_ABI_CTestModel_CICommand__void__stringVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define __x_ABI_CTestModel_CICommand__void__string_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __x_ABI_CTestModel_CICommand__void__string_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __x_ABI_CTestModel_CICommand__void__string_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __x_ABI_CTestModel_CICommand__void__string_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __x_ABI_CTestModel_CICommand__void__string_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __x_ABI_CTestModel_CICommand__void__string_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __x_ABI_CTestModel_CICommand__void__string_Execute(This,pValue)	\
    ( (This)->lpVtbl -> Execute(This,pValue) ) 

#define __x_ABI_CTestModel_CICommand__void__string_CanExecute(This,value)	\
    ( (This)->lpVtbl -> CanExecute(This,value) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* ____x_ABI_CTestModel_CICommand__void__string_INTERFACE_DEFINED__ */


#ifndef ____x_ABI_CTestModel_CICommand__void__string__async_INTERFACE_DEFINED__
#define ____x_ABI_CTestModel_CICommand__void__string__async_INTERFACE_DEFINED__

/* interface __x_ABI_CTestModel_CICommand__void__string__async */
/* [uuid][object] */ 


EXTERN_C const IID IID___x_ABI_CTestModel_CICommand__void__string__async;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("64897e1d-1ad5-ee0b-490d-22d9eac37953")
    __x_ABI_CTestModel_CICommand__void__string__async : public IInspectable
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Execute( 
            /* [out][retval] */ __FIAsyncOperation_1_HSTRING **pValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CanExecute( 
            /* [out][retval] */ boolean *value) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct __x_ABI_CTestModel_CICommand__void__string__asyncVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __x_ABI_CTestModel_CICommand__void__string__async * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __x_ABI_CTestModel_CICommand__void__string__async * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __x_ABI_CTestModel_CICommand__void__string__async * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetIids )( 
            __x_ABI_CTestModel_CICommand__void__string__async * This,
            /* [out] */ ULONG *iidCount,
            /* [size_is][size_is][out] */ IID **iids);
        
        HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )( 
            __x_ABI_CTestModel_CICommand__void__string__async * This,
            /* [out] */ HSTRING *className);
        
        HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )( 
            __x_ABI_CTestModel_CICommand__void__string__async * This,
            /* [out] */ TrustLevel *trustLevel);
        
        HRESULT ( STDMETHODCALLTYPE *Execute )( 
            __x_ABI_CTestModel_CICommand__void__string__async * This,
            /* [out][retval] */ __FIAsyncOperation_1_HSTRING **pValue);
        
        HRESULT ( STDMETHODCALLTYPE *CanExecute )( 
            __x_ABI_CTestModel_CICommand__void__string__async * This,
            /* [out][retval] */ boolean *value);
        
        END_INTERFACE
    } __x_ABI_CTestModel_CICommand__void__string__asyncVtbl;

    interface __x_ABI_CTestModel_CICommand__void__string__async
    {
        CONST_VTBL struct __x_ABI_CTestModel_CICommand__void__string__asyncVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define __x_ABI_CTestModel_CICommand__void__string__async_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __x_ABI_CTestModel_CICommand__void__string__async_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __x_ABI_CTestModel_CICommand__void__string__async_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __x_ABI_CTestModel_CICommand__void__string__async_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __x_ABI_CTestModel_CICommand__void__string__async_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __x_ABI_CTestModel_CICommand__void__string__async_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __x_ABI_CTestModel_CICommand__void__string__async_Execute(This,pValue)	\
    ( (This)->lpVtbl -> Execute(This,pValue) ) 

#define __x_ABI_CTestModel_CICommand__void__string__async_CanExecute(This,value)	\
    ( (This)->lpVtbl -> CanExecute(This,value) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* ____x_ABI_CTestModel_CICommand__void__string__async_INTERFACE_DEFINED__ */


#ifndef ____x_ABI_CTestModel_CIClassStatics_INTERFACE_DEFINED__
#define ____x_ABI_CTestModel_CIClassStatics_INTERFACE_DEFINED__

/* interface __x_ABI_CTestModel_CIClassStatics */
/* [uuid][object] */ 


EXTERN_C const IID IID___x_ABI_CTestModel_CIClassStatics;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1FCEB2D2-179D-4044-A680-73CA4D377E1F")
    __x_ABI_CTestModel_CIClassStatics : public IInspectable
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Register( void) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct __x_ABI_CTestModel_CIClassStaticsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            __x_ABI_CTestModel_CIClassStatics * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            __x_ABI_CTestModel_CIClassStatics * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            __x_ABI_CTestModel_CIClassStatics * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetIids )( 
            __x_ABI_CTestModel_CIClassStatics * This,
            /* [out] */ ULONG *iidCount,
            /* [size_is][size_is][out] */ IID **iids);
        
        HRESULT ( STDMETHODCALLTYPE *GetRuntimeClassName )( 
            __x_ABI_CTestModel_CIClassStatics * This,
            /* [out] */ HSTRING *className);
        
        HRESULT ( STDMETHODCALLTYPE *GetTrustLevel )( 
            __x_ABI_CTestModel_CIClassStatics * This,
            /* [out] */ TrustLevel *trustLevel);
        
        HRESULT ( STDMETHODCALLTYPE *Register )( 
            __x_ABI_CTestModel_CIClassStatics * This);
        
        END_INTERFACE
    } __x_ABI_CTestModel_CIClassStaticsVtbl;

    interface __x_ABI_CTestModel_CIClassStatics
    {
        CONST_VTBL struct __x_ABI_CTestModel_CIClassStaticsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define __x_ABI_CTestModel_CIClassStatics_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define __x_ABI_CTestModel_CIClassStatics_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define __x_ABI_CTestModel_CIClassStatics_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define __x_ABI_CTestModel_CIClassStatics_GetIids(This,iidCount,iids)	\
    ( (This)->lpVtbl -> GetIids(This,iidCount,iids) ) 

#define __x_ABI_CTestModel_CIClassStatics_GetRuntimeClassName(This,className)	\
    ( (This)->lpVtbl -> GetRuntimeClassName(This,className) ) 

#define __x_ABI_CTestModel_CIClassStatics_GetTrustLevel(This,trustLevel)	\
    ( (This)->lpVtbl -> GetTrustLevel(This,trustLevel) ) 


#define __x_ABI_CTestModel_CIClassStatics_Register(This)	\
    ( (This)->lpVtbl -> Register(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* ____x_ABI_CTestModel_CIClassStatics_INTERFACE_DEFINED__ */


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


