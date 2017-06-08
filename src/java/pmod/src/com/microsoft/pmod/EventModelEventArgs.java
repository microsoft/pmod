/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:EventModelEventArgs.java
****/
package com.microsoft.pmod;

/**
 * Class:EventModelEventArgs
 * @author rodrigov
 * Represent an IEventModelEventArgs
 */
class EventModelEventArgs extends EventArgs {
	
  /**
   * Internal ctor invoked by JNI
   * @param interfaceValue
   */
	EventModelEventArgs(long interfaceValue)
	{
		super(interfaceValue);
	}
	
	native int getEvent();
	native Object getEventArgs();
		
}
