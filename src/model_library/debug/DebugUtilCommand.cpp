/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:DebugUtilCommand.cpp
****/
#include "pch.h"

#if defined(DEBUG)

#include <foundation/library/debug_library.h>
#include <pmod/library/library_util.h>

using namespace foundation;
using namespace pmod;
using namespace pmod::library;

static HRESULT STDMETHODCALLTYPE CreateDebugUtilCommand(
    _In_ foundation::IInspectable *pContextParameter,
    _COM_Outptr_ foundation::IObject **ppModel)
{
    //Func Delegate
    auto debug_util_func = [](foundation::IInspectable *parameter, foundation::IInspectable **ppResult) -> HRESULT {

        PropertyType propertyType;
        foundation::pv_util::GetType(parameter, &propertyType);

        if (propertyType == PropertyType_UInt32)
        {
            UINT32 debug_option;
            IFR_ASSERT(foundation::pv_util::GetValue(parameter, &debug_option));
            switch (debug_option)
            {
            case 0:
                debug_util::ClearTrackedObjects();
                break;
            case 1:
                debug_util::DumpTrackedObjects();
                break;
            case 2:
                foundation::pv_util::CreateInt32Value(debug_util::CountTrackedObjects(), ppResult);
                break;
            case 3:
            {
                UINT32 flags;
                debug_util::GetOptionsFlags(flags);
                foundation::pv_util::CreateUInt32Value(flags, ppResult);
            }
                break;
            case 10:
                debug_util::ClearAllPerfCounters();
                break;
            case 11:
                return debug_util::GetPerfCountersInfo(reinterpret_cast<IDictionary **>(ppResult));
                break;
            }
        }
        else if(propertyType == PropertyType_UInt32Array)
        {
            UInt32ArrayWrapper values;
            _IFR_(foundation::pv_util::GetValue(
                parameter, 
                values.GetSizeAddressOf(),
                values.GetBufferAddressOf()));
            if (values.GetSize() < 2)
            {
                return E_UNEXPECTED;
            }
            UINT32 debug_option = values[0];
            UINT32 option = values[1];
            if (debug_option == 3)
            {
                debug_util::SetOptionsFlags(option);
            }
            else if (debug_option == 4)
            {
                debug_util::SetOptionsFlag(option);
            }
            else if (debug_option == 5)
            {
                debug_util::RemoveOptionsFlag(option);
            }
            else if (debug_option == 6)
            {
                foundation::pv_util::CreateBooleanValue(debug_util::IsOptionsFlag(option), ppResult);
            }
            else if (debug_option == 7)
            {
                UINT32 counter;
                UINT64 time;
                debug_util::GetPerfCounter(option, counter, time);
                std::vector<foundation::InspectablePtr> values;
                values.push_back(foundation::pv_util::CreateValue(counter));
                values.push_back(foundation::pv_util::CreateValue(time));
                foundation::pv_util::CreateInspectableArrayValue(
                    2, 
                    values.front().GetAddressOfPtr(),
                    ppResult);
            }
            else if (debug_option == 8)
            {
                debug_util::ClearPerfCounter(option);
            }
        }
        else if (propertyType == PropertyType_InspectableArray)
        {
            InspectableArrayWrapper values;
            _IFR_(foundation::pv_util::GetValue(
                parameter,
                values.GetSizeAddressOf(),
                values.GetBufferAddressOf()));
            if (values.GetSize() < 3)
            {
                return E_UNEXPECTED;
            }
            UINT32 debug_option;
            IFR_ASSERT(foundation::pv_util::GetValue(values[0], &debug_option));
            if (debug_option == 9)
            {
                UINT32 perfId;
                IFR_ASSERT(foundation::pv_util::GetValue(values[1], &perfId));
                UINT64 time;
                IFR_ASSERT(foundation::pv_util::GetValue(values[2], &time));
                debug_util::AddPerfCounter(perfId, time);
            }
        }
        return S_OK;
    };

    return pmod::library::library::CreateCommand(
        foundation_GUID_NULL,
        CommandOptions::None,
        debug_util_func,
        ppModel);
}

HRESULT RegisterDebugUtilCommand()
{
    // {71D56E14-BC19-4687-98C5-DE9EBEF5A0E0}
    static const GUID IID_DebugUtilCommand =
    { 0x71d56e14, 0xbc19, 0x4687, { 0x98, 0xc5, 0xde, 0x9e, 0xbe, 0xf5, 0xa0, 0xe0 } };

    return foundation::library::RegisterObjectFactory(IID_DebugUtilCommand, CreateDebugUtilCommand);
}

#endif
