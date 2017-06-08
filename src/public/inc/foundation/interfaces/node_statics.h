/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:node_statics.h
****/
#pragma once

#include <foundation/com_interfaces.h>
#include "node.h"

namespace foundation
{
    // INamespaceStatics Interface
    BEGIN_DECLARE_INTERFACE(INodeStatics, foundation::IUnknown, FOUNDATION_API)
		STDMETHOD(GetObjectParent)(
			_In_ foundation::IObject *pObject,
			_COM_Outptr_result_maybenull_ foundation::IObject **ppParent) = 0;
		STDMETHOD(GetObjectAncestor)(
			_In_ foundation::IObject *pObject,
			REFIID iidAncestorType,
			_COM_Outptr_result_maybenull_ foundation::IObject **ppAncestor) = 0;
		STDMETHOD(GetObjectRoot)(
			_In_ foundation::IObject *pObject,
			_COM_Outptr_ foundation::IObject **ppRoot) = 0;
	END_DECLARE_INTERFACE()
}

