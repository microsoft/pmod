/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:GeneratePtrClass.cpp
****/

#include "GeneratePtrClass.h"

// Format template for Model Inc Header File
// {0} = Namespace
// {1} = interface adapters
// {2} = typedef ptr's
// {3} = forward ptr's
const wchar_t *_modelPtrInc_Template = 
L"\n\
#pragma once \n\
\n\
// depependency Files\n\
#include \"{1}\"\n\
#include <pmod/command_ptr.h>\n\
#include <pmod/observable_collection_ptr.h>\n\
#include <pmod/observable_object_ptr.h>\n\
#include <foundation/async_operation_ptr.h>\n\
\n\
namespace {0} { \n\
// typedef ptrs \n\
{2}\n\
// forward ptrs \n\
{3}\n\
// class ptrs \n\
{4}\n\
// inline class ptrs \n\
{5}\n\
// typedef ptrs \n\
{6}\n\
\n\
}\n\
";

const wchar_t *_collectionPtrTypedef_Template =
L"typedef {0}::_RefCountedObservableCollectionPtr<{1}> {3}_{2}_ptr;\n\
";

const wchar_t *_commandPtrTypedef_Template =
L"typedef {0}::_command_model_ptr<{1}> command_{2}_ptr;\n\
";

const wchar_t *_comPtrTypedef_Template =
L"typedef _{1}_ptr<{0}> {1}_ptr;\n\
";

const wchar_t *_comPtrForward_Template =
L"template <class T>\n\
class _{0}_ptr;\n\
\n";

const wchar_t *_comPtrHResultGetValue_Template = L"\n\
    HRESULT get_{1}({2} *pValue) throw()\n\
    {\n\
        return this->GetValue(&T::Get{0}, pValue);\n\
    }\n\
";

const wchar_t *_comPtrTemplateGetValue_Template = L"\n\
    {2} get_{1}() \n\
    {\n\
        return this->template GetValueWithException<{2}>(&T::Get{0});\n\
    }\n\
";

const wchar_t *_comPtrGetValue_Template = L"\n\
    {2} get_{1}() \n\
    {\n\
        return this->GetValueWithException(&T::Get{0});\n\
    }\n\
";

const wchar_t *_comPtrSetValue_Template = L"\n\
    void set_{1}({2} value) \n\
    {\n\
        return this->SetValueWithException(value, &T::Set{0});\n\
    }\n\
    HRESULT set_{1}_hr({2} value) throw()\n\
    {\n\
        return this->SetValue(value, &T::Set{0});\n\
    }\n\
";

const wchar_t *_comPtrSetValue2_Template = L"\n\
    void set_{1}({2} value) \n\
    {\n\
        return this->SetValueWithException(value, &T::Set{0});\n\
    }\n\
";


const wchar_t *_comPtrGetptremplate =
L"\n    {0} get_{1}();\n\
    {0} get_{1}_hr();\n\
    template <class com_ptr_t>\n\
    HRESULT get_{1}(com_ptr_t& ptr);\n\
";

const wchar_t *_comPtrSetptremplate =
L"    void set_{1}({0} *pT);\n\
    HRESULT set_{1}_hr({0} *pT) throw(); \n\
";

const wchar_t *_comPtrGetPtr_Impl_Template =
L"template <class T>\n\
inline {0} _{1}_ptr<T>::get_{2}()\n\
{\n\
    return this->template GetComPtrWithException<{0}>(&T::Get{3});\n\
}\n\
template <class T>\n\
inline {0} _{1}_ptr<T>::get_{2}_hr()\n\
{\n\
    return this->template GetComPtr<{0}>(&T::Get{3});\n\
}\n\
template <class T>\n\
template <class com_ptr_t>\n\
inline HRESULT _{1}_ptr<T>::get_{2}(com_ptr_t& ptr)\n\
{\n\
    return this->GetValue(&T::Get{3}, ptr.GetAddressOf());\n\
}\n\
";

const wchar_t *_comPtrSetPtr_Impl_Template =
L"template <class T>\n\
inline void _{1}_ptr<T>::set_{2}({0} *pT) \n\
{\n\
    this->SetValueWithException(pT, &T::Set{3});\n\
}\n\
template <class T>\n\
inline HRESULT _{1}_ptr<T>::set_{2}_hr({0} *pT) throw()\n\
{\n\
    this->SetValue(pT, &T::Set{3});\n\
}\n\
";

const wchar_t *_comptremplate = L"\n\
template <class T>\n\
class _{0}_ptr : public {1}<T>\n\
{\n\
public:\n\
    typedef {1}<T> _Base_Type; \n\
    \n\
    _{0}_ptr() throw()\n\
    {\n\
    }\n\
    _{0}_ptr(_In_ const _{0}_ptr<T>& lp) throw() :\n\
        _Base_Type(lp)\n\
    {\n\
    }\n\
    _{0}_ptr(_In_ T* lp) throw() :\n\
        _Base_Type(lp)\n\
    {\n\
    }\n\
\n\
    // property access methods\n\
{2}\n\
}; \n\
";

void CGeneratePtrClass::initialize(foundation::library::DictionaryClassPtr& parameters)
{
    CCodeGenToolBase::initialize(parameters);
}

void CGeneratePtrClass::helpInternal(std::wostream& os)
{
    CCodeGenToolBase::helpInternal(os);
    os << "modelPtrInc: ptr header file" << std::endl;
    os << "modelAdapterInc: Adapter header file" << std::endl;
}

void CGeneratePtrClass::codeGenerateEnd(foundation::library::DictionaryClassPtr& parameters)
{
    std::wstring modelPtrInc = getStringParameter(parameters, L"modelPtrInc", nullptr);
    std::wstring modelAdapterInc = getStringParameter(parameters, L"modelAdapterInc", nullptr);

    std::wstring ptrIncContent = FormatW(_modelPtrInc_Template,
        this->getNamespace().c_str(),// {0}
        getFilePath(modelAdapterInc).c_str(),      // {1}
        _osPtrTypedefInc.str().data(), // {2}
        _osPtrForwardInc.str().data(), // {3}
        _osPtrClassInc.str().data(), // {4}
        _osPtrClassInlineInc.str().data(), // {5}
        _osPtrClassTypedefInc.str().data(), // {6}
        nullptr);

    // generate Ptr Header File
    std::wofstream fileModelPtrInc(ToUtf8String(modelPtrInc.data()), std::ios_base::out);
    fileModelPtrInc << ptrIncContent;
    fileModelPtrInc.close();
}

void CGeneratePtrClass::codeGenerate(const schema::CAsyncOperationTypeInfo *pAsyncOperationModelInfo)
{
    if(pAsyncOperationModelInfo->getResultTypeInfo()->getModelType() == nullptr)
    {
        // the Item Type on the schema is a supported type on the 'Core' library so we don't need to declare it
        return;
    }
}
std::wstring CGeneratePtrClass::getCommandClassPtrName(const schema::CCommandTypeInfo *pCommmandModelInfo)
{
    std::wstring interfaceName = pCommmandModelInfo->getName();
    std::vector<std::wstring> interfaceNameTypes = SplitW(interfaceName, L"_");
    std::vector<std::wstring>::const_iterator iter = interfaceNameTypes.begin();
    ++iter;

    std::wstring command_ptr_name;
    for (; iter != interfaceNameTypes.end(); ++iter)
    {
        if (command_ptr_name.length())
        {
            command_ptr_name += L"_";
        }
        std::wstring next = (*iter);
        if (next[0] == L'I')
        {
            next = next.substr(1);
        }
        command_ptr_name += this->toCodeGenTypeName(next);
    }
    return command_ptr_name;
}

void CGeneratePtrClass::codeGenerate(const schema::CCommandTypeInfo *pCommmandModelInfo)
{
    std::wstring standardCommandPtrClass;
    if (tryGetStandardCommandClassPtr(pCommmandModelInfo, standardCommandPtrClass))
    {
        return;
    }

    std::wstring interfaceName = pCommmandModelInfo->getName();

    _osPtrTypedefInc <<
        FormatW(_commandPtrTypedef_Template,
        getCoreNamespace(),
        pCommmandModelInfo->getName().c_str(),
        getCommandClassPtrName(pCommmandModelInfo).c_str(),
        nullptr);
}

void CGeneratePtrClass::codeGenerate(const schema::CEnumerableTypeInfo *pCollectionModelInfo)
{
    if(pCollectionModelInfo->getItemTypeInfo()->getModelType() == nullptr)
    {
        // the Item Type on the schema is a supported type on the 'Core' library so we don't need to declare it
        return;
    }

    std::wstring itemTypeInfoName = toTypeInfoName(
        pCollectionModelInfo->getItemTypeInfo()->getModelType()->getName());

    _osPtrTypedefInc <<
        FormatW(_collectionPtrTypedef_Template,
        getCoreNamespace(),
        pCollectionModelInfo->getName().c_str(),
        this->toCodeGenTypeName(itemTypeInfoName).c_str(),
        pCollectionModelInfo->getModelType() == schema::ModelTypeInfoType_ObservableList ? L"List" : L"Collection",
        nullptr);


}

void CGeneratePtrClass::codeGenerate(const schema::CObservableObjectInfo *pPropertyModelInfo)
{
    bool isViewModel = pPropertyModelInfo->getModelType() == schema::ModelTypeInfoType_ViewModel;

    std::wstring typeInfoName = toTypeInfoName(pPropertyModelInfo->getName());

    std::wstring typeInfoName_lower = this->toCodeGenTypeName(typeInfoName);
    _osPtrForwardInc <<
        FormatW(_comPtrForward_Template,
        typeInfoName_lower.c_str(),
        nullptr);

    std::wostringstream osPropertiesClass;

    if (isViewModel)
    {
        const schema::CObservableObjectInfo *pSourceType = ((const schema::CPropertyViewModelInfo *)pPropertyModelInfo)->getSourceType();
        if (!pSourceType->getParent().isFactorySchema())
        {
            osPropertiesClass << FormatW(_comPtrGetptremplate,
                toComPtrClass(pSourceType).c_str(),
                L"source",
                nullptr);
            _osPtrClassInlineInc << FormatW(_comPtrGetPtr_Impl_Template,
                toComPtrClass(pSourceType).c_str(),
                typeInfoName_lower.c_str(),
                L"source",
                L"Source",
                nullptr);
        }
    }

    // Iterate Properties
    for (schema::_PropertyIteratorType::const_iterator iter = pPropertyModelInfo->getProperties().begin();
        iter != pPropertyModelInfo->getProperties().end();
        ++iter)
    {
        bool isArray = ((*iter)->getPropertyType() & 0x400) != 0;

        if (isArray)
        {
            continue;
        }
        bool isValueType = ((*iter)->getModelType() == nullptr ||
            (*iter)->getModelType()->getModelType() == schema::ModelTypeInfoType_Enum);

        std::wstring propertyName_lower = this->toCodeGenTypeName((*iter)->getName());

        UINT32 flags = (*iter)->getFlags();
        if ((flags & PropertyFlagType_CanRead))
        {
            if (isValueType)
            {
                osPropertiesClass << FormatW(_comPtrHResultGetValue_Template,
                    (*iter)->getName().c_str(),
                    propertyName_lower.c_str(),
                    toDeclarePropertyTypeInfo((*iter), true).c_str(),
                    nullptr);

                if ((*iter)->getPropertyType() == foundation::PropertyType_String)
                {
                    osPropertiesClass << FormatW(_comPtrGetValue_Template,
                        (*iter)->getName().c_str(),
                        propertyName_lower.c_str(),
                        FormatW(L"{0}::HStringPtr", getFoundationNamespace(), nullptr).c_str(),
                        nullptr);
                }
                else
                {
                    osPropertiesClass << FormatW(_comPtrTemplateGetValue_Template,
                        (*iter)->getName().c_str(),
                        propertyName_lower.c_str(),
                        toDeclarePropertyTypeInfo((*iter), true).c_str(),
                        nullptr);
                }
            }
            else
            {
                osPropertiesClass << FormatW(_comPtrGetptremplate,
                    toComPtrClass((*iter)->getModelType()).c_str(),
                    propertyName_lower.c_str(),
                    nullptr);
                _osPtrClassInlineInc << FormatW(_comPtrGetPtr_Impl_Template,
                    toComPtrClass((*iter)->getModelType()).c_str(),
                    typeInfoName_lower.c_str(),
                    propertyName_lower.c_str(),
                    (*iter)->getName().c_str(),
                    nullptr);
                
            }
        }
        if ((flags & PropertyFlagType_CanWrite))
        {
            if (isValueType)
            {
                osPropertiesClass << FormatW(_comPtrSetValue_Template,
                    (*iter)->getName().c_str(),
                    propertyName_lower.c_str(),
                    toDeclarePropertyTypeInfo((*iter), true).c_str(),
                    nullptr);

                if ((*iter)->getPropertyType() == foundation::PropertyType_String)
                {
                    osPropertiesClass << FormatW(_comPtrSetValue2_Template,
                        (*iter)->getName().c_str(),
                        propertyName_lower.c_str(),
                        L"LPCSTR_t",
                        nullptr);
                }
            }
            else
            {
                std::wstring typeInfoName = toModelTypeInfo((*iter)->getModelType());

                osPropertiesClass << FormatW(_comPtrSetptremplate,
                    typeInfoName.c_str(),
                    propertyName_lower.c_str(),
                    nullptr);
                _osPtrClassInlineInc << FormatW(_comPtrSetPtr_Impl_Template,
                    typeInfoName.c_str(),
                    typeInfoName_lower.c_str(),
                    propertyName_lower.c_str(),
                    (*iter)->getName().c_str(),
                    nullptr);
            }
        }
    }
    std::wostringstream os_base_class_ptr;
    if (pPropertyModelInfo->getBaseType())
    {
        os_base_class_ptr << this->getNativeNamespaceLookup(pPropertyModelInfo->getBaseType()->getParent().getNamespace());
        os_base_class_ptr << "::_" << this->toCodeGenTypeName(toTypeInfoName(pPropertyModelInfo->getBaseType()->getName())) << "_ptr";
    }
    else
    {
        os_base_class_ptr << getCoreNamespace() << "::ObservableObjectPtr";
    }

    _osPtrClassInc << FormatW(_comptremplate,
        typeInfoName_lower.c_str(),
        os_base_class_ptr.str().c_str(),
        osPropertiesClass.str().c_str(),
        nullptr);

    _osPtrClassTypedefInc << FormatW(_comPtrTypedef_Template,
        pPropertyModelInfo->getName().c_str(),
        typeInfoName_lower.c_str(),
        nullptr);
}

void CGeneratePtrClass::emitComPtrClass(std::wostream& os, const schema::CTypeInfo *pModelTypeInfo)
{
    assert(pModelTypeInfo);
    schema::ModelTypeInfoType modelTypeInfoType = pModelTypeInfo->getModelType();
    if (
        modelTypeInfoType == schema::ModelTypeInfoType_ObjectDispatch ||
        modelTypeInfoType == schema::ModelTypeInfoType_ObservableObject ||
        modelTypeInfoType == schema::ModelTypeInfoType_ViewModel)
    {
        os << this->getNativeNamespaceLookup(pModelTypeInfo->getParent().getNamespace());
        os << "::_" << this->toCodeGenTypeName(toTypeInfoName(pModelTypeInfo->getName())) << "Ptr";
        os << "<";
        os << this->getNativeNamespaceLookup(pModelTypeInfo->getParent().getNamespace());
        os << "::" << pModelTypeInfo->getName();
        os << ">";

    }
    else if (modelTypeInfoType == schema::ModelTypeInfoType_ObservableCollection ||
             modelTypeInfoType == schema::ModelTypeInfoType_ObservableList)
    {
        const schema::CEnumerableTypeInfo *pCollectionModelInfo = (const schema::CEnumerableTypeInfo *)pModelTypeInfo;
        const schema::CTypeInfo *pItemModelTypeInfo = pCollectionModelInfo->getItemTypeInfo()->getModelType();
        if (pItemModelTypeInfo == nullptr)
        {
            os << this->getCoreNamespace() << "::collection_";
            os << ToLowerCase(toPropertyTypeName(pCollectionModelInfo->getItemTypeInfo()->getPropertyType()).c_str());
            os << "_ptr";
        }
        else
        {
            if (modelTypeInfoType == schema::ModelTypeInfoType_ObservableList)
            {
                // TODO: temporary workaround until we support better list based collections
                os << "pmod::list_inspectable_ptr";
                return;
            }

            os << this->getNativeNamespaceLookup(pItemModelTypeInfo->getParent().getNamespace());
            os << "::";

            if (modelTypeInfoType == schema::ModelTypeInfoType_ObservableCollection)
            {
                os << "collection_";
            }
            else
            {
                os << "list_";
            }

            os << this->toCodeGenTypeName(toTypeInfoName(pItemModelTypeInfo->getName()));
            os << "_ptr";
        }
    }
    else if (modelTypeInfoType == schema::ModelTypeInfoType_Command)
    {
        const schema::CCommandTypeInfo *pCommmandModelInfo = (const schema::CCommandTypeInfo *)pModelTypeInfo;

        std::wstring standardCommandPtrClass;
        if (tryGetStandardCommandClassPtr(pCommmandModelInfo, standardCommandPtrClass))
        {
            os << standardCommandPtrClass;
            return;
        }

        os << this->getNativeNamespaceLookup(pModelTypeInfo->getParent().getNamespace());
        os << "::command_" << getCommandClassPtrName(pCommmandModelInfo);
        os << "_ptr";
    }
}

std::wstring CGeneratePtrClass::toComPtrClass(const schema::CTypeInfo *pModelTypeInfo)
{
    std::wostringstream os;
    emitComPtrClass(os,pModelTypeInfo);
    return os.str();

}

bool CGeneratePtrClass::tryGetStandardCommandClassPtr(
    const schema::CCommandTypeInfo *pCommmandModelInfo,
    std::wstring& commandClassPtrName)
{
    assert(pCommmandModelInfo != nullptr);
    assert(commandClassPtrName.empty());

    const schema::CPropertyTypeInfo* paramTypeInfo = pCommmandModelInfo->getParameterTypeInfo();
    const schema::CPropertyTypeInfo* resultTypeInfo = pCommmandModelInfo->getResultTypeInfo();

    const foundation::PropertyType paramType = paramTypeInfo->getPropertyType();
    const foundation::PropertyType resultType = resultTypeInfo->getPropertyType();

    const bool isAsync = pCommmandModelInfo->getIsAsync();
    if (isSimpleCommand(pCommmandModelInfo))
    {
        commandClassPtrName = FormatW(L"{0}::simple_{1}CommandPtr", getCoreNamespace(), (isAsync ? L"async_" : L""), nullptr);
        return true;
    }

    if (isObjectObjectCommand(pCommmandModelInfo))
    {
        commandClassPtrName = FormatW(L"{0}::{1}CommandPtr", getCoreNamespace(), (isAsync ? L"async_" : L""), nullptr);
        return true;
    }

    return false;
}
