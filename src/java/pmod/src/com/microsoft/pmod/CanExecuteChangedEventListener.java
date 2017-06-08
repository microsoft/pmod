/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:CanExecuteChangedEventListener.java
****/
package com.microsoft.pmod;

import java.util.EventListener;

import com.microsoft.pmod.CanExecuteChangedEvent;

public interface CanExecuteChangedEventListener extends EventListener {
	public abstract void canExecuteChanged(CanExecuteChangedEvent canExecuteChangedEvent);
}
