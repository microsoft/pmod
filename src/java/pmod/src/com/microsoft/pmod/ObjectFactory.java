/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ObjectFactory.java
****/
package com.microsoft.pmod;

public final class ObjectFactory extends InspectableObject {

	// Private Native methods
	private static native long getObjectFactoryInstanceInterface();
	private native FoundationObjectImpl tryCreateObjectInternal(String type, Object context);
	private static native FoundationObjectImpl activateObjectInternal(String type, Object context);
	
	private static ObjectFactory modelFactory;
	public static ObjectFactory getObjectFactoryInstance() {
		if (modelFactory == null) {
			modelFactory = new ObjectFactory(getObjectFactoryInstanceInterface());
		}
        
		return modelFactory;
	}

	public ObjectFactory(long  interfaceValue) {
		super(interfaceValue);
	}
	
	public FoundationObject tryCreateObject(final String type, Object context) {
		return tryCreateObject(type, context, null);
	}

	public FoundationObject tryCreateObject(final String type, Object context, final ClassFactory classFactory) {
		return (FoundationObject)FactoryObject.executeFoundationValue(context, new ExecuteFoundationValues() {
			public Object execute(Object[] values) {
				FoundationObjectImpl object = tryCreateObjectInternal(type, values[0]);
				if (classFactory != null) {
					object = classFactory.createObject(object);
				}
                
				return object;
			}
		});
	}
	
	@SuppressWarnings("unchecked")
	public static <T extends FoundationObject> T activateObject(final String type, Object context, final ClassFactory classFactory) {
		return (T)FactoryObject.executeFoundationValue(context, new ExecuteFoundationValues() {
			public Object execute(Object[] values) {
				FoundationObjectImpl object = activateObjectInternal(type, values[0]);
				if (classFactory != null) {
					object = classFactory.createObject(object);
				}                
				return object;
			}
		});
	}

}
