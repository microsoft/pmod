/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:DispatcherCallback.java
****/
package com.microsoft.pmod;

public interface DispatcherCallback {
	boolean hasThreadAccess();
	void runAsync(java.lang.Runnable runnable);
}
