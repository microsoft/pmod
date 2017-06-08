/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObservableList.java
****/
package com.microsoft.pmod;


public interface ObservableList<T> extends ObservableCollection<T> {
	
	public void setItem(int index,T value);
	public boolean add(T item);
	public boolean remove(T value);
	public void clear();
}
