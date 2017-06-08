/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:value_ptr.h
****/
#pragma once

#include <foundation/com_ptr.h>
#include <foundation/macros.h>
#include "boxing_util.h"
#include "exception.h"

namespace foundation
{
    class ValuePtr :
        public ComPtr<IInspectable>
    {
    public:
        ValuePtr() throw()
        {
        }

        template <class T>
        ValuePtr(_In_ T value) throw()
        {
            *this->GetAddressOf() = foundation::CreateValue(value).Detach();
        }

        HRESULT ChangeType(PropertyType type)
        {
            IFCPTR_ASSERT(this->Get());
            return pv_util::ChangeType(type,this->GetAddressOfPtr());
        }

        int Compare(foundation::IInspectable *value) const
        {
            return pv_util::CompareInspectableValues(this->Get(),value);
        }

        template <class T>
        bool operator==(_In_ T value) const
        {
            return Compare(foundation::CreateValue(value)) == 0;
        }

        template <class T>
        bool operator!=(_In_ T value) const
        {
            return !operator==(value);
        }

        HStringPtr ToHString() const
        {
            HStringPtr hstr;
            pv_util::ToString(this->Get(), hstr.GetAddressOf());
            return hstr;
        }

        string_t ToString() const
        {
            return string_t(ToHString().GetRawBuffer());
        }

        template <class T>
        ValuePtr& operator=(_In_ T value)
        {
            Release();
            *this->GetAddressOf() = foundation::CreateValue(value).Detach();
            return (*this);
        }

        foundation::PropertyType GetType() const
        {
            if (this->Get() == nullptr)
            {
                return PropertyType_Empty;
            }
            foundation::PropertyType propertyType;
            pv_util::GetType(this->Get(), &propertyType);
            return propertyType;
        }

        bool IsEmpty()
        {
            return pv_util::IsValueEmpty(this->Get());
        }

        bool IsArray()
        {
            return pv_util::IsValueArray(this->Get());
        }

        template <class T>
        HRESULT GetValue(T& value)
        {
            IFCPTR_ASSERT(this->Get());
            return foundation::GetValue(this->Get(), value);
        }

        template <class T>
        T GetValue()
        {
            T value;
            THROW_IFR(foundation::GetValue(this->Get(), value));
            return value;
        }
    };

}

