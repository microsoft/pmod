/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:GenerateCppBase.cpp
****/

#include "GenerateCppBase.h"

void CGenerateCppBase::emitModelTypeVarDeclare(
    std::wostream& os,
    const schema::CPropertyTypeInfo*pPropertyTypeInfo,
    const wchar_t *pVarName)
{
    const schema::CTypeInfo *pModelTypeInfo = pPropertyTypeInfo->getModelType();
    schema::ModelTypeInfoType modelType = pModelTypeInfo ?
        pModelTypeInfo->getModelType() : schema::ModelTypeInfoType_None;

    if ((modelType == schema::ModelTypeInfoType_ObjectDispatch ||
        modelType == schema::ModelTypeInfoType_ObservableObject ||
        modelType == schema::ModelTypeInfoType_ViewModel ||
        modelType == schema::ModelTypeInfoType_ObservableCollection))
    {
        schema::ModelTypeInfoType modelItemType;
        const schema::CTypeInfo *pModelItemTypeInfo;
        const schema::CPropertyTypeInfo *pItemTypeInfo;
        if (modelType == schema::ModelTypeInfoType_ObservableCollection)
        {
            const schema::CEnumerableTypeInfo* collectionInfo = (schema::CEnumerableTypeInfo*) pModelTypeInfo;
            pItemTypeInfo = collectionInfo->getItemTypeInfo();
            pModelItemTypeInfo = pItemTypeInfo->getModelType();

            modelItemType = pModelItemTypeInfo ?
                pModelTypeInfo->getModelType() : schema::ModelTypeInfoType_None;

        }

        if (modelType == schema::ModelTypeInfoType_ObservableCollection && !modelItemType)
        {
            os << FormatW(L"foundation::ComPtr<{0}::{1}> {2};",
                getCoreNamespace(),
                FormatW(L"IObservableCollection_{0}", toPropertyTypeName(pItemTypeInfo->getPropertyType()).data(), nullptr).data(),
                pVarName,
                nullptr) << std::endl;
        }
        else if (pPropertyTypeInfo->getPropertyType() & 0x400)
        {
            emitArrayWrapperType(os, pPropertyTypeInfo, pVarName);
        }
        else
        {
            // TODO fix this custom hack
            std::wstring typeInfoNamespace = this->getNativeNamespaceLookup(pModelTypeInfo->getParent().getNamespace());

            os << FormatW(L"foundation::ComPtr<{0}::{1}> {2};",
                typeInfoNamespace.c_str(),
                pModelTypeInfo->getName().c_str(),
                pVarName,
                nullptr) << std::endl;

        }
    }
    else if(modelType == schema::ModelTypeInfoType_Command)
    {
        os << "foundation::ComPtr<";
        emitModelTypeInfo(os, pPropertyTypeInfo->getModelType());
        os << "> " << pVarName << ";" << std::endl;
    }
    else
    {
        emitTypeVarDeclare(os, pPropertyTypeInfo, pVarName);
    }
}

void CGenerateCppBase::emitTypeVarDeclare(
    std::wostream& os,
    const schema::CPropertyTypeInfo*pPropertyTypeInfo,
    const wchar_t *pVarName)
{
    if (pPropertyTypeInfo->getModelType() && (pPropertyTypeInfo->getPropertyType() & 0x400) == 0)
    {
        if (pPropertyTypeInfo->getModelType()->getModelType() == schema::ModelTypeInfoType_Enum)
        {
            // Enum Type Info generation
            this->emitFullName(os, pPropertyTypeInfo->getModelType());
        }
        else
        {
            os << "foundation::ComPtr<";
            emitCoreModelTypeInfo(os, pPropertyTypeInfo->getModelType()->getModelType());
            os << ">";
        }
    }
    else
    {
        if (pPropertyTypeInfo->getPropertyType() & 0x400)
        {
            emitDeclareArrayWrapperType(os, pPropertyTypeInfo);
        }
        else if (pPropertyTypeInfo->getPropertyType() == foundation::PropertyType_Inspectable)
        {
            os << "foundation::InspectablePtr";
        }
        else if (pPropertyTypeInfo->getPropertyType() == foundation::PropertyType_String)
        {
            os << "foundation::HStringPtr";
        }
        else
        {
            os << toCoreType(pPropertyTypeInfo->getPropertyType());
        }
    }
    os << " " << pVarName << ";" << std::endl;
}

void CGenerateCppBase::emitDeclareArrayWrapperType(std::wostream& os,const schema::CPropertyTypeInfo* pPropertyTypeInfo)
{
    assert(pPropertyTypeInfo->getPropertyType() & 0x400);

    bool isCustomWrapper = false;
    os << "foundation::";

    if ((pPropertyTypeInfo->getPropertyType() & 0xff) == foundation::PropertyType_String)
    {
        os << "HString";
    }
    else if (isPropertyTypeInfoEnum(pPropertyTypeInfo))
    {
        os << "ArrayWrapper<";
        emitFullName(os,pPropertyTypeInfo->getModelType());
        os << ">";
        isCustomWrapper = true;
    }
    else if (pPropertyTypeInfo->getModelType() && pPropertyTypeInfo->getModelType()->getModelType() != schema::ModelTypeInfoType_None &&
        pPropertyTypeInfo->getModelType()->getModelType() != schema::ModelTypeInfoType_Enum)
    {
        std::wstring typeInfoNamespace = this->getNativeNamespaceLookup(pPropertyTypeInfo->getModelType()->getParent().getNamespace());

        os << "ArrayRefCountWrapper<" << typeInfoNamespace << "::" << pPropertyTypeInfo->getModelType()->getName().c_str() << ">";
        isCustomWrapper = true;
    }
    else if ((pPropertyTypeInfo->getPropertyType() & 0xff) == foundation::PropertyType_Inspectable)
    {
        os << "Inspectable";
    }
    else if ((pPropertyTypeInfo->getPropertyType() & 0xff) == foundation::PropertyType_Boolean)
    {
        os << "Bool";
    }
    else
    {
        os << toPropertyTypeName((foundation::PropertyType)(pPropertyTypeInfo->getPropertyType() & 0xff)).c_str();
    }
    if (!isCustomWrapper)
        os << "ArrayWrapper";
}

void CGenerateCppBase::emitArrayWrapperType(std::wostream& os,
    const schema::CPropertyTypeInfo* pPropertyTypeInfo,
    const wchar_t *pVarName,
    bool useJustType)
{
    emitDeclareArrayWrapperType(os, pPropertyTypeInfo);
    if (useJustType)
    {
        os << pVarName;
    }
    else
    {
        os << " " << pVarName << ";" << std::endl;
    }
}

void CGenerateCppBase::emitOutputPtrVar(
    std::wostream& os,
    const schema::CPropertyTypeInfo* pPropertyTypeInfo,
    const wchar_t *pVarName)
{
    if (isPropertyTypeInfoArray(pPropertyTypeInfo))
    {
        os << pVarName << ".GetSizeAddressOf()," << pVarName << ".GetBufferAddressOf()";
    }
    else if (
             pPropertyTypeInfo->getPropertyType() == foundation::PropertyType_String ||
             isPropertyTypeInfoObject(pPropertyTypeInfo) ||
             (hasPropertyTypeInfoModel(pPropertyTypeInfo) && !isPropertyTypeInfoEnum(pPropertyTypeInfo)))
    {
        os << pVarName << ".GetAddressOf()";
    }
    else
    {
        os << "&" << pVarName;
    }
}

void CGenerateCppBase::emitInputPtrVar(
                                        std::wostream& os,
                                        const schema::CPropertyTypeInfo* pPropertyTypeInfo,
                                        const wchar_t *pVarName)
{
    if (isPropertyTypeInfoArray(pPropertyTypeInfo))
    {
        os << pVarName << ".GetSize(),(";
        emitDeclareArrayWrapperType(os, pPropertyTypeInfo);
        os << "::_Item_Type *)" << pVarName << ".GetBuffer()";
    }
    else
    {
        os << pVarName;
    }
}

std::wstring CGenerateCppBase::toPropertyTypeInfoAsync(const schema::CPropertyTypeInfo *pResultPropertyTypeInfo, bool isAsync)
{
    if (!isAsync)
    {
        return CCodeGenToolBase::toPropertyTypeInfo(pResultPropertyTypeInfo);
    }
    else
    {
        std::wstring interfaceName;
        if (pResultPropertyTypeInfo->getModelType() == nullptr)
        {
            interfaceName = getFoundationNamespace();
            if (pResultPropertyTypeInfo->getPropertyType() == foundation::PropertyType_Empty)
            {
                interfaceName += L"::IAsyncOperationBase";
            }
            else
            {
                interfaceName += L"::IAsyncOperation_";
                interfaceName += toPropertyTypeName(pResultPropertyTypeInfo->getPropertyType());
            }
        }
        else if (pResultPropertyTypeInfo->getModelType()->getModelType() == schema::ModelTypeInfoType_Object)
        {
            interfaceName = getFoundationNamespace();
            interfaceName += L"::IAsyncOperation_IObject";
        }
        else if (pResultPropertyTypeInfo->getModelType()->getModelType() == schema::ModelTypeInfoType_Dictionary)
        {
            interfaceName = getFoundationNamespace();
            interfaceName += L"::IAsyncOperation_IDictionary";
        }
        else
        {
            interfaceName = getNamespace();
            interfaceName += L"::IAsyncOperation_";
            interfaceName += pResultPropertyTypeInfo->getModelType()->getName();
        }
        return interfaceName;
    }
}

std::wstring CGenerateCppBase::toCodeGenTypeName(const std::wstring& typeName)
{
    if (this->_useLowerCaseConvention)
    {
        return ToLowerCaseConvention(typeName.c_str());
    }
    return typeName;
}

std::wstring CGenerateCppBase::toCodeGenVarName(const std::wstring& typeName)
{
    if (this->_useLowerCaseConvention)
    {
        return ToLowerCaseConvention(typeName.c_str());
    }
    std::wstring varTypeName = typeName;
    varTypeName[0] = ::tolower(varTypeName[0]);
    return varTypeName;
}
