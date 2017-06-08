/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:CompletedEventArgs.java
****/
package com.microsoft.pmod;


class CompletedEventArgs extends EventArgs {
	
	  /**
	   * Internal ctor invoked by JNI
	   * @param interfaceValue
	   */
	CompletedEventArgs(long interfaceValue) {
		super(interfaceValue);
	}
}
