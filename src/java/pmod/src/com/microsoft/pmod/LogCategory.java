/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:LogCategory.java
****/
package com.microsoft.pmod;


/**
 * Class:LogCatgerory
 * @author rodrigov
 *
 */
public class LogCategory extends InspectableObject {
	
	LogCategory(long interfaceValue) {
		super(interfaceValue);
	}
	private native int GetLevel();
	private native void SetLevel(int level);

	
	public LoggingLevel getLevel() {
		LoggingLevel level = LoggingLevel.values()[this.GetLevel()];
		return level;
	}
	
	public void setLevel(LoggingLevel level) {
		SetLevel(toInt(level));
	}

	private static int toInt(LoggingLevel level)
	{
		switch(level)
		{
		case Off:
			return 0;
		case Fatal:
			return 1;
		case Error:
			return 2;
		case Warning:
			return 3;
		case Info:
			return 4;
		case Debug:
			return 5;
		case Trace:
			return 6;
		}
		return -1;
	}
}
