/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObservableCollection.java
****/
package com.microsoft.pmod;


public interface ObservableCollection<T> extends FoundationObject,Iterable<T> {

	public T getItem(int index);
	public int size();	
	public T[] toArray(T[] contents);
	public void addNotifyCollectionChangedListener(NotifyCollectionChangedListener listener);
	public void removeNotifyCollectionChangedListener(NotifyCollectionChangedListener listener);
}
