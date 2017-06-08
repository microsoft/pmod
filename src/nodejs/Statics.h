/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:Statics.h
****/
#pragma once


class Statics {
 public:
  static void Init(v8::Handle<v8::Object> exports);
private:
  static void LoadLibraryInternal(const v8::FunctionCallbackInfo<v8::Value>& args);
  static void GetActivationFactory(const v8::FunctionCallbackInfo<v8::Value>& args);
};

