/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:source_model.h
****/
#pragma once

#include <foundation/interfaces/object.h>
#include <pmod/pmod_api.h>

namespace pmod {

    ////////////////////// Source based Interfaces  ////////////////////////////    
    BEGIN_DECLARE_INTERFACE(ISourceModelBase, foundation::IInspectable, PMOD_API)
        STDMETHOD(GetIsAttached)(bool *value) = 0;
        STDMETHOD(GetForceAttached)(bool *value) = 0;
        STDMETHOD(SetForceAttached)(bool newValue) = 0;
        STDMETHOD(GetAttachEnabled)(bool *value) = 0;
        STDMETHOD(SetAttachEnabled)(bool newValue) = 0;
    END_DECLARE_INTERFACE()

    BEGIN_DECLARE_INTERFACE(ISourceModel, ISourceModelBase, PMOD_API)
        STDMETHOD(GetSource)(foundation::IObject **ppModel) = 0;
        STDMETHOD(SetSource)(foundation::IObject *pModel) = 0;
    END_DECLARE_INTERFACE()


    BEGIN_DECLARE_INTERFACE(IMultiSourceModel, ISourceModelBase, PMOD_API)
        STDMETHOD(AddSource)(foundation::IObject *ppModel) = 0;
        STDMETHOD(RemoveSource)(foundation::IObject *pModel) = 0;
        STDMETHOD(GetSources)(UINT32 *size, foundation::IObject ***ppModel) = 0;
        STDMETHOD(RemoveAllSources)() = 0;
    END_DECLARE_INTERFACE()

}


