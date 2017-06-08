/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:EventModelEventListener.java
****/
package com.microsoft.pmod;

import java.util.EventListener;

public interface EventModelEventListener extends EventListener {
	public void onEventModel(EventModelEvent eventArgs);
}

