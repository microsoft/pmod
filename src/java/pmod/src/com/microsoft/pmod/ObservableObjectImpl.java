/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObservableObjectImpl.java
****/
package com.microsoft.pmod;

import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import java.util.Iterator;

public class ObservableObjectImpl extends FoundationObjectImpl implements ObservableObject
{
	
	private  java.util.concurrent.CopyOnWriteArrayList<EventModelEventListener> eventModelListeners;
	
	private java.beans.PropertyChangeSupport propertyChangeSupport;
	private long eventModelListenerProxy;
	
    //
	// native methods
    //
	
    // Properties by Id
	private native void setPropertyById(int propertyId, Object value);
	private native Object getPropertyById(int propertyId);
	
	// Properties by Name
	private native void setPropertyByName(String propertyName, Object value);
	private native Object getPropertyByName(String propertyName);
	
	private native Object Invoke(int methodId, Object[] parameters);
	
	/**
	 * create a listener to the 
	 */
	native long createListenerProxy(EventSink eventSink);
	native long createEventModelListenerProxy(EventSink eventSink);

	public ObservableObjectImpl(ObservableObjectImpl other, ClassFactory classFactory) {
		super(other, classFactory);
	}
	
    /**
     * Internal constructor invoked by JNI
     * @param interfaceValue
     */
	public ObservableObjectImpl(long interfaceValue) {
		super(interfaceValue);
	}
	
	public ObservableObjectInfo getPropertyModelTypeInfo() {
		return (ObservableObjectInfo)getTypeInfo();
	}
	
	public Object invoke(final int methodId, Object[] parameters) {
		Object coreValue = executeFoundationValues(parameters, new ExecuteFoundationValues() {
			public Object execute(Object[] values) {
				return Invoke(methodId, values);
			}
		});
        
		return fromFoundationValue(coreValue);
	}

	public void setProperty(final int propertyId, Object value) {
		executeFoundationValue(value, new ExecuteFoundationValues() {
			public Object execute(Object[] values) {
				setPropertyById(propertyId, values[0]);
				return null;
			}
		});
	}
	
	public Object getProperty(int propertyId) {
		return fromFoundationValue(getPropertyById(propertyId));
	}
	
	public void setProperty(final String propertyName, Object value) {
		executeFoundationValue(value, new ExecuteFoundationValues() {
			public Object execute(Object[] values) {
				setPropertyByName(propertyName, values[0]);
				return null;
			}
		});
	}
	
	public Object getProperty(String propertyName) {
		return fromFoundationValue(getPropertyByName(propertyName));
	}
	
	public void addPropertyChangeListener(java.beans.PropertyChangeListener listener)
	{
		if (propertyChangeSupport == null) {
			propertyChangeSupport = new java.beans.PropertyChangeSupport(this);						
		}
        
		propertyChangeSupport.addPropertyChangeListener(listener);
        
		if (propertyChangeSupport.hasListeners(null) && !isListenerProxy()) {
			addEventListenerProxy();
		}
	}
	
	public void removePropertyChangeListener(java.beans.PropertyChangeListener listener)
	{
		if (propertyChangeSupport != null)
		{
			propertyChangeSupport.removePropertyChangeListener(listener);
			if (!propertyChangeSupport.hasListeners(null))
			{
				removeEventListenerProxy();
			}
		}		
	}
	
	boolean isEventModelListenerProxy() {
        return eventModelListenerProxy != 0;
    }

	public void addEventModelListener(EventModelEventListener listener) {
		if (eventModelListeners == null) {
			eventModelListeners = new java.util.concurrent.CopyOnWriteArrayList<EventModelEventListener>();
		}
        
		eventModelListeners.add(listener);
        
		if (!isEventModelListenerProxy()) {
			addEventModelEventListenerProxy();
		}
	}
	
	public void removeEventModelListener(EventModelEventListener listener) {
		if (eventModelListeners != null) {
			eventModelListeners.remove(listener);
            
			if(eventModelListeners.isEmpty()) {
				removeEventModelEventListenerProxy();
			}
		}		
	}

	void addEventModelEventListenerProxy() {
		EventSink eventSink = new EventSink(this);
		eventModelListenerProxy = createEventModelListenerProxy(eventSink);
	}
	
	void removeEventModelEventListenerProxy() {
		if (eventModelListenerProxy != 0) {
			releaseListenerProxy(eventModelListenerProxy);
			eventModelListenerProxy = 0;
		}
	}

	void onNativeEvent(EventArgs eventArgs)
	{
		if (eventArgs instanceof PropertyChangedEventArgs) {
			assert propertyChangeSupport != null;
			PropertyChangedEventArgs propertyChangedEventArgs = (PropertyChangedEventArgs)eventArgs;
			int propertyId = propertyChangedEventArgs.getProperty();
			ObservableObjectInfo propertyModelInfo = getPropertyModelTypeInfo();
			String propertyName = propertyModelInfo.getPropertyById(propertyId).getName();
			
			Object oldValue = this.fromFoundationValue(propertyChangedEventArgs.getOldValue());
			Object newValue = this.fromFoundationValue(propertyChangedEventArgs.getNewValue());
			
			// fire using 'propertyChangeSupport'
			PropertyChangeListener[] listeners = propertyChangeSupport.getPropertyChangeListeners();
			if (listeners != null) {
				PropertyChangeEvent event = new PropertyChangeEvent(this, propertyName, oldValue, newValue);
				for (PropertyChangeListener listener : listeners) {
                    listener.propertyChange(event);
				}
			}
		}
		else if (eventArgs instanceof EventModelEventArgs) {
			EventModelEventArgs eventModelEventArgs = 
                (EventModelEventArgs)eventArgs;
			
			EventModelEvent eventModelEvent = 
				new	EventModelEvent(
                    eventModelEventArgs.getEvent(),
                    eventModelEventArgs.getEventArgs()
                    );
			
			for (Iterator<EventModelEventListener> iter = eventModelListeners.iterator(); iter.hasNext(); ) {
				EventModelEventListener listener = iter.next();
				listener.onEventModel(eventModelEvent);
			}
		}
	}

}
