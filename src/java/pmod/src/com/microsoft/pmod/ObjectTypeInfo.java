/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObjectTypeInfo.java
****/
package com.microsoft.pmod;

import com.microsoft.pmod.InspectableObject;
import com.microsoft.pmod.ObjectTypeInfo;

public class ObjectTypeInfo extends InspectableObject {
	
	// native methods
	public native String getName();
	public native String getType();
	public native ObjectTypeInfo getBaseType();
	
	/*
	 * Internal constructor invoked by JNI
	 * @param interfaceValue
	 */
	ObjectTypeInfo(long interfaceValue) {
		super(interfaceValue);
	}
}
