/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:SimpleRef.java
****/
package com.microsoft.pmod.test;


public class SimpleRef<T>
{
	private T _ref;
	
	public void setRef(T ref)
	{
		_ref = ref;
	}
	public T getRef()
	{
		return _ref;
	}
}

