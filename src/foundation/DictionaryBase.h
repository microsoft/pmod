/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:DictionaryBase.h
****/

#pragma once

#include <foundation/interfaces.h>
#include <foundation/errors.h>

#include <foundation/hstring_wrapper.h>
#include <foundation/library/dictionary_impl.h>
#include <foundation/library/dictionary_class.h>
#include <foundation/library/util_stl.h>

#include <foundation/com_ptr.h>
#include <foundation/ctl/com_weak_reference_source.h>

//------------------------------------------------------------------------------
// Class:   CDictionaryClass
//
// Purpose: class for IDictionary Interface
//
//------------------------------------------------------------------------------
class CDictionaryClass :
    public foundation::library::_DictionaryBase
    <
        CDictionaryClass,
        foundation::library::_DefaultAdapterHostType<foundation::IDictionary, foundation::ctl::ComWeakReferenceSourceInspectableBase, CDictionaryClass >
    >
{
public:
    HRESULT _Initialize()
    {
        IFR_ASSERT(CreateCriticalSectionImpl(_cs.GetAddressOf()));
        return S_OK;
    }

    static HRESULT CreateInstance(_COM_Outptr_ foundation::IDictionary **ppDictionary)
    {
        return foundation::ctl::inspectable::CreateInstanceWithInitialize<CDictionaryClass>(ppDictionary);
    }

    foundation::library::ICriticalSection* _Get_lock()
    {
        return _cs;
    }
private:
    foundation::ComPtr<foundation::library::ICriticalSection> _cs;

protected:
#if defined(_WINDOWS_RUNTIME)
    INSPECTABLE_CLASS(U("Microsoft.PropertyModel.Dictionary"), foundation::TrustLevel::BaseTrust);
#endif
};
