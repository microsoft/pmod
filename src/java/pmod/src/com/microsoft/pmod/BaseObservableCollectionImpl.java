/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:BaseObservableCollectionImpl.java
****/
package com.microsoft.pmod;

import java.util.Iterator;

class BaseIteratorImpl<T> extends InspectableObject implements Iterator<T>
{
	private native FoundationObjectImpl GetCurrent();
	private native boolean GetHasCurrent();
	private native boolean MoveNext();
	
	private BaseObservableCollectionImpl _collectionModelImpl;
	
	BaseIteratorImpl(BaseObservableCollectionImpl collectionModelImpl,long interfaceValue) {
		super(interfaceValue);
		this._collectionModelImpl = collectionModelImpl;
	}

	@Override
	public boolean hasNext() {
		return GetHasCurrent();
	}

	@SuppressWarnings("unchecked")
	@Override
	public T next() {
		FoundationObjectImpl o = GetCurrent();
		MoveNext();
		return (T)this._collectionModelImpl.fromFoundationValue(o);
	}
	
	@Override
	public void remove() {
	}
}

public class BaseObservableCollectionImpl extends FoundationObjectImpl {
	
	private java.util.concurrent.CopyOnWriteArrayList<NotifyCollectionChangedListener> listeners;
	
	// Private Native methods
	protected native int GetCount();
	protected native FoundationObjectImpl GetItem(int index);
	protected native FoundationObjectImpl[] GetItems();
	
	private native long GetFirst();

	/**
	 * create a listener to the Collection
	 */
	@Override
	native long createListenerProxy(EventSink eventSink);

	  /**
	   * Internal constructor invoked by JNI
	   * @param interfaceValue
	   */
	BaseObservableCollectionImpl(long interfaceValue) {
		super(interfaceValue);
	}
		
	public void addNotifyCollectionChangedListener(NotifyCollectionChangedListener listener) {
		if (listeners == null) {
			listeners = new java.util.concurrent.CopyOnWriteArrayList<NotifyCollectionChangedListener>();
		}
        
		listeners.add(listener);
        
		if (!isListenerProxy()) {
			addEventListenerProxy();
		}
	}
	
	public void removeNotifyCollectionChangedListener(NotifyCollectionChangedListener listener) {
		if (listeners != null) {
			listeners.remove(listener);
            
			if (listeners.isEmpty()) {
				removeEventListenerProxy();
			}
		}		
	}
	
	protected <T> Iterator<T> iteratorInternal() {
		return new BaseIteratorImpl<T>(this, GetFirst());
	}
	
	@Override
	void onNativeEvent(EventArgs eventArgs) {
		if (eventArgs instanceof NotifyCollectionChangedEventArgs) {
			NotifyCollectionChangedEventArgs notifyCollectionChangedEventArgs = 
                (NotifyCollectionChangedEventArgs)eventArgs;
			
			NotifyCollectionModelChangedAction action = NotifyCollectionModelChangedAction.values()[notifyCollectionChangedEventArgs.getAction()];
			Object[] oldCoreValues = notifyCollectionChangedEventArgs.getOldItems();
			Object[] newCoreValues = notifyCollectionChangedEventArgs.getNewItems();
			
			Object[] oldValues = null; 
			Object[] newValues = null; 

			if (oldCoreValues != null) {
				oldValues = fromFoundationValues(new Object[oldCoreValues.length],oldCoreValues);
			}
            
			if (newCoreValues != null) {
				newValues = fromFoundationValues(new Object[newCoreValues.length],newCoreValues);
			}
            
			NotifyCollectionChangedEvent notifyCollectionChangedEvent =
                new NotifyCollectionChangedEvent(
                    action,
                    notifyCollectionChangedEventArgs.getOldStartingIndex(),
                    notifyCollectionChangedEventArgs.getNewStartingIndex(),
                    oldValues,
                    newValues
                    );
			
			for (Iterator<NotifyCollectionChangedListener> iter = listeners.iterator(); iter.hasNext(); ) {
				NotifyCollectionChangedListener listener = iter.next();
				listener.notifyCollectionChanged(notifyCollectionChangedEvent);
			}
		}
	}
}
