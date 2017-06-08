/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:CommandResultImpl.java
****/
package com.microsoft.pmod;

public class CommandResultImpl<T> extends BaseCommandImpl implements CommandResult<T> {

	  /**
	   * Internal constructor invoked by JNI
	   * @param interfaceValue
	   */
	CommandResultImpl(long interfaceValue) {
		super(interfaceValue);
	}
		
	@SuppressWarnings("unchecked")
	public T execute() {
		return (T)super.executeInternal(null);
	}
	
	public boolean canExecute() {
		return super.canExecuteInternal(null);
	}
}
