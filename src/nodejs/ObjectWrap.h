/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObjectWrap.h
****/
#pragma once

#include "InspectableBaseWrap.h"
#include <foundation/interfaces/object.h>


class ObjectWrap : public InspectableBaseWrap 
{
protected:
    static void SetPrototypeMethods(v8::Local<v8::FunctionTemplate>& tpl);
private:
    static void GetUniqueId(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetProcessId(const v8::FunctionCallbackInfo<v8::Value>& args);
};

