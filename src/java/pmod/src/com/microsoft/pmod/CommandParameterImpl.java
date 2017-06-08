/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:CommandParameterImpl.java
****/
package com.microsoft.pmod;

public class CommandParameterImpl<T> extends BaseCommandImpl implements CommandParameter<T> {

	  /**
	   * Internal constructor invoked by JNI
	   * @param interfaceValue
	   */
	CommandParameterImpl(long interfaceValue) {
		super(interfaceValue);
	}
		
	public void execute(T parameter) {
		super.executeInternal(parameter);
	}
	
	public boolean canExecute(T parameter) {
		return super.canExecuteInternal(parameter);
	}
}
