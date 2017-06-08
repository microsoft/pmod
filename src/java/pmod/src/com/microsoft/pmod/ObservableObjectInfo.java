/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObservableObjectInfo.java
****/
package com.microsoft.pmod;

import com.microsoft.pmod.InstanceFlags;
import com.microsoft.pmod.ObjectTypeInfo;
import com.microsoft.pmod.PropertyInfo;

public class ObservableObjectInfo extends ObjectTypeInfo {

	/**
	   * Internal constructor invoked by JNI
	   * @param interfaceValue
	   */
	ObservableObjectInfo(long interfaceValue) {
		super(interfaceValue);
	}
	
    public native PropertyInfo getPropertyById(int propertyId);
    public native PropertyInfo getPropertyByName(String propertyName);

    private native PropertyInfo[] getPropertiesInternal(int flags);
    
    public PropertyInfo[] getProperties(InstanceFlags flags) {
    	return getPropertiesInternal(flags.flags());
    }

}
