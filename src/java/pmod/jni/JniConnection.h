/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:JniConnection.h
****/
#ifndef _JNI_CONNECTION_DEFINED
#define _JNI_CONNECTION_DEFINED

#include <jni.h>
#include <pmod/event_connection.h>
#include "JVMEnv.h"
#include "JCallbackBase.h"

template 
<
    class TSource,
    class TDelegate,
    class TEventArgs,
    class TListenerBase,
    class JavaEventArgsName
>
class _TListenerProxy :
    public foundation::ctl::ComBase,
    protected JCallbackBase
{
public:
    template <class T>
    static foundation::IUnknown *CreateInstance(
        JNIEnv *env,
        jobject javaSinkObj,
        TSource *pSource)
    {
        foundation::ctl::ComObject<T> *pT;
        foundation::ctl::ComObject<T>::CreateInstance(&pT);
        IGNOREHR(pT->_Initialize(env,javaSinkObj,pSource));
        return pT;
    }
protected:
    _TListenerProxy():
        m_pSource(nullptr),
        m_pEventHandler(nullptr),
        m_JavaEventArgsClass(nullptr)
    {}

    virtual ~_TListenerProxy()
    {
        if (m_JavaEventArgsClass)
        {
            JNIEnv* env = nullptr;
            if (Java::JVMEnv::attachCurrentJNIEnv(&env) == JNI_OK)
            {
                env->DeleteGlobalRef(m_JavaEventArgsClass);
            }
        }
        if(m_pEventHandler)
        {
            m_pEventHandler->SetCallback(nullptr);
            if(m_pSource)
            {
                Detach();
            }
            foundation::ctl::ReleaseInterface(m_pEventHandler);
        }
    }

private:
    // Invoke delegate callback from 'Core' object
    HRESULT InvokeInternal(foundation::IUnknown *pSender, TEventArgs *pEventArgs)
    {
        JNIEnv* env = nullptr;
        if (Java::JVMEnv::attachCurrentJNIEnv(&env) != JNI_OK)
        {
            return E_UNEXPECTED;
        }
        foundation_assert(env);
        // call the method
        jobject javaEventArgs = createInspectableObject(env, m_JavaEventArgsClass, pEventArgs);

        InvokeCallback(env, javaEventArgs);

        return S_OK;
    }

    HRESULT _Initialize(
        JNIEnv *env,
        jobject javaSinkObj,
        TSource *pSource)
    {
        foundation_assert(env);

        setJObject(
            env, 
            javaSinkObj, 
            "invokeCallback", 
            "(Lcom/microsoft/pmod/EventArgs;)V");
        m_pSource = pSource;

        jclass tmp = env->FindClass(JavaEventArgsName::GetEventArgsClassName());
        m_JavaEventArgsClass = (jclass)env->NewGlobalRef(tmp);
        return attach();
    }

private:
    typedef _TListenerProxy<TSource, TDelegate, TEventArgs, TListenerBase, JavaEventArgsName> _TypeThis;
    
    typedef foundation::EventHandlerDelegateBaseEx<_TypeThis,TDelegate,TEventArgs> _EventHandlerDelegateType;
    // attach to 'Core' object
    HRESULT attach()
    {
        foundation_assert(m_pSource);
        foundation_assert(m_pEventHandler == nullptr);
        IFR_ASSERT(_EventHandlerDelegateType::CreateDelegateInstance(
            this,
            &_TypeThis::InvokeInternal,
            this,
            &m_pEventHandler));
        IFR_ASSERT(TListenerBase::AttachEventHandler(m_pSource,m_pEventHandler));
        return S_OK;
    }

    // detach from 'Core' object
    HRESULT Detach()
    {
        foundation_assert(m_pSource);
        foundation_assert(m_pEventHandler);

        IFR_ASSERT(TListenerBase::DetachEventHandler(m_pSource,m_pEventHandler));
        return S_OK;
    }
private:
    TSource                     *m_pSource;
    _EventHandlerDelegateType   *m_pEventHandler;
    jclass                      m_JavaEventArgsClass;
};

#endif
