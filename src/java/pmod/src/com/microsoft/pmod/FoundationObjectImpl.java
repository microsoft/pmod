/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:FoundationObjectImpl.java
****/
package com.microsoft.pmod;

/**
 * Class FoundationObjectImpl
 * @author rodrigov
 * Represent an FoundationObject interface on 'Foundation' library
 */
public class FoundationObjectImpl extends FactoryObject implements FoundationObject {
	
	// native methods
	public native int getUniqueId();
	public native ObjectTypeInfo getTypeInfo();	
	
	public FoundationObjectImpl(long interfaceValue) {
		super(interfaceValue);
	}
	
	protected FoundationObjectImpl(FoundationObjectImpl other, ClassFactory classFactory) {
		super(other, classFactory);
	}	
	
	void onNativeEvent(EventArgs eventArgs)
	{
		throw new java.lang.UnsupportedOperationException();
	}

	long createListenerProxy(EventSink eventSink)
	{
		throw new java.lang.UnsupportedOperationException();
	}

}
