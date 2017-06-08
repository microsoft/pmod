/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:Errors.winrt.g.cpp
****/
typedef TMoEnumValueAdapterBase<ABI::Windows::Foundation::IReference<enum ABI::Microsoft::PropertyModel::Errors>, ABI::Microsoft::PropertyModel::Errors> CMoErrors_EnumValue;


    // Model  Factory Entries
    static const foundation::library::_TypeAdapterEntry _coreErrorsFactoryEntries [] = {
        { __uuidof(ABI::Windows::Foundation::IReference<enum ABI::Microsoft::PropertyModel::Errors>), pmod::ErrorsEnum::IIDType, foundation::CreateAdapter<CMoErrors_EnumValue> },

    { GUID_NULL, GUID_NULL, 0L }
};
