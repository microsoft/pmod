/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:AsyncOperationImpl.java
****/
package com.microsoft.pmod;

import java.lang.Throwable;
import java.util.concurrent.CancellationException;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;

public class AsyncOperationImpl<T> extends BaseAsyncOperationImpl implements AsyncOperation<T> {
    
    /**
     * Internal constructor invoked by JNI
     * @param interfaceValue
     */
	AsyncOperationImpl(long interfaceValue) {
		super(interfaceValue);
	}
		
	@SuppressWarnings("unchecked")
	public T getResult() {
		return (T)fromFoundationValue(this.GetResult(-1));
	}

	@SuppressWarnings("unchecked")
	public T getResult(int timeout) {
		return (T)fromFoundationValue(this.GetResult(timeout));
	}
	
	// Future<Object> implementation
	public boolean cancel(boolean mayInterruptIfRunning)
	{
		if (!mayInterruptIfRunning) {
			return false;
		}

		if (this.getAsyncStatus() != AsyncStatus.Started) {
			return false;
		}
		
		return this.Cancel();
	}
	
	public boolean isCancelled() {
		return (this.getAsyncStatus() == AsyncStatus.Canceled);
	}
	
	public boolean isDone() {
		return (this.getAsyncStatus() != AsyncStatus.Started);
	}
	
	public T get() throws CancellationException,
                               ExecutionException,
                               InterruptedException {
		try {
			return this.get((long)Integer.MAX_VALUE, TimeUnit.MILLISECONDS);
		}
        catch (TimeoutException toe) {
			return null;
		}
	}
	
	public T get(long timeout, TimeUnit unit) throws CancellationException,
	                                                      ExecutionException,
	                                                      InterruptedException,
	                                                      TimeoutException {
		long timeoutMilliSecs = TimeUnit.MILLISECONDS.convert(timeout, unit);
		if (timeoutMilliSecs > (long)Integer.MAX_VALUE || timeoutMilliSecs == Long.MIN_VALUE) {
			timeoutMilliSecs = (long)Integer.MAX_VALUE;
		}
		
		AsyncStatus status;
		try {
			int waitResult = this.Wait((int)timeoutMilliSecs);
			if (waitResult == -1) {
				throw new TimeoutException();
			}
			status = AsyncStatus.values()[waitResult];
		}
        catch (TimeoutException toe) {
			throw toe;
		}
        catch (Exception e) {
			throw new InterruptedException(e.toString());
		}
		
		if (status == AsyncStatus.Canceled) {
			throw new CancellationException();
		}
		
		if (status == AsyncStatus.Error) {
			throw new ExecutionException(new Throwable(this.getError()));
		}
		
		return this.getResult(0);
	}
}
