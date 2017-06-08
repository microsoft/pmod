/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:Command.java
****/
package com.microsoft.pmod;

public interface Command<TResult,TParameter> extends BaseCommand {
	
	public TResult execute(TParameter parameter);
	public boolean canExecute(TParameter parameter);
}
