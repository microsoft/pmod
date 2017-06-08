/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:NotifyCollectionChangedListener.java
****/
package com.microsoft.pmod;

import java.util.EventListener;

import com.microsoft.pmod.NotifyCollectionChangedEvent;

public interface NotifyCollectionChangedListener extends EventListener {
	public abstract void notifyCollectionChanged(NotifyCollectionChangedEvent notifyCollectionChangedEvent);
}
