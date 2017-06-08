/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:BaseAsyncOperationImpl.java
****/
package com.microsoft.pmod;

import java.util.Iterator;
import java.util.concurrent.TimeoutException;

public class BaseAsyncOperationImpl extends FoundationObjectImpl implements BaseAsyncOperation {

	private  java.util.concurrent.CopyOnWriteArrayList<CompletedEventListener> listeners;

	// Private Native methods
	protected native Object GetResult(int timeout);
	private native int GetStatus();
	protected native boolean Cancel();
	protected native int Wait(int timeout);
	
	/**
	 * create a listener to the 
	 */
	@Override
	native long createListenerProxy(EventSink eventSink);
    
	public native String getError();

    /**
     * Internal constructor invoked by JNI
     * @param interfaceValue
     */
	BaseAsyncOperationImpl(long interfaceValue) {
		super(interfaceValue);
	}
	
	public AsyncStatus getAsyncStatus() {
		AsyncStatus status = AsyncStatus.values()[this.GetStatus()];
		return status;
	}
	
	public AsyncStatus wait(int timeout) throws TimeoutException {
		int waitResult = this.Wait(timeout);
		if (waitResult == -1) {
			throw new TimeoutException();
		}
		return  AsyncStatus.values()[waitResult];
	}
		
	public void addCompletedListener(CompletedEventListener listener) {
		if (listeners == null) {
			listeners = new java.util.concurrent.CopyOnWriteArrayList<CompletedEventListener>();
		}
		
		listeners.add(listener);
		
		if (!isListenerProxy()) {
			addEventListenerProxy();
		}
	}
	
	public void removeCompletedListener(CompletedEventListener listener)
	{
		if (listeners != null) { 
			listeners.remove(listener);
			
			if (listeners.isEmpty()) {
				removeEventListenerProxy();
			}
		}		
	}
	
	@Override
	void onNativeEvent(EventArgs eventArgs) {
		if (eventArgs instanceof CompletedEventArgs) {
			CompletedEvent completedEvent = new CompletedEvent();
			
			for (Iterator<CompletedEventListener> iter = listeners.iterator(); iter.hasNext(); ) {
				CompletedEventListener listener = iter.next();
				listener.onCompleted(completedEvent);
			}
		}
	}
}
