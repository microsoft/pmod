/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:TypePropertiesTest.java
****/
package com.microsoft.testmodel.test;

import com.microsoft.testmodel.*;
import com.microsoft.pmod.*;

public class TypePropertiesTest extends TestCaseBase {

	public void test() {
		TypeProperties typeProperties = (TypeProperties)ObjectFactory.activateObject(
				TypeInfo.TypeProperties.IID,
				null,
				new ClassFactory(FactoryTypes.getTypes()));

		typeProperties.setShortProperty((short)100);
		assertEquals(typeProperties.getShortProperty(),100);
		typeProperties.setIntProperty(200);
		assertEquals(typeProperties.getIntProperty(),200);
	}
	

}
