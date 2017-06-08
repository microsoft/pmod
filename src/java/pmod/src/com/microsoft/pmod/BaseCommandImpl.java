/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:BaseCommandImpl.java
****/
package com.microsoft.pmod;

import java.util.Iterator;

public class BaseCommandImpl extends FoundationObjectImpl {

	private java.util.concurrent.CopyOnWriteArrayList<CanExecuteChangedEventListener> listeners;

	// Private Native methods
	private native Object Execute(Object parameter);
	private native boolean CanExecute(Object parameter);
	private native int GetState();
	
	/**
	 * create a listener to the 
	 */
	@Override
	native long createListenerProxy(EventSink eventSink);

	  /**
	   * Internal constructor invoked by JNI
	   * @param interfaceValue
	   */
	BaseCommandImpl(long interfaceValue) {
		super(interfaceValue);
	}
	
	public int getState() {
		return GetState();
	}
	
	protected Object executeInternal(Object parameter) {
		Object coreValue = executeFoundationValue(parameter, new ExecuteFoundationValues() {
			public Object execute(Object[] values) {
				return Execute(values[0]);
			}
		});
        
		return fromFoundationValue(coreValue);
	}
	
	protected boolean canExecuteInternal(Object parameter) {
		Object coreValue = executeFoundationValue(parameter, new ExecuteFoundationValues() {
			public Object execute(Object[] values) {
				return CanExecute(values[0]);
			}
		});
        
		return ((Boolean)fromFoundationValue(coreValue)).booleanValue();
	}
	
	public void addCanExecuteChangedListener(CanExecuteChangedEventListener listener) {
		if (listeners == null) {
			listeners = new java.util.concurrent.CopyOnWriteArrayList<CanExecuteChangedEventListener>();
		}
        
		listeners.add(listener);
        
		if (!isListenerProxy()) {
			addEventListenerProxy();
		}
	}
	
	public void removeCanExecuteChangedListener(CanExecuteChangedEventListener listener)
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
		if (eventArgs instanceof CanExecuteChangedEventArgs) {
			CanExecuteChangedEventArgs canExecuteChangedEventArgs = 
                (CanExecuteChangedEventArgs)eventArgs;
			
			CanExecuteChangedEvent canExecuteChangedEvent =
				new CanExecuteChangedEvent(
                    canExecuteChangedEventArgs.getOldState(),
                    canExecuteChangedEventArgs.getNewState()
                    );
			
			for (Iterator<CanExecuteChangedEventListener> iter = listeners.iterator(); iter.hasNext(); ) {
				CanExecuteChangedEventListener listener = iter.next();
				listener.canExecuteChanged(canExecuteChangedEvent);
			}
		}
	}
}
