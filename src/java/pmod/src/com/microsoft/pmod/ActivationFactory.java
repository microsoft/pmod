/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:ActivationFactory.java
****/
package com.microsoft.pmod;


class ActivationFactory {
	static native long getFactory(String activationId);
}
