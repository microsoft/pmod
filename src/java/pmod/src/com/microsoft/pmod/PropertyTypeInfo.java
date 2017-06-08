/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PropertyTypeInfo.java
****/
package com.microsoft.pmod;

public class PropertyTypeInfo extends InspectableObject {
	/**
	   * Internal constructor invoked by JNI
	   * @param interfaceValue
	   */
	PropertyTypeInfo(long interfaceValue) {
		super(interfaceValue);
	}	

	private native int getPropertyTypeInternal();
	
	public PropertyType getPropertyType() {
		int type = getPropertyTypeInternal();
		return PropertyValue.toPropertyType(type);
	}

	public native ObjectTypeInfo getModelType();
}
