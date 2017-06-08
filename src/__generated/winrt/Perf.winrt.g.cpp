/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:Perf.winrt.g.cpp
****/
typedef TMoEnumValueAdapterBase<ABI::Windows::Foundation::IReference<enum ABI::Microsoft::PropertyModel::Core_PerfId>, ABI::Microsoft::PropertyModel::Core_PerfId> CMoCore_PerfId_EnumValue;
typedef TMoEnumValueAdapterBase<ABI::Windows::Foundation::IReference<enum ABI::Microsoft::PropertyModel::ProxyFactory_PerfId>, ABI::Microsoft::PropertyModel::ProxyFactory_PerfId> CMoProxyFactory_PerfId_EnumValue;
typedef TMoEnumValueAdapterBase<ABI::Windows::Foundation::IReference<enum ABI::Microsoft::PropertyModel::CoreProxyFactory_PerfId>, ABI::Microsoft::PropertyModel::CoreProxyFactory_PerfId> CMoCoreProxyFactory_PerfId_EnumValue;


    // Model  Factory Entries
    static const foundation::library::_TypeAdapterEntry _corePerfFactoryEntries [] = {
{ __uuidof(ABI::Windows::Foundation::IReference<enum ABI::Microsoft::PropertyModel::Core_PerfId>), pmod::Core_PerfIdEnum::IIDType, foundation::CreateAdapter<CMoCore_PerfId_EnumValue> },
{ __uuidof(ABI::Windows::Foundation::IReference<enum ABI::Microsoft::PropertyModel::ProxyFactory_PerfId>), pmod::ProxyFactory_PerfIdEnum::IIDType, foundation::CreateAdapter<CMoProxyFactory_PerfId_EnumValue> },
{ __uuidof(ABI::Windows::Foundation::IReference<enum ABI::Microsoft::PropertyModel::CoreProxyFactory_PerfId>), pmod::CoreProxyFactory_PerfIdEnum::IIDType, foundation::CreateAdapter<CMoCoreProxyFactory_PerfId_EnumValue> },

    { GUID_NULL, GUID_NULL, 0L }
};
