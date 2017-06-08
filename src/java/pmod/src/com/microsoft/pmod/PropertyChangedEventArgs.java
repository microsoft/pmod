/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PropertyChangedEventArgs.java
****/
package com.microsoft.pmod;

/**
 * Class:PropertyChangedEventArgs
 * @author rodrigov
 * Represent an PropertyChangedEventArgs
 */
class PropertyChangedEventArgs extends EventArgs {
	
  /**
   * Internal ctor invoked by JNI
   * @param interfaceValue
   */
	PropertyChangedEventArgs(long interfaceValue) {
		super(interfaceValue);
	}
	
	native int getProperty();
	native Object getOldValue();
	native Object getNewValue();
		
}
