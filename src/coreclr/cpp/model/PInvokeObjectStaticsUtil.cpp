/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PInvokeObjectStaticsUtil.cpp
****/
#include "../pinvoke_api.h"
#include "../PInvokeUtils.h"

#include <foundation/library/object_factory_util.h>
#include <foundation/interfaces/property_value.h>
#include <pmod/interfaces.h>

using namespace foundation;

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE ObjectStaticsUtil_ActivateObject(
    GUID *pIIdType,
    foundation::IInspectable *pContext,
    foundation::IObject **ppObject)
{
    return library::ActivateObject(*pIIdType, pContext, ppObject);
}

EXTERN_C PINVOKE_API HRESULT STDAPICALLTYPE ObjectStaticsUtil_GetObjectType(
    foundation::IInspectable *pObject,
    IID *pType)
{
    IID iid;

    if (foundation::IsTypeOf(pObject, foundation::IEnumValue::GetIID()))
    {
        iid = foundation::IEnumValue::GetIID();
    }
    else if(foundation::IsTypeOf(pObject, foundation_IID_IPropertyValue))
    {
        iid = foundation_IID_IPropertyValue;
    }
    else if (foundation::IsTypeOf(pObject, pmod::IObservableList::GetIID()))
    {
        iid = pmod::IObservableList::GetIID();
    }
    else if (foundation::IsTypeOf(pObject, pmod::IObservableCollection::GetIID()))
    {
        iid = pmod::IObservableCollection::GetIID();
    }
    else if (foundation::IsTypeOf(pObject, pmod::ICommand::GetIID()))
    {
        iid = pmod::ICommand::GetIID();
    }
    else if (foundation::IsTypeOf(pObject, foundation::IAsyncOperation::GetIID()))
    {
        iid = foundation::IAsyncOperation::GetIID();
    }
    else if (foundation::IsTypeOf(pObject, pmod::IObservableObject::GetIID()))
    {
        iid = pmod::IObservableObject::GetIID();
    }
    else if (foundation::IsTypeOf(pObject, foundation::IObjectDispatch::GetIID()))
    {
        iid = foundation::IObjectDispatch::GetIID();
    }
    else if (foundation::IsTypeOf(pObject, foundation::IDictionary::GetIID()))
    {
        iid = foundation::IDictionary::GetIID();
    }
    else
    {
        iid = foundation_IID_IInspectable;
    }
    *pType = iid;
    return S_OK;
}
