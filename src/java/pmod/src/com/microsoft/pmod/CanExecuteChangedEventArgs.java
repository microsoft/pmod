/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:CanExecuteChangedEventArgs.java
****/
package com.microsoft.pmod;

class CanExecuteChangedEventArgs extends EventArgs {
	
	native int getOldState();
	native int getNewState();

	  /**
	   * Internal ctor invoked by JNI
	   * @param interfaceValue
	   */
	CanExecuteChangedEventArgs(long interfaceValue) {
		super(interfaceValue);
	}
}
