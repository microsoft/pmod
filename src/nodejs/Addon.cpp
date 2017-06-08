/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:Addon.cpp
****/
#include "pch.h"

#include "DictionaryWrap.h"
#include "DispatcherWrap.h"
#include "AsyncOperationWrap.h"
#include "ObjectDispatchWrap.h"
#include "ObservableObjectWrap.h"
#include "ObservableCollectionWrap.h"
#include "CommandWrap.h"
#include "Statics.h"

#if defined(DEBUG)
#include <foundation/library/debug_library.h>
using namespace foundation;
#endif
#include "uv.h"

using namespace v8;

uv_thread_t _MainThread;

void InitAll(Handle<Object> exports)
{
    _MainThread = uv_thread_self();
#if defined(DEBUG)
    debug_util::RemoveOptionsFlag(library::_OptionTrackTypeInfoLeaks);
#endif

    Statics::Init(exports);
    DictionaryWrap::Init(exports);
    DispatcherWrap::Init(exports);
    AsyncOperationWrap::Init(exports);
    ObjectDispatchWrap::Init(exports);
    ObservableObjectWrap::Init(exports);
    ObservableCollectionWrap::Init(exports);
    CommandWrap::Init(exports);
}

NODE_MODULE(pmod, InitAll)
