/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:EventSink.java
****/
package com.microsoft.pmod;

import com.microsoft.pmod.EventArgs;
import com.microsoft.pmod.FactoryObject;

/**
 * Helper class to receive the 'Core' Event
 * @author rodrigov
 *
 */
class EventSink {

	EventSink(FactoryObject target) {
		targetRef = new java.lang.ref.WeakReference<FactoryObject>(target);
	}
	
	/**
	 * Method callback invoked by the JNI layer when a 'Core' event is being
	 * dispatched on the native side
	 * @param eventArgs
	 */
	void invokeCallback(EventArgs eventArgs) {
		FactoryObject target = targetRef.get();
		try {
			if (target != null) {
				target.onNativeEvent(eventArgs);
			}
		}
		finally {
			eventArgs.release();
		}
	}
	
	private java.lang.ref.WeakReference<FactoryObject> targetRef;
    
}
