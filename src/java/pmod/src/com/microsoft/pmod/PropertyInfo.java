/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PropertyInfo.java
****/
package com.microsoft.pmod;

import com.microsoft.pmod.PropertyTypeInfo;

public class PropertyInfo extends PropertyTypeInfo {

	/**
	   * Internal constructor invoked by JNI
	   * @param interfaceValue
	   */
	PropertyInfo(long interfaceValue) {
		super(interfaceValue);
	}
	
	public native int getId();
	public native String getName();

}
