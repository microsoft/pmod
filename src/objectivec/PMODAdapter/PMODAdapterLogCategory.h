/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:PMODAdapterLogCategory.h
****/

#include <foundation/interfaces/logger.h>
#include <foundation/com_ptr.h>

class CPMODAdapterLogger
{
private:
    CPMODAdapterLogger();
    static CPMODAdapterLogger& GetInstance();
public:
    static foundation::ILogger *GetPMODAdapterLogger()
    {
        return GetInstance()._spNSAdapterLogger;
    }
private:
    foundation::ComPtr<foundation::ILogger> _spNSAdapterLogger;
};

class CPMODAdapterLogCategory
{
private:
    CPMODAdapterLogCategory();
    static CPMODAdapterLogCategory& GetInstance();
public:
    static foundation::ILogCategory *GetPMODAdapterFactoryCategory()
    {
        return GetInstance()._spAdapterFactoryLogCategory;
    }

    static foundation::ILogCategory *GetPMODFoundationCategory()
    {
        return GetInstance()._spFoundationLogCategory;
    }
    static foundation::ILogCategory *GetPMODObjectBaseCategory()
    {
        return GetInstance()._spObjectBaseLogCategory;
    }
    static foundation::ILogCategory *GetPMODObjectCategory()
    {
        return GetInstance()._spObservableObjectLogCategory;
    }
    static foundation::ILogCategory *GetPMODCollectionCategory()
    {
        return GetInstance()._spObservableCollectionLogCategory;
    }
private:
    foundation::ComPtr<foundation::ILogCategory> _spAdapterFactoryLogCategory;
    foundation::ComPtr<foundation::ILogCategory> _spFoundationLogCategory;
    foundation::ComPtr<foundation::ILogCategory> _spObjectBaseLogCategory;
    foundation::ComPtr<foundation::ILogCategory> _spObservableObjectLogCategory;
    foundation::ComPtr<foundation::ILogCategory> _spObservableCollectionLogCategory;
};
