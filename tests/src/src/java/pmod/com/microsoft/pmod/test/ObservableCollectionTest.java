/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObservableCollectionTest.java
****/
package com.microsoft.pmod.test;


import java.util.*;

import com.microsoft.pmod.*;

public class ObservableCollectionTest extends TestCaseBase {

	public void testFactoryCollections() {		
		
		@SuppressWarnings("unchecked")
		ObservableList<String> listModel = (ObservableList<String>)ObjectFactory.activateObject(
				ObservableCollectionFactoryTypes.IID_String,
				new Integer(0),
				null);
		
		listModel.add("hello");
		listModel.add("-world");
		
		assertEquals(listModel.size(),2);		
		assertEquals(listModel.getItem(0),"hello");
		assertEquals(listModel.getItem(1),"-world");
		
		String[] values = listModel.toArray(new String[0]);
		assertEquals(values.length,2);		
		assertEquals(values[0],"hello");
		assertEquals(values[1],"-world");
		
		Vector<String> iterValues = new Vector<String>();
		Iterator<String> itr = listModel.iterator();
		while(itr.hasNext()) {
			String element = itr.next();
			iterValues.add(element);
		}
		assertEquals(iterValues.size(),2);		
		assertEquals(iterValues.get(0),"hello");
		assertEquals(iterValues.get(1),"-world");
	    
		listModel.clear();
		assertEquals(listModel.size(),0);		
	}

}
