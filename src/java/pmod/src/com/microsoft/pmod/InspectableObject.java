/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:InspectableObject.java
****/
package com.microsoft.pmod;


public class InspectableObject {
	
	protected long _interface = 0;

	/**
	 * Release the attached interface
	 */
	public native void release();
	public native static Object createObjectInternal(long pInterface);

	public static <T extends InspectableObject> T createObject(long pInterface) {
		return (T)createObjectInternal(pInterface);
	}

	protected InspectableObject()
	{	
	}
	
	InspectableObject(InspectableObject  other)
	{
		// transfer ownership
		this._interface = other._interface;
		other._interface = 0;
	}	
	
	InspectableObject(long interfaceValue)
	{
		this._interface = interfaceValue;
	}	
	
	long getInterface()
	{
		return _interface;
	}
	
	void setInterface(long interfaceValue)
	{
		_interface = interfaceValue;
	}
	
	void releaseIf()
	{
		if (_interface != 0) release();		
	}
	
	protected void finalize()
	{
		releaseIf();		
	}
}
