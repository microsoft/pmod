/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:stream_provider.h
****/
#pragma once

#include <foundation/com_interfaces.h>
#include <foundation/foundation_api.h>


namespace foundation {

    BEGIN_DECLARE_INTERFACE(IStreamProvider, foundation::IUnknown, FOUNDATION_API)
        STDMETHOD(Serialize)(_In_ IInspectable *pValue, _COM_Outptr_ IInspectable **ppValue) = 0;
        STDMETHOD(Deserialize)(_In_ IInspectable *pValue, _COM_Outptr_ IInspectable **ppValue) = 0;
    END_DECLARE_INTERFACE()
}

