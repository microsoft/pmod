/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:BaseAsyncOperation.java
****/
package com.microsoft.pmod;

import java.util.concurrent.TimeoutException;


public interface BaseAsyncOperation extends FoundationObject {
	
	public AsyncStatus getAsyncStatus();
	public String getError();
	public AsyncStatus wait(int timeout) throws TimeoutException;
	public void addCompletedListener(CompletedEventListener listener);
	public void removeCompletedListener(CompletedEventListener listener);
    
}
