/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:CommandParameter.java
****/
package com.microsoft.pmod;

public interface CommandParameter<T> extends BaseCommand {
	
	public void execute(T parameter);
	public boolean canExecute(T parameter);
}
