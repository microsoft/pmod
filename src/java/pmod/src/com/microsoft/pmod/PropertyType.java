/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PropertyType.java
****/
package com.microsoft.pmod;

public enum PropertyType {
	Empty,
	UInt8,
	Int16,
	UInt16,
	Int32,
	UInt32,
	Int64,
	UInt64,
	Single,
	Double,
	Char16,
	Boolean,
	String,
	Inspectable,
	DateTime,
	TimeSpan,
	Guid,
	Point,
	Size,
	Rect,
	OtherType,
	UInt8Array,
	Int16Array,
	UInt16Array,
	Int32Array,
	UInt32Array,
	Int64Array,
	UInt64Array,
	SingleArray,
	DoubleArray,
	Char16Array,
	BooleanArray,
	StringArray,
	InspectableArray,
	DateTimeArray,
	TimeSpanArray,
	GuidArray,
	PointArray,
	SizeArray,
	RectArray,
	OtherTypeArray;
	
	static PropertyType toPropertyType(int type)
	{
		if (values == null) {
			values = PropertyType.values();
		}
        
		// for Array types we need to adjust the offset in our values
		if ((type & 0x400) != 0)
		{
			type -= 1004;
		}
        
		return values[type];
	}
    
	static PropertyType[] values = null;
}
