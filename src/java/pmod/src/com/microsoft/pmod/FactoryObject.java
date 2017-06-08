/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:FactoryObject.java
****/
package com.microsoft.pmod;

import java.lang.Object;
import java.lang.reflect.Array;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Iterator;
import java.util.List;

/**
 * Class:FactoryObject
 * @author rodrigov
 *
 */
public abstract class FactoryObject extends InspectableObject {
	
	private ClassFactory classFactory;
	private long eventListenerProxy;

	/**
	 * Release a Event Proxy
	 * @param pListenerProxyInterface
	 */
	native static void releaseListenerProxy(long pListenerProxyInterface);

	protected FactoryObject() {
	}
	
	FactoryObject(long interfaceValue) {
		super(interfaceValue);
	}
	
	FactoryObject(FactoryObject other, ClassFactory classFactory) {
		super(other);
		this.classFactory = classFactory;
	}
	
	public void dispose() {
		releaseIf();
	}
	
	public ClassFactory getClassFactory() {
		return this.classFactory;
	}

	void setClassFactory(ClassFactory classFactory) {
		this.classFactory = classFactory;
	}
		
	protected void finalize() {
		removeEventListenerProxy();
		super.finalize();	
	}

	protected static boolean toBoolean(Object value) {
		assert value instanceof Boolean;
		return ((Boolean)value).booleanValue();
	}
	
	protected static byte toByte(Object value) {
		assert value instanceof Byte;
		return ((Byte)value).byteValue();
	}
	
	protected static short toShort(Object value) {
		assert value instanceof Short;
		return ((Short)value).shortValue();
	}
	
	protected static int toInt(Object value) {
		assert value instanceof Integer;
		return ((Integer)value).intValue();
	}
	
	protected static long toLong(Object value) {
		assert value instanceof Long;
		return ((Long)value).longValue();
	}
	
	protected static float toFloat(Object value) {
		assert value instanceof Float;
		return ((Float)value).floatValue();
	}
	
	protected static double toDouble(Object value) {
		assert value instanceof Double;
		return ((Double)value).doubleValue();
	}
	
	protected static String toString(Object value) {
		assert value instanceof String;
		return (String)value;
	}
	
	protected static java.util.Date toDate(Object value) {
		assert value instanceof java.util.Date;
		return ((java.util.Date)value);
	}
	
	protected static Object toObject(Object value) {
		return value;
	}
	
	protected static byte[] toByteArray(Object value) {
		assert value instanceof byte[];
		return (byte[])value;
	}
	
	protected static short[] toShortArray(Object value) {
		assert value instanceof short[];
		return (short[])value;
	}
	
	protected static int[] toIntArray(Object value) {
		assert value instanceof int[];
		return (int[])value;
	}
	
	protected static long[] toLongArray(Object value) {
		assert value instanceof long[];
		return (long[])value;
	}
	
	protected static boolean[] toBooleanArray(Object value) {
		assert value instanceof boolean[];
		return (boolean[])value;
	}
	
	protected static String[] toStringArray(Object value) {
		assert value instanceof String[];
		return (String[])value;
	}
	
	protected static double[] toDoubleArray(Object value) {
		assert value instanceof double[];
		return (double[])value;
	}
	
	protected static Object[] toObjectArray(Object value) {
		assert value instanceof Object[];
		return (Object[])value;
	}
		
	@SuppressWarnings("unchecked")
	protected static <T extends FoundationObject> T toFoundationObject(Object value) {
		if (value == null) {
			return (T)null;
        }
        
		return (T)value;
	}

	protected static <T extends ObservableObject> T[] toFoundationObjectArray(Object value, Class<T[]> clazz) {
        if (value == null) {
            return (T[])null;
        }
        
        assert ObservableObject[].class.isAssignableFrom(value.getClass());
        Object[] values = toObjectArray(value);
        return Arrays.copyOf(values, values.length, clazz);
	}
	
	/**
	 * Execute a callback after converting each value to 'Core'
	 * @param values
	 * @param executeValues
	 * @return
	 */
	static Object executeFoundationValues(Object[] values, ExecuteFoundationValues executeValues) {
		List<PropertyValue> pvValues = new ArrayList<PropertyValue>();
		Object[] propValues = PropertyValue.toPropertyValues(values, pvValues);

		try {
			return executeValues.execute(propValues);
		}
		finally {
			// release temporary created PropertyValue values
			for (Iterator<PropertyValue> iterator = pvValues.iterator(); iterator.hasNext();) {
				iterator.next().release();
		    }
		}
	}
	
	static Object executeFoundationValue(Object value, ExecuteFoundationValues executeValues) {
		return executeFoundationValues(new Object[] {value}, executeValues);
	}
	
	/**
	 * Convert a 'Core' returned value into a standard 'Java' object
	 * @param value
	 * @return
	 */
	Object fromFoundationValue(Object value) {
		if (value instanceof PropertyValue) {
			PropertyValue pv = (PropertyValue)value;
			try {
				if (pv.getType() == PropertyType.InspectableArray) {
					Object[] pvValues = pv.getObjectArray();
					for (int index = 0; index < pvValues.length; ++index) {
						pvValues[index] = fromFoundationValue(pvValues[index]);
					}
                    
					return pvValues;
				}
                
				return pv.toObject();
			}
			finally {
				pv.release();
			}
		}
        
		if (classFactory != null && value instanceof FoundationObjectImpl) {
			return classFactory.getOrCreateObjectInternal((FoundationObjectImpl)value);
		}
        
		return value;		
	}

	@SuppressWarnings("unchecked")
	<T> T[] fromFoundationValues(T[] contents,Object[] coreValues) {
        int s = coreValues.length;
        if (contents.length < s) {
            T[] newArray
                = (T[]) Array.newInstance(contents.getClass().getComponentType(), s);
            contents = newArray;
        }
		for (int index = 0;index < s; ++index) {
			contents[index] = (T)fromFoundationValue(coreValues[index]);
		}    
        return contents;
    }
		
	boolean isListenerProxy() {
        return eventListenerProxy != 0;
    }
	
	void addEventListenerProxy() {
		EventSink eventSink = new EventSink(this);
		eventListenerProxy = createListenerProxy(eventSink);
	}
	
	void removeEventListenerProxy() {
		if (eventListenerProxy != 0) {
			releaseListenerProxy(eventListenerProxy);
			eventListenerProxy = 0;
		}
	}

	/**
	 * Handle a 'Native' event
	 * @param eventArgs
	 */
	abstract void onNativeEvent(EventArgs eventArgs);

	/**
	 * create an Event listener proxy on the JNI layer
	 * @param eventSink
	 * @return the 'raw' native interface
	 */
	abstract long createListenerProxy(EventSink eventSink);

}
