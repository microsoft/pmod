/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PropertyValue.java
****/
package com.microsoft.pmod;

import java.nio.charset.Charset;
import java.util.List;

public final class PropertyValue extends InspectableObject
{
	private final Charset UTF8_CHARSET = Charset.forName("UTF-8");

    // Single Value support
    private native void createEmpty();
    private native void createByte(byte b);
    private native void createChar(char c);
    private native void createBoolean(boolean bool);
    private native void createShort(short value);
    private native void createInt(int value);
    private native void createLong(long value);
    private native void createString(String str);
    private native void createSingle(float value);
    private native void createDouble(double value);
    private native void createDateTime(java.util.Date value);

    // Array Value support
    private native void createByteArray(byte[] values);
    private native void createCharArray(char[] values);
    private native void createBooleanArray(boolean[] values);
    private native void createShortArray(short[] values);
    private native void createIntArray(int[] values);
    private native void createLongArray(long[] values);
    private native void createSingleArray(float[] values);
    private native void createDoubleArray(double[] values);
    private native void createStringArray(String[] values);
    private native void createObjectArray(Object[] values);

    // get Type
    private native int getTypeInternal();

    private native byte[] getStringInternal();
    private native Object[] getStringArrayInternal();

    // Public Native methods
    public native byte getByte();
    public native boolean getBoolean();
    public native char getChar();
    public native short getShort();
    public native int getInt();
    public native long getLong();
    public native float getSingle();
    public native double getDouble();
    public native java.util.Date getDateTime();

    public native byte[] getByteArray();
    public native char[] getCharArray();
    public native boolean[] getBooleanArray();
    public native short[] getShortArray();
    public native int[] getIntArray();
    public native long[] getLongArray();
    public native float[] getFloatArray();
    public native double[] getDoubleArray();

    native Object[] getObjectArrayInternal();

    public PropertyValue() {
    }

    /**
    * Internal static ctor invoked by JNI
    * @param interfaceValue
    */
    static PropertyValue createPropertyValue(long interfaceValue) {
        PropertyValue propertyValue = new PropertyValue();
        propertyValue.setInterface(interfaceValue);
        return propertyValue;
    }

    static java.util.Date toDate(int year, int month, int dayOfMonth, int hourOfDay, int minute, int second, int milliSecs) {
        java.util.GregorianCalendar calendar = new java.util.GregorianCalendar(year, month, dayOfMonth, hourOfDay, minute, second);
        calendar.set(java.util.Calendar.MILLISECOND, milliSecs);
        return calendar.getTime();
    }

    static java.util.Calendar toCalendar(java.util.Date dt) {
        java.util.Calendar calendar = java.util.Calendar.getInstance();
        calendar.setTime(dt);	  
        return calendar;
    }

    public PropertyValue(byte byteVal) {
        createByte(byteVal);
    }

    public PropertyValue(boolean bool) {
        createBoolean(bool);
    } 

    public PropertyValue(short value) {
        createShort(value);
    }

    public PropertyValue(int value) {
        createInt(value);
    }

    public PropertyValue(long value) {
        createLong(value);
    }

    public PropertyValue(char ch) {
        createChar(ch);
    }

    public PropertyValue(String value) {
        createString(value);
    }

    public PropertyValue(Double value) {
        createDouble(value);
    }

    public PropertyValue(Float value) {
        createSingle(value);
    }

    public PropertyValue(java.util.Date value) {
        createDateTime(value);
    }

    public PropertyValue(byte[] values) {
        createByteArray(values);
    }

    public PropertyValue(boolean[] values) {
        createBooleanArray(values);
    }

    public PropertyValue(char[] values) {
        createCharArray(values);
    }

    public PropertyValue(short[] values) {
        createShortArray(values);
    }

    public PropertyValue(int[] values) {
        createIntArray(values);
    }
    
    public PropertyValue(long[] values) {
        createLongArray(values);
    }
    
    public PropertyValue(float[] values) {
        createSingleArray(values);
    }
    
    public PropertyValue(double[] values) {
        createDoubleArray(values);
    }
    
    public PropertyValue(String[] values) {
        createStringArray(values);
    }

    public PropertyValue(Object[] values) {
        createObjectArray(toPropertyValues(values));	  
    }

    public PropertyValue(Object o) {
        if (o == null) {
            createEmpty();
        } else if (o instanceof Byte) {
            createByte(((Byte)o).byteValue());
        } else if (o instanceof Character) {
            createChar(((Character)o).charValue());
        } else if (o instanceof Short) {
            createShort(((Short)o).shortValue());
        }  else if (o instanceof Integer) {
            createInt(((Integer) o).intValue());
        } else if (o instanceof Long) {
            createLong(((Long) o).longValue());
        } else if (o instanceof String) {
            createString((String) o);
        } else if (o instanceof Boolean) {
            createBoolean(((Boolean) o).booleanValue());
        } else if (o instanceof Double) {
            createDouble(((Double) o).doubleValue());
        } else if (o instanceof Float) {
            createSingle(((Float) o).floatValue());
        } else if (o instanceof java.util.Date) {
            createDateTime((java.util.Date) o);
        } else if (o instanceof byte[]) {
            createByteArray((byte[]) o);
        } else if (o instanceof char[]) {
            createCharArray((char[]) o);
        } else if (o instanceof boolean[]) {
            createBooleanArray((boolean[]) o);
        } else if (o instanceof short[]) {
            createShortArray((short[]) o);
        } else if (o instanceof int[]) {
            createIntArray((int[]) o);
        } else if (o instanceof long[]) {
            createLongArray((long[]) o);
        } else if (o instanceof float[]) {
            createSingleArray((float[]) o);
        } else if (o instanceof double[]) {
            createDoubleArray((double[]) o);
        } else if (o instanceof String[]) {
            createStringArray((String[]) o);
        } else if (o instanceof Object[]) {
            createObjectArray(toPropertyValues((Object[])o));
        } else {
            throw new java.lang.UnsupportedOperationException();
        }
    }

    static PropertyType toPropertyType(int type) {
        return PropertyType.toPropertyType(type);	  
    }

    public PropertyType getType() {
        if (_interface == 0)
        {
            return PropertyType.Empty;
        }
        
        int type = getTypeInternal();
        return toPropertyType(type);
    }
    
    public String getString()
    {
    	byte[] bytes = getStringInternal();
    	return new String(bytes, UTF8_CHARSET);
    }
    public String[] getStringArray()
    {
    	Object[] bytesArray = getStringArrayInternal();
    	String[] array = new String[bytesArray.length];
    	for(int index = 0;index < bytesArray.length;++index)
    	{
    		array[index] = new String((byte[])bytesArray[index], UTF8_CHARSET);
    	}   	
    	return array;
    }

    public Object toObject() {
        switch(getType())
        {
        case Boolean:
            return getBoolean();
        case Char16:
            return getChar();
        case DateTime:
            return getDateTime();
        case Empty:
            return null;
        case Int16:
        case UInt16:
            return getShort();
        case Int32:
        case UInt32:
            return getInt();
        case Int64:
        case UInt64:
            return getLong();
        case Single:
            return getSingle();
        case Double:
            return getDouble();
        case String:
            return getString();
        case UInt8:
            return getByte();
        case UInt8Array:
            return getByteArray();
        case Char16Array:
            return getCharArray();
        case BooleanArray:
            return getBooleanArray();
        case Int16Array:
        case UInt16Array:
            return getShortArray();
        case Int32Array:
        case UInt32Array:
            return getIntArray();
        case Int64Array:
        case UInt64Array:
            return getLongArray();
        case StringArray:
            return getStringArray();
        case SingleArray:
            return getFloatArray();
        case DoubleArray:
            return getDoubleArray();
        case InspectableArray:
            return getObjectArray();
        default:
            break;		  
        }
        
        throw new java.lang.UnsupportedOperationException();
    }

    public Object[] getObjectArray() {
        Object[] pvValues = getObjectArrayInternal();
        for (int index = 0; index < pvValues.length; ++index)
        {
            Object o = pvValues[index];
            if (o instanceof PropertyValue) {
                PropertyValue pvValue = (PropertyValue)o;
                o = pvValue.toObject();
                pvValue.release();
            }
            
            pvValues[index] = o;
        }
        
        return pvValues;
    }

    static Object[] toPropertyValues(Object[] values) {
        return toPropertyValues(values, null);
    }

    static Object[] toPropertyValues(Object[] values, List<PropertyValue> pvValues) {
        final int size = values.length;
        // create an array with Core values
        Object[] propValues = new Object[size];
        for (int index = 0; index < size; ++index) {
            Object value = values[index];

            // if value is Enum we will auto convert to Int
            if (value != null && value.getClass().isEnum()) {
                // it is an Enum Type we need to convert
                if (value instanceof EnumValue) {
                    value = new Integer(((EnumValue)value).getValue());
                }
            }
            
            boolean isPropertyValue = value instanceof PropertyValue;
            if (value != null && !isPropertyValue && !(value instanceof InspectableObject)) {
                PropertyValue pvValue = new PropertyValue(value);
                if (pvValues != null) {
                    pvValues.add(pvValue);
                }
                value = pvValue;
            }
            
            propValues[index] = value;
        }
        
        return propValues;
    }
}
