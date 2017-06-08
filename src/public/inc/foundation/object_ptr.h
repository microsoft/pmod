/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:object_ptr.h
****/
#pragma once

#include <foundation/com_ptr.h>
#include "node_statics_util.h"
#include "type_info_statics_util.h"

namespace foundation
{
    template <class T>
    struct _IInterfaceHost
    {
        virtual T *GetInterface() const = 0;
    };

    template <class T, class TPublicInterface>
    class _PtrWrapperBase
    {
    public:
        typedef TPublicInterface _Interface_Type;
    protected:
        inline TPublicInterface *GetClassInterface() const
        {
            const T *pThis = static_cast<const T *>(this);
            TPublicInterface *pInterface = pThis->GetInterface();
            foundation_assert(pInterface != nullptr);
            return pInterface;
        }
    };

    template <class T,class TPublicInterface>
    class _ObjectPtrWrapper : public _PtrWrapperBase<T, TPublicInterface>
    {
    public:
        UINT32 GetUniqueId()
        {
            UINT32 uniqueId = 0;
            IGNOREHR(this->GetClassInterface()->GetUniqueId(&uniqueId));
            return uniqueId;
        }

        UINT32 GetProcessId()
        {
            UINT32 processId = 0;
            this->GetClassInterface()->GetProcessId(&processId);
            return processId;
        }

        IID GetObjectType()
        {
            IID iidType;
            IGNOREHR(foundation::GetObjectType(this->GetClassInterface(), iidType));
            return iidType;
        }

        HRESULT GetTypeInfo(foundation::IObjectTypeInfo **ppTypeInfo)
        {
            return this->GetClassInterface()->GetTypeInfo(ppTypeInfo);
        }

		///
		/// @brief return the parent TParentInterface reference
		///
		template <class TParentInterface>
		HRESULT GetParent(_COM_Outptr_result_maybenull_ TParentInterface **ppParent)
		{
			IFCPTR_ASSERT(ppParent);

			return GetObjectParent(
				this->GetClassInterface(),
				ppParent);
		}
    };


template <class _Interface_wrapper, class T = typename _Interface_wrapper::_Interface_Type>
class _ObjectPtr :
    public ComPtr<T>,
    public _Interface_wrapper
{
public:
    typedef ComPtr<T> _Com_ptr_base_type;

    _ObjectPtr() throw()
        :_hresult(S_OK)
    {
    }
    _ObjectPtr(_Inout_ const _ComPtrBase<T>& lp) throw() :
        _Com_ptr_base_type(lp.get()),
        _hresult(S_OK)
    {
    }
    _ObjectPtr(_In_ T* lp) throw() :
        _Com_ptr_base_type(lp),
        _hresult(S_OK)
    {
    }
    _ObjectPtr(_Inout_ const _ObjectPtr<_Interface_wrapper>& other) throw() :
        _Com_ptr_base_type(other.Get()),
        _hresult(other._hresult)
    {
    }

    T *GetInterface() const
    {
        return this->Get();
    }

    void _Sethresult(HRESULT hr) { _hresult = hr; }
    HRESULT hresult() { return _hresult; }
private:
    HRESULT _hresult;
};

}

