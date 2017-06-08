/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:InstanceFlags.java
****/
package com.microsoft.pmod;

public enum InstanceFlags {
    None(0),
    Declared(1),
    Inherited(2);

    private final int _flags;
    
    public int flags() { return _flags; }

    InstanceFlags(int flags)
    {
    	this._flags = flags;
    }
}
