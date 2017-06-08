/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObservableListImpl.java
****/
package com.microsoft.pmod;

import java.util.Iterator;

public class ObservableListImpl<T> extends BaseObservableListImpl implements ObservableList<T> {

	/**
	   * Internal constructor invoked by JNI
	   * @param interfaceValue
	   */
	ObservableListImpl(long interfaceValue) {
		super(interfaceValue);
	}
	
	@SuppressWarnings("unchecked")
	public T getItem(int index) {
		return (T)fromFoundationValue(GetItem(index));
	}
		
	public int size() {
		return GetCount();
	}
	
	public T[] toArray(T[] contents) {
		return fromFoundationValues(contents,GetItems());
	}
	
	@Override
	public Iterator<T> iterator() {
		return super.iteratorInternal();
	}

	public void setItem(final int index, T value) {
		super.setItemInternal(index, value);
	}	

	public boolean add(T item) {
		return super.addInternal(item);		
	}
	
	public boolean remove(T value) {        
		return super.removeInternal(value);		
	}
	
	public void clear() {
		super.clearInternal();
	}
}
