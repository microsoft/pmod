/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObservableObjectTest.java
****/
package com.microsoft.pmod.test;

import java.beans.PropertyChangeEvent;

import com.microsoft.pmod.*;

public class ObservableObjectTest extends TestCaseBase {

	public void testCreateModelFactory() {		
		
		ObservableObject propertyModel = (ObservableObject)ObjectFactory.activateObject(
				Constants.IID_IObservableObject, null, null);
		
		String iidType = propertyModel.getTypeInfo().getType();
		assertEquals(iidType, Constants.IID_IObservableObject);
				
		// test boolean value
		propertyModel.setProperty("property1", true);		
		assertEquals(propertyModel.getProperty("property1"),true);
				
		// test string value
		propertyModel.setProperty("property2", "hello");
		assertEquals(propertyModel.getProperty("property2"),"hello");
		
		// test integer value
		propertyModel.setProperty("property3", 100);
		assertEquals(propertyModel.getProperty("property3"),100);
		
		final SimpleRef<PropertyChangeEvent> simpleRef = new SimpleRef<PropertyChangeEvent>();
		
		// test event support
		java.beans.PropertyChangeListener listener = new java.beans.PropertyChangeListener() {
			
			@Override
			public void propertyChange(PropertyChangeEvent evt) {
				simpleRef.setRef(evt);
			}
		};
		propertyModel.addPropertyChangeListener(listener);
		propertyModel.setProperty("property3", 200);	
		
		assertEquals(simpleRef.getRef().getPropertyName(),"property3");
		assertEquals(simpleRef.getRef().getOldValue(),100);
		assertEquals(simpleRef.getRef().getNewValue(),200);
		
		propertyModel.removePropertyChangeListener(listener);
		
	}

}
