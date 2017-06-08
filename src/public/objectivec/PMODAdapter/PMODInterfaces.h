/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODInterfaces.h
****/



#if __cplusplus

namespace pmod {
	struct IObservableObject;
}

namespace foundation {
    struct IInspectable;
    struct IPropertyValue;
    struct IObject;
    struct ILogger;
    struct IEnumValue;
    struct IObjectDispatch;
}

#define IFoundationInspectable  foundation::IInspectable
#define IFoundationObject       foundation::IObject
#define IFoundationObjectDispatch       foundation::IObjectDispatch
#define IFoundationLogger       foundation::ILogger
#define IFoundationEnumValue    foundation::IEnumValue
#define IPmodObservableObject   pmod::IObservableObject

#else

#define IFoundationInspectable         void
#define IFoundationObject              void
#define IFoundationObjectDispatch      void
#define IFoundationLogger       void
#define IFoundationEnumValue    void
#define IPmodObservableObject   void


#endif

