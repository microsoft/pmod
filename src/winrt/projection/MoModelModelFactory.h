/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:MoModelModelFactory.h
****/

#pragma once

#include "MoModelFactory.h"
#include "Microsoft.PropertyModel.h"

using namespace ABI::Microsoft::PropertyModel;

//------------------------------------------------------------------------------
// Class:   MoModelModelFactory
//
// Purpose: Factory class for a Model
//
//------------------------------------------------------------------------------
class MoModelModelFactory :
        public ctlwinrt::AbstractActivationFactory,
        public IModelStatics
{
public:
    // IModelStatics
    IFACEMETHOD(IsFactoryAvailable)(
        /* [in] */ HSTRING activationId,
        /* [out][retval] */ boolean *value) override;

    IFACEMETHOD(GetActivationFactory)(
        HSTRING activationId,
        IInspectable **value) override;

    IFACEMETHOD(ActivateObject)(
        GUID iidInstanceType,
        IInspectable *pContext,
        IObject **value) override;

    IFACEMETHOD(IsInstanceOfType)(
                            IObject *pModel,
                            GUID iidInstanceType,
                            boolean *pResult);

    IFACEMETHOD(IsInstanceOfTypeWithOptions)( 
                            IObject *pModel,
                            GUID iidInstanceType,
                            InstanceTypeOptions options,
                            boolean *pResult);
                        
    IFACEMETHOD(IsEqual)( 
                            IObject *pItem1,
                            IObject *pItem2,
                            boolean *pResult);
                        
    IFACEMETHOD(GetParent)(
        IObject *pModel,
        IObject **ppResult);

    IFACEMETHOD(GetChilds)(
        ABI::Microsoft::PropertyModel::IObject *pModel,
        UINT32 *length,
        ABI::Microsoft::PropertyModel::IObject ***value);

    IFACEMETHOD(GetAncestor)(
                            IObject *pModel,
                            GUID iidAncestorType,
                            IObject **ppResult);
                        
    IFACEMETHOD(GetRoot)( 
                            IObject *pModel,
                            IObject **ppRoot);
                        
    IFACEMETHOD(GetSource)( 
                            IInspectable *pSourceModel,
                            IObject **ppSource);

                        
    IFACEMETHOD(GetSourceOfType)( 
                            IInspectable *pSourceModel,
                            GUID iidInstanceType,
                            InstanceTypeOptions options,
                            boolean isModelClassType,
                            IObject **ppSource);
    IFACEMETHOD(Dispose)(
        IInspectable *pObject);
protected:
    HRESULT QueryInterfaceImpl(_In_ REFIID iid, _COM_Outptr_ IUnknown **ppInterface) override
    {
        if (iid == __uuidof(IModelStatics))
        {
            *ppInterface = static_cast<IModelStatics *>(this);
        }
        else
        {
            return __super::QueryInterfaceImpl(iid,ppInterface);
        }
        return S_OK;
    }

};
