/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:Dispatcher.cpp
****/
#include "Dispatcher.h"

#include "JniCoreUtils.h"
#include "JVMEnv.h"
#include "JObject.h"

#include <foundation/library/dispatcher_util.h>
#include <foundation/library/async_operation_impl.h>
#include <foundation/ctl/com_library.h>
#include <assert.h>

using namespace foundation;
using namespace foundation::library;

class CDelegateDispatchableAdapter :
    public foundation::ctl::ComInspectableBase
{
public:
    HRESULT _Initialize(
        IDelegateDispatchable *pDelegateDispatchable)
    {
        _dispatchable_ptr = pDelegateDispatchable;
        CreateAsyncOperationImpl(
            IAsyncOperation::GetIID(),
            _asyncOperationPtr.GetAddressOf());
        return S_OK;
    }

    HRESULT _InvokeAndComplete()
    {
        HRESULT hr = _dispatchable_ptr->Invoke();
        _asyncOperationPtr->_SetCompleted(hr, nullptr);
        return S_OK;
    }
    AsyncOperationImpl *get_async_action() { return _asyncOperationPtr; }
private:
    ComPtr<AsyncOperationImpl> _asyncOperationPtr;
    foundation::ComPtr<IDelegateDispatchable> _dispatchable_ptr;
};

class CDispatcherAdapter :
    public foundation::ctl::ComInspectableBase,
    public IDispatcher
{
public:
    HRESULT _Initialize(
        JNIEnv *env,
        jobject javaSinkObj)
    {
        m_javaSinkObj = javaSinkObj;
        jclass javaSinkClass = env->GetObjectClass(m_javaSinkObj);
        assert(javaSinkClass);

        m_javaSinkHasThreadAccessMethodId = env->GetMethodID(
            javaSinkClass, 
            "hasThreadAccessCallback", 
            "()Z");
        assert(m_javaSinkHasThreadAccessMethodId);
        m_javaSinkHasRunAsyncMethodId = env->GetMethodID(
            javaSinkClass,
            "runAsyncCallback",
            "(J)V");
        assert(m_javaSinkHasRunAsyncMethodId);
        return S_OK;
    }
protected:
    HRESULT QueryInterfaceImpl(REFIID iid, foundation::IUnknown **ppInterface) override
    {
        if (iid == IDispatcher::GetIID())
        {
            *ppInterface = static_cast<IDispatcher *>(this);
        }
        else
        {
            return foundation::ctl::ComInspectableBase::QueryInterfaceImpl(iid, ppInterface);
        }
        return S_OK;
    }

    STDMETHOD(HasThreadAccess)(bool *pValue) override
    {
        JNIEnv* env = nullptr;
        if (Java::JVMEnv::attachCurrentJNIEnv(&env) != JNI_OK)
        {
            return E_UNEXPECTED;
        }
        jboolean result = env->CallBooleanMethod(m_javaSinkObj, m_javaSinkHasThreadAccessMethodId);
        *pValue = result ? true : false;
        return S_OK;
    }

    STDMETHOD(RunAsync)(
        IDelegateDispatchable *pDelegateDispatchable,
        IAsyncOperation **ppAsyncAction)
    {
        JNIEnv* env = nullptr;
        if (Java::JVMEnv::attachCurrentJNIEnv(&env) != JNI_OK)
        {
            return E_UNEXPECTED;
        }

        CDelegateDispatchableAdapter *pDelegateDispatchableAdapter;
        foundation::ctl::inspectable::CreateInstanceWithInitialize(
            &pDelegateDispatchableAdapter,
            pDelegateDispatchable);

        *ppAsyncAction = pDelegateDispatchableAdapter->get_async_action();
        (*ppAsyncAction)->AddRef();

        // callback to Java
        env->CallVoidMethod(
            m_javaSinkObj,
            m_javaSinkHasRunAsyncMethodId,
            (jlong)pDelegateDispatchableAdapter->CastToInspectable());

        return S_OK;
    }

private:
    Java::JObject     m_javaSinkObj;
    jmethodID         m_javaSinkHasThreadAccessMethodId;
    jmethodID         m_javaSinkHasRunAsyncMethodId;
};


JNIEXPORT void JNICALL Java_com_microsoft_pmod_Dispatcher_createDispatcherAdapter
(JNIEnv *env, jobject _this)
{
    CDispatcherAdapter *pDispatcherAdapter;
    foundation::ctl::inspectable::CreateInstanceWithInitialize(&pDispatcherAdapter, env, _this);
    initInterface(env, _this, pDispatcherAdapter->CastToInspectable());
}

JNIEXPORT void JNICALL Java_com_microsoft_pmod_Dispatcher_invokeDispatchable
(JNIEnv *env, jclass jclass,jlong dispatchable)
{
    CDelegateDispatchableAdapter *pDelegateDispatchableAdapter =
        static_cast<CDelegateDispatchableAdapter *>(reinterpret_cast<foundation::IInspectable *>(dispatchable));
    pDelegateDispatchableAdapter->_InvokeAndComplete();
    pDelegateDispatchableAdapter->Release();
}
