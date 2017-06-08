/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ComException.java
****/
package com.microsoft.pmod;

public class ComException extends RuntimeException {
    /**
     * 
     */
    private static final long serialVersionUID = 1L;

    // Fields
    protected int hr;

    // Constructors
    public ComException() {
        super();
    }

    public ComException(int hr) {
        super();
        this.hr = hr;
    }

    public ComException(int hr, String message) {
        super(message);
        this.hr = hr;
    }

    // Methods
    public int getHResult() {
        return hr;
    }
}
