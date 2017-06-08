/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:AsyncOperation.java
****/
package com.microsoft.pmod;

import java.util.concurrent.Future;


public interface AsyncOperation<T> extends BaseAsyncOperation, Future<T> {
	
	public T getResult();
	public T getResult(int timeout);    
}
