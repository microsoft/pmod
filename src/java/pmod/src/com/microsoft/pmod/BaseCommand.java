/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:BaseCommand.java
****/
package com.microsoft.pmod;

public interface BaseCommand extends FoundationObject {
	
	public int getState();
	public void addCanExecuteChangedListener(CanExecuteChangedEventListener listener);
	public void removeCanExecuteChangedListener(CanExecuteChangedEventListener listener);
    
}
