/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:Errors.cs
****/
using System;

namespace pmod
{
    public enum Errors
    {
        E_COLLECTION_READ_ONLY             = 0x80f70103L,
        E_EVENT_NOT_DEFINED                = 0x80f70104L,
        E_COMMAND_NOT_ENABLED              = 0x80f70105L,

        E_PROXY_DISPOSED                 = 0x80f70400L,
        E_PROXY_NO_SOURCE                = 0x80f70401L,
        E_PROXY_NO_FACTORY               = 0x80f70402L,
    }
}

