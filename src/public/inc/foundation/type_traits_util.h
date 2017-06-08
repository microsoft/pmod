/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:type_traits_util.h
****/
#pragma once

#if _HAS_CPP0X

#include <type_traits>

namespace foundation
{
    template< class T>
    struct is_foundation_type
        : std::integral_constant<
        bool,
        std::is_arithmetic<T>::value ||
        std::is_same<T, bool>::value ||
        std::is_same<T, foundation::DateTime>::value ||
        std::is_same<T, foundation::SystemTime>::value ||
        std::is_same<T, HSTRING>::value ||
        std::is_same<T, GUID>::value ||
        std::is_base_of<foundation::IInspectable, typename std::remove_pointer<T>::type>::value
        > {};

#define ENABLE_IF_VECTOR_VALUE \
    typename std::enable_if< \
        std::is_same<std::random_access_iterator_tag, typename ContainerT::iterator::iterator_category>::value && \
        (std::is_fundamental<typename ContainerT::value_type>::value || \
        std::is_same<foundation::HStringPtr, typename ContainerT::value_type>::value) \
        >::type* = nullptr

#define ENABLE_IF_VECTOR_PTR \
    typename std::enable_if<\
        std::is_same<std::random_access_iterator_tag, typename ContainerT::iterator::iterator_category>::value &&\
        std::is_base_of<_ComPtrBaseType, typename ContainerT::value_type>::value &&\
        std::is_base_of<foundation::IInspectable, typename std::remove_pointer<typename ContainerT::value_type::_PtrClass>::type>::value\
        >::type* = nullptr

#define ENABLE_IF_CONTAINER_VALUE \
    typename std::enable_if<\
    !std::is_same<std::random_access_iterator_tag, typename ContainerT::iterator::iterator_category>::value &&\
        (std::is_fundamental<typename ContainerT::value_type>::value ||\
        std::is_same<foundation::HStringPtr, typename ContainerT::value_type>::value)\
            >::type* = nullptr

#define ENABLE_IF_CONTAINER_PTR \
    typename std::enable_if<\
    !std::is_same<std::random_access_iterator_tag, typename ContainerT::iterator::iterator_category>::value &&\
        std::is_base_of<_ComPtrBaseType, typename ContainerT::value_type>::value &&\
        std::is_base_of<foundation::IInspectable, typename std::remove_pointer<typename ContainerT::value_type::_PtrClass>::type>::value\
            >::type* = nullptr

}
#endif
