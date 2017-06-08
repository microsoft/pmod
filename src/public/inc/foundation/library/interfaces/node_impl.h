/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:node_impl.h
****/
#pragma once
#include <foundation/foundation_api.h>
#include <foundation/com_interfaces.h>

namespace foundation {
	
	namespace library {

		BEGIN_DECLARE_INTERFACE(_INode_impl, foundation::IUnknown, FOUNDATION_API)
			virtual _INode_impl *_GetParent() = 0;
			virtual void _SetParent(_INode_impl *pChild) = 0;
			virtual void _AddChild(_INode_impl *pChild) = 0;
			virtual void _RemoveChild(_INode_impl *pChild) = 0;
			virtual void _ClearParent() = 0;
			virtual bool _IsChildContained(_INode_impl *pChild) = 0;
			virtual void _RaiseAncestorChanged(_INode_impl *pChild, _INode_impl *pParent) = 0;
			virtual void _NotifyParentChanged(_INode_impl *pParent) = 0;
			virtual void _OnParentChanged(_INode_impl *pParent) = 0;
			virtual void _OnAncestorChanged(_INode_impl *pChild, _INode_impl *pParent) = 0;
		END_DECLARE_INTERFACE()
	}
}

