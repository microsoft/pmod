/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:object_node_class.h
****/
#pragma once

#include <pmod/pmod_api.h>
#include <foundation/library/interfaces/object_class.h>
#include <foundation/library/interfaces/dispatcher.h>
#include <foundation/library/interfaces/node_impl.h>
#include "../pmod_lib_api.h"

namespace pmod
{
    namespace library
    {
        enum class BatchModelChangesOptions
        {
            None = 0x00,
            Transaction = 0x04,
        };

        enum class EndModelChangesOptions
        {
            None = 0x00,
            Abort = 0x01,
        };

        // The Model Class Interface Base
        BEGIN_DECLARE_INTERFACE(IObjectNodeClass, foundation::library::IObjectClass, PMOD_LIB_API)
            // Initialize at runtime
            STDMETHOD(RuntimeInitialize)() = 0;
            // Begin/End Model Changes
            STDMETHOD(BeginModelChanges)(BatchModelChangesOptions options) = 0;
            STDMETHOD(EndModelChanges)(EndModelChangesOptions options) = 0;
            STDMETHOD(GetIsInModelChanges)(_Out_ bool *pValue) = 0;
            // Get/Set Local Working Dispatcher property
            STDMETHOD(GetLocalWorkingDispatcher)(_COM_Outptr_result_maybenull_ foundation::library::IDispatcher **ppWorkingDispatcher) = 0;
            STDMETHOD(SetLocalWorkingDispatcher)(_In_opt_ foundation::library::IDispatcher *pWorkingDispatcher) = 0;
            // Working Dispatcher property
            STDMETHOD(GetWorkingDispatcher)(_COM_Outptr_result_maybenull_ foundation::library::IDispatcher **ppWorkingDispatcher) = 0;

            STDMETHOD(SetParentInternal)(foundation::IObject *pParent) = 0;

            // Private methods used by Impl
            STDMETHOD(_GetParent)(foundation::library::_INode_impl *pParent,foundation::IObject **ppParent) = 0;
            STDMETHOD(_SetParent)(foundation::library::_INode_impl *pThisNode, foundation::library::_INode_impl *pParent) = 0;
            STDMETHOD(_OnParentChanged)(_In_opt_ foundation::library::_INode_impl *pParent) = 0;
            STDMETHOD(_OnAncestorChanged)(
                _In_ foundation::library::_INode_impl *pChild, 
                _In_opt_ foundation::library::_INode_impl *pParent) = 0;

        END_DECLARE_INTERFACE()

    }
}

