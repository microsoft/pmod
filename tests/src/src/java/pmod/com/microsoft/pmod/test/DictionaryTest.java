/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:DictionaryTest.java
****/
package com.microsoft.pmod.test;

import com.microsoft.pmod.Dictionary;

public class DictionaryTest extends TestCaseBase {

	public void testDictionary() {
		Dictionary dictionary = new Dictionary();
		dictionary.put("key1", 100);
		
		Object value = dictionary.get("key1");
		assertEquals(value, 100);
	}
	

}
