/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:CommandResult.java
****/
package com.microsoft.pmod;

public interface CommandResult<T> extends BaseCommand {
	
	public T execute();
	public boolean canExecute();
}
