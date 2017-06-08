/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:EventModelEvent.java
****/
package com.microsoft.pmod;

public class EventModelEvent {
	

	private int _event;
	private Object _eventArgs;
	
	EventModelEvent(int event,Object eventArgs)
	{
		_event = event;
		_eventArgs = eventArgs;
		
	}
	public int getEvent()
	{
		return _event;
	}
	
	public Object getEventArgs()
	{
		return _eventArgs;
	}
}
