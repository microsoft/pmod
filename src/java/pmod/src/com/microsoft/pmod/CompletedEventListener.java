/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:CompletedEventListener.java
****/
package com.microsoft.pmod;

import java.util.EventListener;

import com.microsoft.pmod.CompletedEvent;

public interface CompletedEventListener extends EventListener {
	public void onCompleted(CompletedEvent eventArgs);
}

