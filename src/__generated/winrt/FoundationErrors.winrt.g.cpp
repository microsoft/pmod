/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:FoundationErrors.winrt.g.cpp
****/
typedef TMoEnumValueAdapterBase<ABI::Windows::Foundation::IReference<enum ABI::Microsoft::PropertyModel::FoundationErrors>, ABI::Microsoft::PropertyModel::FoundationErrors> CMoFoundationErrors_EnumValue;


    // Model  Factory Entries
    static const foundation::library::_TypeAdapterEntry _foundationErrorsFactoryEntries [] = {
        { __uuidof(ABI::Windows::Foundation::IReference<enum ABI::Microsoft::PropertyModel::FoundationErrors>), foundation::ErrorsEnum::IIDType, foundation::CreateAdapter<CMoFoundationErrors_EnumValue> },

    { GUID_NULL, GUID_NULL, 0L }
};
