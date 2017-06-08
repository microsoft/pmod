/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:NotifyCollectionChangedEventArgs.java
****/
package com.microsoft.pmod;

import com.microsoft.pmod.EventArgs;

class NotifyCollectionChangedEventArgs extends EventArgs {
	
	/**
	   * Internal ctor invoked by JNI
	   * @param interfaceValue
	   */
	NotifyCollectionChangedEventArgs(long interfaceValue) {
		super(interfaceValue);
	}
	
	native int getAction();
	native Object[] getNewItems();
	native Object[] getOldItems();
	native int getNewStartingIndex();
	native int getOldStartingIndex();
}
