/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:BaseObservableListImpl.java
****/
package com.microsoft.pmod;

public class BaseObservableListImpl extends BaseObservableCollectionImpl {

	// mutable native methods
	private native void SetItem(int index, Object value);
	private native void AppendItem(Object value);
	private native void InsertItem(int index, Object value);
    
	private native void RemoveItem(Object value);
	private native void RemoveAt(int index);
	private native void RemoveAll();

	/**
	   * Internal constructor invoked by JNI
	   * @param interfaceValue
	   */
	BaseObservableListImpl(long interfaceValue) {
		super(interfaceValue);
	}
	
	protected void setItemInternal(final int index, Object value) {
		executeFoundationValue(value, new ExecuteFoundationValues() {
			public Object execute(Object[] values) {
				SetItem(index, values[0]);
				return null;
			}
		});
	}	

	protected boolean addInternal(Object item) {
		executeFoundationValue(item, new ExecuteFoundationValues() {
			public Object execute(Object[] values) {
				AppendItem(values[0]);
				return null;
			}
		});
        
		return true;		
	}
	
	protected boolean removeInternal(Object value) {
		executeFoundationValue(value, new ExecuteFoundationValues() {
			public Object execute(Object[] values) {
				RemoveItem(values[0]);
				return null;
			}
		});
        
		return true;		
	}
	
	protected void clearInternal() {
		RemoveAll();
	}
}
