/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:library_util.h
****/
#pragma once

#include "factory_util.h"

#include <foundation/com_ptr.h>
#include <foundation/macros.h>
#include <foundation/ctl/com_library.h>

#include <foundation/library/com_class_ptr.h>
#include <foundation/library/async_operation_class_ptr.h>


// include public ptr classes 
#include <foundation/async_operation_ptr.h>
#include <pmod/observable_collection_ptr.h>
#include <pmod/observable_object_ptr.h>
#include <pmod/command_ptr.h>
#include <pmod/binding_ptr.h>

// our class wrapper classes
#include "observable_object_class_wrapper.h"
#include "observable_collection_class_wrapper.h"
#include "command_class_wrapper.h"
#include "collection_viewmodel_class_wrapper.h"

namespace pmod
{
    namespace library {
        // CreateSingleSourceModelCallbackAdapter Class
        class CreateSingleSourceModelCallbackAdapter :
            public foundation::_CallbackDelegateAdapterBase
            <
            CreateSingleSourceModelCallbackAdapter,
            CreateSingleSourceModelCallback,
            ICreateSingleSourceModelDelegate,
            &IID_ICreateSingleSourceModelDelegate
            >
        {
        protected:
            STDMETHOD(Invoke)(_In_ foundation::IObject *pModel, ISourceModel **ppSourceModel)
            {
                return (*this->GetCallback())(pModel, ppSourceModel);
            }
        };
#if _HAS_CPP0X
        typedef foundation::FuncInvoke2Delegate<IExecuteDelegate, foundation::IInspectable*, foundation::IInspectable **>  ExecuteDelegateFunc;
        typedef foundation::FuncInvoke2Delegate<ICanExecuteDelegate, foundation::IInspectable*, bool *>        CanExecuteDelegateFunc;
#endif

        class _ObservableObjectClassHostWrapper :
            public foundation::_IClassInterfaceHost<IObservableObjectClass>,
            public _ObservableObjectClassWrapper<_ObservableObjectClassHostWrapper>
        {};

        class _ObservableCollectionClassHostWrapper :
            public foundation::_IClassInterfaceHost<IObservableCollectionClass>,
            public _ObservableCollectionClassWrapper<_ObservableCollectionClassHostWrapper>
        {};

        class _CommandClassHostWrapper :
            public foundation::_IClassInterfaceHost<ICommandClass>,
            public _CommandClassWrapper<_CommandClassHostWrapper>
        {};

        // Model Class Ptr
        typedef  foundation::_ComClassPtr<IObservableObject, _ObservableObjectClassHostWrapper, ObservableObjectPtr<>> ObservableObjectClassPtr;
        typedef  foundation::_ComClassPtr<ICommand, _CommandClassHostWrapper, CommandPtr> command_model_class_ptr;

        typedef foundation::_ComClassPtr<IObservableCollection_Inspectable, _ObservableCollectionClassHostWrapper, ObservableCollectionInspectablePtr> ObservableCollectionClassPtr;
        typedef foundation::_ComClassPtr<IObservableCollection_Inspectable, _ObservableCollectionClassHostWrapper, ObservableListInspectablePtr> ObservableListClassPtr;
        // type safe specializations
        typedef foundation::_ComClassPtr<IObservableCollection_String, _ObservableCollectionClassHostWrapper, ObservableCollectionStringPtr> ObservableCollectionStringClassPtr;
        typedef foundation::_ComClassPtr<IObservableCollection_Int32, _ObservableCollectionClassHostWrapper, ObservableCollectionInt32Ptr> ObservableCollectionInt32ClassPtr;
        typedef foundation::_ComClassPtr<IObservableCollection_UInt32, _ObservableCollectionClassHostWrapper, ObservableCollectionUInt32Ptr> ObservableCollectionUInt32ClassPtr;
        typedef foundation::_ComClassPtr<IObservableCollection_Int16, _ObservableCollectionClassHostWrapper, ObservableCollectionInt16Ptr> ObservableCollectionInt16ClassPtr;
        typedef foundation::_ComClassPtr<IObservableCollection_UInt16, _ObservableCollectionClassHostWrapper, ObservableCollectionUInt16Ptr> ObservableCollectionUInt16ClassPtr;
        typedef foundation::_ComClassPtr<IObservableCollection_Int64, _ObservableCollectionClassHostWrapper, ObservableCollectionInt64Ptr> ObservableCollectionInt64ClassPtr;
        typedef foundation::_ComClassPtr<IObservableCollection_UInt64, _ObservableCollectionClassHostWrapper, ObservableCollectionUInt64Ptr> ObservableCollectionUInt64ClassPtr;
        typedef foundation::_ComClassPtr<IObservableCollection_UInt8, _ObservableCollectionClassHostWrapper, ObservableCollectionUInt8Ptr> ObservableCollectionUInt8ClassPtr;
        typedef foundation::_ComClassPtr<IObservableCollection_Boolean, _ObservableCollectionClassHostWrapper, ObservableCollectionBooleanPtr> ObservableCollectionBooleanClassPtr;

        // View Model Class Ptr
        class _CollectionViewModelClassHostWrapper :
            public _ObservableCollectionClassHostWrapper,
            public _Collection_ViewModelClassWrapper<_CollectionViewModelClassHostWrapper>
        {
        public:
            void ReleaseHostWrapper()
            {
                _viewmodelClassPtr.Release();
            }

            ICollectionViewModelClass *GetCollectionViewModelClass()
            {
                if (_viewmodelClassPtr == nullptr)
                {
                    foundation::QueryInterface(
                        this->GetClassInterface(),
                        this->_viewmodelClassPtr.GetAddressOf());
                }
                return _viewmodelClassPtr;
            }
        private:
            foundation::ComPtr<ICollectionViewModelClass> _viewmodelClassPtr;
        };

        typedef foundation::_ComClassPtr<IObservableCollection_Inspectable, _CollectionViewModelClassHostWrapper, ObservableCollectionInspectablePtr> CollectionViewModelClassPtr;

        namespace library
        {
#if _HAS_CPP0X
            template <class T, class _Func>
            static HRESULT CreateCommand(
                _In_ const IID&                 iidType,
                _In_  CommandOptions       options,
                _In_ const _Func& on_execute_func,
                _COM_Outptr_ T **ppNewInstance)
            {
                IFCPTR_ASSERT(ppNewInstance);
                *ppNewInstance = nullptr;

                command_model_class_ptr  command_model_class_ptr;

                IFR_ASSERT(CreateCommand(
                    iidType,
                    options,
                    command_model_class_ptr.GetAddressOf()));

                foundation::ComPtr<ExecuteDelegateFunc> execute_func_ptr;
                IFR_ASSERT(CreateFuncDelegate(on_execute_func, execute_func_ptr.GetAddressOf()));
                IFR_ASSERT(command_model_class_ptr.SetExecuteDelegate(execute_func_ptr));
                IFR_ASSERT(foundation::QueryInterface(command_model_class_ptr, ppNewInstance))
                    return S_OK;
            }
#endif
        }
    }
}
