/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:Dictionary.java
****/
package com.microsoft.pmod;


import java.util.Collection;
import java.util.Map;
import java.util.Set;

public final class Dictionary extends FactoryObject implements java.util.Map<String,Object>
{	
	private native void createDictionary();
	private native void insertInternal(String key,Object value);
	private native Object lookupInternal(String key);
    private native void removeInternal(String key);
    private native boolean hasKeyInternal(String key);
    private native boolean clearInternal();

	
	Dictionary(long interfaceValue)
	{
		super(interfaceValue);
	}

	public Dictionary()
	{
		createDictionary();
	}


	public int size()
	{
		throw new UnsupportedOperationException();
	}

	public boolean isEmpty()
	{
		throw new UnsupportedOperationException();
	}

    @Override
    public boolean containsKey(Object key) {
        return hasKeyInternal(key.toString());
    }

    @Override
    public boolean containsValue(Object value) {
        return false;
    }

    @Override
    public Object get(Object key) {
        return 	fromFoundationValue(lookupInternal(key.toString()));
    }

    @Override
    public Object put(final String key, Object value) {
        executeFoundationValue(value, new ExecuteFoundationValues() {
            @Override
            public Object execute(Object[] values) {
                insertInternal(key,values[0]);
                return null;
            }
        });
        return null;
    }

    @Override
    public Object remove(Object key) {
        removeInternal(key.toString());
        return null;
    }

    @Override
    public void putAll(Map<? extends String, ?> m) {

    }

    @Override
    public void clear() {
        clearInternal();
    }

    @Override
    public Set<String> keySet() {
        return null;
    }

    @Override
    public Collection<Object> values() {
        return null;
    }

    @Override
    public Set<Entry<String, Object>> entrySet() {
        return null;
    }


    @Override
	void onNativeEvent(EventArgs eventArgs) {
		
	}

	@Override
	long createListenerProxy(EventSink eventSink) {
		return 0;
	}
}
