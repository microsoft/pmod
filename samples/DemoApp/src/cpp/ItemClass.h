/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ItemClass.h
****/

#pragma once

#include "DemoAppFastInternal.g.h"

class CItemClass :
	public DemoApp::_FastItemBase
{
public:
    HRESULT _InitializeModel();
    HRESULT _InitializeModel(HSTRING name);
protected:
    // to track addref/release behavior from managed languages
    _pal_AtomicIntegerType AddRefImpl() override;
    _pal_AtomicIntegerType ReleaseImpl() override;

    void FinalRelease() override;
private:
    HRESULT ExecuteChangeName(HSTRING parameter) override;

};
