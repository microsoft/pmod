/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:GenerateImplCppTemplate.h
****/


#include "GenerateCppLibrary.h"
#include <sstream>

class CGenerateImplCppTemplate : public CGenerateCppLibrary
{
protected:
    void initialize(foundation::library::DictionaryClassPtr& parameters) override;
    void helpInternal(std::wostream& os) override;
    void codeGenerateEnd(foundation::library::DictionaryClassPtr& parameters) override;
    void codeGenerate(const schema::CObjectDispatchInfo *pObjectDispatchInfo) override;
    void codeGenerate(const schema::CObservableObjectInfo *pPropertyModelInfo) override;

    std::wstring getMethodCallback(const schema::CMethodInfo*pMethodInfo, std::wstring& invokeInstance) override;
private:
    void codeGenerateImpl(
        const std::wstring& baseClassType,
        const schema::CObjectDispatchInfo *pObjectDispatchInfo);

private:
	std::wostringstream _osHeaderImplTemplates;
};
