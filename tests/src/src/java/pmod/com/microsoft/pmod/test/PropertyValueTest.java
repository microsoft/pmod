/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PropertyValueTest.java
****/
package com.microsoft.pmod.test;

import com.microsoft.pmod.PropertyType;
import com.microsoft.pmod.PropertyValue;

public class PropertyValueTest extends TestCaseBase {

	public void testPropertyValueBoolean() {
		PropertyValue pv = new PropertyValue(true);
		assertEquals(pv.getType(), PropertyType.Boolean);
		assertEquals(pv.getBoolean(),true);	
		
		pv = new PropertyValue(new boolean[] { true,false});
		assertEquals(pv.getType(), PropertyType.BooleanArray);
		boolean[] values = pv.getBooleanArray();
		assertEquals(values[0],true);		
		assertEquals(values[1],false);		
		
	}
	
	public void testPropertyValueByte() {		
		PropertyValue pv = new PropertyValue((byte)100);
		assertEquals(pv.getType(), PropertyType.UInt8);
		assertEquals(pv.getByte(),100);
		
		pv = new PropertyValue(new byte[] { 10,20,30});
		assertEquals(pv.getType(), PropertyType.UInt8Array);
		byte[] byteArray = pv.getByteArray();
		assertEquals(byteArray[0],10);		
		assertEquals(byteArray[1],20);		
		assertEquals(byteArray[2],30);
		
		Object o = new byte[] { 100,101 };
		pv = new PropertyValue(o);		
	}
	
	public void testPropertyValueString() {		
		PropertyValue pv = new PropertyValue("hello");
		assertEquals(pv.getType(), PropertyType.String);
		assertEquals(pv.getString(),"hello");
		
		pv = new PropertyValue(new String[] { "hello","world"});
		assertEquals(pv.getType(), PropertyType.StringArray);
		String[] stringArray = pv.getStringArray();
		assertEquals(stringArray[0],"hello");		
		assertEquals(stringArray[1],"world");		
		
	}
	
	public void testPropertyValueObjectArray() {	
		PropertyValue pv = new PropertyValue(new Object[] {"hello",100});
		assertEquals(pv.getType(), PropertyType.InspectableArray);
		Object[] values = pv.getObjectArray();
		assertEquals(values[0],"hello");		
		assertEquals(values[1],100);				
	}

	public void testPropertyValueDateTime() {
		java.util.Date date = new java.util.Date();	        
		PropertyValue pv = new PropertyValue(date);
		assertEquals(pv.getType(), PropertyType.DateTime);
		java.util.Date date2 = pv.getDateTime();
		assertTrue(date.equals(date2));
	}
}
