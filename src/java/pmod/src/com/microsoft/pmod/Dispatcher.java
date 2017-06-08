/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:Dispatcher.java
****/
package com.microsoft.pmod;

public final class Dispatcher extends InspectableObject {

	// Private Native methods
	private static native void invokeDispatchable(long dispatchable);
	private native void createDispatcherAdapter();
	
	private DispatcherCallback dispatcherCallback;
	public Dispatcher(DispatcherCallback dispatcherCallback )
	{
		createDispatcherAdapter();
		this.dispatcherCallback = dispatcherCallback;
	}
	
	boolean hasThreadAccessCallback()
	{
		return dispatcherCallback.hasThreadAccess();
	}
	
	void runAsyncCallback(final long dispatchable)
	{
		dispatcherCallback.runAsync(new java.lang.Runnable() {			
			@Override
			public void run()
			{
				invokeDispatchable(dispatchable);
			}
		});
	}

}
