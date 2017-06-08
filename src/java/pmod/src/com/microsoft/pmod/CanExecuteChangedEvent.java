/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:CanExecuteChangedEvent.java
****/
package com.microsoft.pmod;

public class CanExecuteChangedEvent {

	private int oldState;
	private int newState;
	
	CanExecuteChangedEvent(int oldState, int newState) {
		this.oldState = oldState;
		this.newState = newState;
		
	}
    
	public int getOldState() {
		return oldState;
	}
	
	public int getNewState() {
		return newState;
	}

}
