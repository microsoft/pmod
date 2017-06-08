/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ClassFactory.java
****/
package com.microsoft.pmod;

import java.lang.ref.WeakReference;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

public class ClassFactory {
	
	private HashMap<String, Class<? extends FoundationObjectImpl>> _objectTypes = new HashMap<String, Class<? extends FoundationObjectImpl>>();
	private ConcurrentHashMap<java.lang.Long, WeakReference<FoundationObjectImpl>> _cachedObjects = new ConcurrentHashMap<java.lang.Long, WeakReference<FoundationObjectImpl>>();
	
	public ClassFactory() {
	}
	
	public ClassFactory(Map<String, Class<? extends FoundationObjectImpl>> types) {
		addClassTypes(types);
	}
	
    public void addClassTypes(Map<String, Class<? extends FoundationObjectImpl>> types) {
        _objectTypes.putAll(types);
    }

	public <T extends FoundationObject> T getOrCreateObject(long pInterface)
	{
		FoundationObjectImpl obj = InspectableObject.createObject(pInterface);
		return (T)this.getOrCreateObjectInternal(obj);
	}

	public <T extends FoundationObject> T getActivationFactory(String activationId)
	{
		long pInterface = ActivationFactory.getFactory(activationId);
		return getOrCreateObject(pInterface);
	}

	FoundationObjectImpl getCachedObject(FoundationObjectImpl object) {
		long interfaceValue = object.getInterface();
		
		if (_cachedObjects.containsKey(interfaceValue)) {
			FoundationObjectImpl cachedObject = _cachedObjects.get(interfaceValue).get();
		
            if (cachedObject != null) {
				return cachedObject;
			}
		}       
		return null;
	}
		
	FoundationObjectImpl createObject(FoundationObjectImpl object) {
		assert object != null;
		
		FoundationObjectImpl typeObject = null;
        
		// attempt to lookup in our factory model types
		String typeId = object.getTypeInfo().getType();
        
		if (_objectTypes.containsKey(typeId)) {
			Class<? extends FoundationObjectImpl> type = _objectTypes.get(typeId);
			try {
				typeObject = type.getConstructor(object.getClass(), ClassFactory.class).newInstance(object, this);
			}
            catch(Exception e) {
				// TODO: log this error
			}
		}
		else {
			// we don't have a type info mapped so we use the same passed model
			typeObject = object;
			// ensure the passed Object reference our Class factory
			object.setClassFactory(this);
		}
        
		assert typeObject != null;
		// add into my cache
		_cachedObjects.put(object.getInterface(), new WeakReference<FoundationObjectImpl>(typeObject));
        
		return typeObject;
	}
	
	FoundationObjectImpl getOrCreateObjectInternal(FoundationObjectImpl object) {
		FoundationObjectImpl cachedObject = getCachedObject(object);
        
		if (cachedObject != null) {
			object.release();
			return cachedObject;
		}
        
		return createObject(object);
	}
}
