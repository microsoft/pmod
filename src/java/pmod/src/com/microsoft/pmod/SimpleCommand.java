/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:SimpleCommand.java
****/
package com.microsoft.pmod;

public interface SimpleCommand extends BaseCommand {
	
	public void execute();
	public boolean canExecute();
}
