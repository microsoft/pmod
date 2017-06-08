/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObservableCollectionImpl.java
****/
package com.microsoft.pmod;

import java.util.Iterator;

public class ObservableCollectionImpl<T> extends BaseObservableCollectionImpl implements ObservableCollection<T> {
	
	  /**
	   * Internal constructor invoked by JNI
	   * @param interfaceValue
	   */
	ObservableCollectionImpl(long interfaceValue) {
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
}
