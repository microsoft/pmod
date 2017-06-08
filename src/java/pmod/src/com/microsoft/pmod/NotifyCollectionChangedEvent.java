/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:NotifyCollectionChangedEvent.java
****/
package com.microsoft.pmod;

public class NotifyCollectionChangedEvent {

	private NotifyCollectionModelChangedAction action;
	private int oldStartingIndex;
	private int newStartingIndex;
	private Object[] oldItems;
	private Object[] newItems;

	NotifyCollectionChangedEvent(
        NotifyCollectionModelChangedAction action,
        int oldStartingIndex,
        int newStartingIndex,
        Object[] oldItems,
        Object[] newItems
        ) {
		this.action = action;
		this.oldStartingIndex = oldStartingIndex;
		this.newStartingIndex = newStartingIndex;
		this.oldItems = oldItems;
		this.newItems = newItems;
	}
	
	public NotifyCollectionModelChangedAction getAction() {
		return action;
	}
	
	public Object[] getNewItems() {
		return newItems;
	}
	
	public Object[] getOldItems() {
		return oldItems;		
	}
    
	public int getNewStartingIndex() {
		return newStartingIndex;				
	}
	
	public int getOldStartingIndex() {
		return oldStartingIndex;						
	}
}
