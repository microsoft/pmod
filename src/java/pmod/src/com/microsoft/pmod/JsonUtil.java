/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:JsonUtil.java
****/
package com.microsoft.pmod;

import com.microsoft.pmod.PropertyValue;


public class JsonUtil
{	
	private native static Object fromJsonInternal(String json);
	private native static String toJsonInternal(Object value);

	public static String toJson(Object value)
	{
		PropertyValue pv = new PropertyValue(value);
		try {
			return toJsonInternal(pv);
		}
		finally {
			pv.release();
		}
		
	}
	
	public static Object fromJson(String json)
	{
		Object value = fromJsonInternal(json);
		
		if(value instanceof PropertyValue)
		{
			return ((PropertyValue)value).toObject();
		}
		return value;
	}

}

