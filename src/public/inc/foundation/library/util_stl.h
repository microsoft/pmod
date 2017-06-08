/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:util_stl.h
****/
#pragma once

#include <foundation/pal.h>
#include <foundation/pv_util.h>
#include <foundation/string_t.h>
#include <foundation/interfaces/object.h>

#include <functional>
#include <algorithm>
#include <string>

namespace foundation
{
    namespace library
    {
    // Comparison used to determine whether a GUID is less than another so
    // the entries in a std::map<IID, T, GuidLessThan> are correctly
    // sorted.
    class GuidLessThan :
        public std::binary_function<IID, IID, bool>
    {
    public:
        inline bool operator () (_In_ const IID& iid1, _In_ const IID& iid2) const
        {
            return compare(iid1, iid2);
        }

        static bool compare(_In_ const IID& iid1, _In_ const IID& iid2)
        {
            bool less;

            // Bytes 0-3
            if (compare(((UINT32 *)&iid1)[0], ((UINT32 *)&iid2)[0], less))
                return less;
            // Bytes 4-7
            if (compare(((UINT32 *)&iid1)[1], ((UINT32 *)&iid2)[1], less))
                return less;
            // Bytes 8-11
            if (compare(((UINT32 *)&iid1)[2], ((UINT32 *)&iid2)[2], less))
                return less;
            // Bytes 12-15
            return ((UINT32 *)&iid1)[3] < ((UINT32 *)&iid2)[3];
        }
    private:
        static inline bool compare(UINT32 l1, UINT32 l2, bool& less)
        {
            if (l1 == l2)
                return false;
            less = l1 < l2;
            return true;
        }
    };

    class StringLessThan :
        public std::binary_function<LPCSTR_t, LPCSTR_t, bool>
    {
    public:
        bool operator () (_In_ const LPCSTR_t& str1, _In_ const LPCSTR_t& str2) const
        {
            return _pal_strcmp(str1, str2) < 0;
        }
    };
    class StringTypeLessThan :
        public std::binary_function<string_t, string_t, bool>
    {
    public:
        bool operator () (_In_ const string_t& str1, _In_ const string_t& str2) const
        {
            return _pal_strcmp(str1.c_str(), str2.c_str()) < 0;
        }
    };

	class HStringPtrLessThan :
		public std::binary_function<foundation::HStringPtr, foundation::HStringPtr, bool>
	{
	public:
		bool operator () (_In_ const foundation::HStringPtr& str1, _In_ const  foundation::HStringPtr& str2) const
		{
			return _pal_strcmp(str1.GetRawBuffer(), str2.GetRawBuffer()) < 0;
		}
	};

	class PropertyValueLessThan : std::binary_function<foundation::InspectablePtr, foundation::InspectablePtr, bool>
	{
	public:

		bool operator()(const foundation::InspectablePtr& item1, const foundation::InspectablePtr& item2) const
		{
			return foundation::pv_util::CompareInspectableValues(
				item1,
				item2)  < 0;
		}
	};

	template <class Container,class _Pr>
	UINT32 IndexOf(
		const Container& items,
		foundation::IInspectable *item,
		_Pr _Pred)
	{
		auto iter = std::search_n(
			items.begin(),
			items.end(),
			1,
			foundation::InspectablePtr(item),
			_Pred);
		if (iter != items.end())
		{
			return (UINT32)(iter - items.begin());
		}
		return (UINT32)-1;
	}

	template <class Container>
	UINT32 IndexOf(
		const Container& items,
		foundation::IInspectable *item)
	{
		return IndexOf(items, item, PropertyValueLessThan());
	}

	template <class Container, class _Pr>
	bool IsEqual(
		const Container& items1,
		const Container& items2,
		_Pr _Pred)
	{
		return std::equal(items1.begin(), items1.end(), items2.begin(), _Pred);
	}

	template <class Container>
	bool IsEqual(
		const Container& items1,
		const Container& items2)
	{
		return IsEqual(items1, items2, PropertyValueLessThan());
	}

	class ObjectLessThan :
		public std::binary_function<foundation::IObject *, foundation::IObject *, bool>
	{
	public:
		bool operator () (
			_In_ const foundation::IObject *pObject1,
			_In_ const foundation::IObject *pObject2) const
		{
			UINT32 processId1, uniqueId1;
			UINT32 processId2, uniqueId2;

			IFRF_ASSERT(((foundation::IObject *)pObject1)->GetProcessId(&processId1));
			IFRF_ASSERT(((foundation::IObject *)pObject2)->GetProcessId(&processId2));
			if (processId1 < processId2)
			{
				return true;
			}

			IFRF_ASSERT(((foundation::IObject *)pObject1)->GetUniqueId(&uniqueId1));
			IFRF_ASSERT(((foundation::IObject *)pObject2)->GetUniqueId(&uniqueId2));

			return uniqueId1 < uniqueId2;

		}
	};
    }
}

