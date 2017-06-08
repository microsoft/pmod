/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObservableObject.java
****/
package com.microsoft.pmod;


public interface ObservableObject extends FoundationObject {
	
	public void setProperty(final int propertyId, Object value);
	public Object getProperty(int propertyId);

	public void setProperty(final String propertyName, Object value);
	public Object getProperty(String propertyName);
	
	public void addPropertyChangeListener(java.beans.PropertyChangeListener listener);
	public void removePropertyChangeListener(java.beans.PropertyChangeListener listener);

	public void addEventModelListener(EventModelEventListener listener);
	public void removeEventModelListener(EventModelEventListener listener);

}
