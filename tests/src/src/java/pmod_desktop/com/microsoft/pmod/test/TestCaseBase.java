/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:TestCaseBase.java
****/
package com.microsoft.pmod.test;

import junit.framework.TestCase;

public abstract class TestCaseBase extends TestCase {
	static {
	    System.loadLibrary("pmod");
		System.loadLibrary("pmod_jni");
		System.loadLibrary("pmod_lib");
	  }
}
