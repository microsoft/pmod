/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:object_node_class_wrapper.h
****/
#pragma once

#include <foundation/library/interfaces/dispatcher.h>
#include <foundation/library/object_class_wrapper.h>
#include <foundation/library/dispatcher_util.h>
#include "interfaces/object_node_class.h"

namespace pmod
{
    namespace library {

        template <class T, class TClassInterface>
        class _ObjectNodeClassWrapper :
            public foundation::_ObjectClassWrapper<T, TClassInterface>
        {
        public:
            bool IsInModelChanges()
            {
                bool isInModelChanges;
                this->GetClassInterfaceInternal()->GetIsInModelChanges(&isInModelChanges);
                return isInModelChanges;
            }

            HRESULT BeginModelChanges(BatchModelChangesOptions options = BatchModelChangesOptions::None)
            {
                return this->GetClassInterfaceInternal()->BeginModelChanges(options);
            }

            HRESULT EndModelChanges(EndModelChangesOptions options = EndModelChangesOptions::None)
            {
                return this->GetClassInterfaceInternal()->EndModelChanges(options);
            }

            HRESULT SetLocalWorkingDispatcher(_In_ foundation::library::IDispatcher *pDispatcher)
            {
                return this->GetClassInterfaceInternal()->SetLocalWorkingDispatcher(pDispatcher);
            }

            HRESULT GetLocalWorkingDispatcher(_Out_ foundation::library::IDispatcher **ppDispatcher)
            {
                return this->GetClassInterfaceInternal()->GetLocalWorkingDispatcher(ppDispatcher);
            }

            foundation::library::IDispatcher *GetWorkingDispatcher()
            {
                foundation::ComPtr<foundation::library::IDispatcher> dispatcher_ptr;
                IGNOREHR(this->GetClassInterfaceInternal()->GetWorkingDispatcher(dispatcher_ptr.GetAddressOf()));
                return dispatcher_ptr;
            }

            HRESULT ClearParentObject()
            {
                return SetParentObject(static_cast<foundation::IObject *>(nullptr));
            }

            HRESULT SetParentObject(_In_opt_ foundation::IObject *pParent)
            {
                return this->GetClassInterfaceInternal()->SetParentInternal(pParent);
            }

            HRESULT SetParentObject(_In_ foundation::IUnknown *pUnkParent)
            {
                IFCPTR_ASSERT(pUnkParent);
                foundation::IObject* pObject = nullptr;
                IFR_ASSERT(foundation::QueryWeakReference(pUnkParent, &pObject));
                return SetParentObject(pObject);
            }

            HRESULT SetChildObject(_In_ foundation::IUnknown *pUnkChild)
            {
                IFCPTR_ASSERT(pUnkChild);

                IObjectNodeClass* pModelClassChild = nullptr;
                IFR_ASSERT(foundation::QueryWeakReference(pUnkChild, &pModelClassChild));
                return pModelClassChild->SetParentInternal(this->template GetClassInterfaceInternalAs<foundation::IObject>());
            }

            template <class TAncestorInterface>
            HRESULT GetAncestorClass(_COM_Outptr_result_maybenull_ TAncestorInterface **ppAncestor)
            {
                return foundation::GetObjectAncestor(
                    this->template GetClassInterfaceInternalAs<foundation::IObject>(),
                    ppAncestor);
            }

            template <class class_t, class TAncestorInterface>
            class_t *GetAncestorClass()
            {
                foundation::ComPtr<TAncestorInterface> spAncestor;
                IGNOREHR(this->GetAncestorClass(spAncestor.GetAddressOf()));
                return static_cast<class_t *>(spAncestor.Get());
            }

            template <class TService>
            HRESULT QueryServiceClass(
                REFIID guidService,
                _COM_Outptr_ TService** ppService)
            {
                return foundation::QueryObjectService(
                    this->template GetClassInterfaceInternalAs<foundation::IObject>(),
                    guidService,
                    ppService);
            }

            template <class _Func>
            HRESULT RunWorkingDispatcherSync(
                _Func func)
            {
                return foundation::RunIfSync(this->GetWorkingDispatcher(), func);
            }

            template <class _Func>
            HRESULT RunWorkingDispatcherAsync(
                _Func func,
                /*_COM_Outptr_opt_result_maybenull_*/ foundation::IAsyncOperation **ppAsyncAction = nullptr,
                bool has_thread_access_option = true)
            {
                return foundation::RunIfAsync(
                    this->GetWorkingDispatcher(),
                    func,
                    ppAsyncAction,
                    has_thread_access_option);
            }

        protected:
            HRESULT TrySetChildObject(_In_ foundation::IObject* pChild)
            {
                IFCPTR_ASSERT(pChild);

                foundation::ComPtr<foundation::IObject> parent_model_ptr;
                IFR_ASSERT(foundation::GetObjectParent(pChild, parent_model_ptr.GetAddressOf()));

                if (!!parent_model_ptr)
                {
                    return S_FALSE;
                }

                return SetChildObject(pChild);
            }
        };
    }

}
