/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:object_serializer.h
****/
#pragma once

#include <foundation/com_interfaces.h>
#include <foundation/foundation_api.h>

namespace foundation {

    BEGIN_DECLARE_INTERFACE(IObjectSerializer, IUnknown, FOUNDATION_API)
        STDMETHOD(GetContent)(HSTRING serializerActivationId,IInspectable *pContentOptions, IInspectable **ppSerializedContent) = 0;
        STDMETHOD(SetContent)(HSTRING serializerActivationId, IInspectable *pSerializedContent) = 0;
    END_DECLARE_INTERFACE()

}

