/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:SimpleCommandImpl.java
****/
package com.microsoft.pmod;

public class SimpleCommandImpl extends BaseCommandImpl implements SimpleCommand {

	  /**
	   * Internal constructor invoked by JNI
	   * @param interfaceValue
	   */
	SimpleCommandImpl(long interfaceValue) {
		super(interfaceValue);
	}
		
	public void execute() {
		super.executeInternal(null);
	}
	
	public boolean canExecute() {
		return super.canExecuteInternal(null);
	}
}
