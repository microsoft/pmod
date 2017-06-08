/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:CommandImpl.java
****/
package com.microsoft.pmod;

public class CommandImpl<TResult,TParameter> extends BaseCommandImpl implements Command<TResult,TParameter> {

	  /**
	   * Internal constructor invoked by JNI
	   * @param interfaceValue
	   */
	CommandImpl(long interfaceValue) {
		super(interfaceValue);
	}
		
	@SuppressWarnings("unchecked")
	public TResult execute(TParameter parameter) {
		return (TResult)super.executeInternal(parameter);
	}
	
	public boolean canExecute(TParameter parameter) {
		return super.canExecuteInternal(parameter);
	}
}
