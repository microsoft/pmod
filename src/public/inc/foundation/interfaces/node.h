/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:node.h
****/
#pragma once

#include <foundation/com_interfaces.h>
#include <foundation/foundation_api.h>
#include "object.h"

namespace foundation {

	////////////////////// INode declare ////////////////////////////
	template <class T, const IID& iidType>
	struct INode_impl : public IInspectable
	{
		typedef T _Node_Type;
		static const IID& GetIID() { return iidType; }

		STDMETHOD(GetParent)(_COM_Outptr_result_maybenull_ T **ppParent) = 0;
		STDMETHOD(GetChilds)(UINT32 *size, T ***ppChilds) = 0;
	};

	BEGIN_DECLARE_IID(_IObjectNode, FOUNDATION_API);
	typedef INode_impl<IObject, IID__IObjectNode> _IObjectNode;
}

