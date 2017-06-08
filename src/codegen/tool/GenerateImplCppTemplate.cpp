/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:GenerateImplCppTemplate.cpp
****/
#include "GenerateImplCppTemplate.h"

static const wchar_t* _implCppTemplateInc_Template =
L"\
#pragma once\n\
\n\
#include <foundation/pv_util.h>\n\
#include <pmod/library/observable_object_impl.h>\n\
\n\
{0} \n\
{1}\n\
{2}\n\
";

// Format template for the FastModel containers
// {0} = Type Name eg: simple_model
// {1} = Base class
// {2} = PropertyGet case statements
// {3} = PropertySet case statements
static const wchar_t* _property_data_Template =
L"template <class T> \n\
class _{0}ObjectDispatchHelper : \n\
    public {1} \n\
{\n\
protected:\n\
    typedef {1} _BaseType; \n\
    \n\
    HRESULT GetPropertyInternal(UINT32 propertyId,foundation::IInspectable **ppValue) \n\
    { \n\
{2}\n\
        return _BaseType::GetPropertyInternal(propertyId, ppValue); \n\
    } \n\
    \n\
    HRESULT SetPropertyInternal(UINT32 propertyId, foundation::IInspectable *pValue) \n\
    { \n\
{3}\n\
        return _BaseType::SetPropertyInternal(propertyId, pValue); \n\
    }\n\
    \n\
};\n\n\
";

static const wchar_t* _methodHandlerTemplate =
L"template <class T> \n\
class _{0}MethodHandler : \n\
    public {1} \n\
{\n\
protected:\n\
    typedef {1} _BaseType; \n\
    \n\
    HRESULT InvokeMethodInternal(UINT32 methodId, _In_ UINT32 size, _In_ foundation::IInspectable **parameters, foundation::IInspectable **ppResult) \n\
    { \n\
        HRESULT hr;\n\
        switch(methodId)\n\
        {\n\
{2}\n\
        default:\n\
            return _BaseType::InvokeMethodInternal(methodId,size,parameters,ppResult); \n\
        }\n\
        return S_OK; \n\
    }\n\
};\n\n\
";

static const wchar_t* _implGetSetValueSwitchTemplate =
L"\t\tswitch (propertyId)\n\
\t\t{ \n\
{0}\
\t\t}\n";

static const wchar_t* _methodHandlerObjectImplTemplate =
L"\
";

static const wchar_t* _implObservableObjectImplTemplate =
L"\
    template <class TInterface = {1},class TBASE = {2}>\n\
    class _{0}ImplBase :\n\
        public TBASE,\n\
        public _{0}ObjectDispatchHelper<_{0}ImplBase<TInterface,TBASE>>,\n\
        public {4}<_{0}ImplBase<TInterface,TBASE>>\n\
    {\n\
    public:\n\
{3}\n\
{5}\n\
        // Interface IObservableObject\n\
        STDMETHOD(GetProperty)(UINT32 propertyId, foundation::IInspectable** value)\n\
        {\n\
            return this->GetPropertyInternal(propertyId, value); \n\
        }\n\
\n\
        STDMETHOD(SetProperty)(UINT32 propertyId, foundation::IInspectable *value)\n\
        {\n\
            return this->SetPropertyInternal(propertyId, value); \n\
        }\n\
\n\
        STDMETHOD(InvokeMethod)(UINT32 methodId, _In_ UINT32 size, _In_ foundation::IInspectable **parameters, foundation::IInspectable **ppResult)\n\
        {\n\
            return this->InvokeMethodInternal(methodId,size,parameters,ppResult); \n\
        }\n\
    };\n\
";

static const wchar_t* _implGetPropertyTemplate =
L"\t\tHRESULT Get{0}({1} pValue) override\n\
\t\t{ \n\
\t\t\tfoundation_assert(false);\n\
\t\t\treturn E_NOTIMPL;\n\
\t\t}\n";

static const wchar_t* _implSetPropertyTemplate =
L"\t\tHRESULT Set{0}({1} value) override\n\
\t\t{ \n\
\t\t\tfoundation_assert(false);\n\
\t\t\treturn E_NOTIMPL;\n\
\t\t}\n";

void CGenerateImplCppTemplate::initialize(foundation::library::DictionaryClassPtr& parameters)
{
    CCodeGenToolBase::initialize(parameters);
}

void CGenerateImplCppTemplate::helpInternal(std::wostream& os)
{
    CCodeGenToolBase::helpInternal(os);
}

void CGenerateImplCppTemplate::codeGenerateEnd(foundation::library::DictionaryClassPtr& parameters) 
{
    std::wstring implCppTemplateIncPath = getStringParameter(parameters, L"implCppTemplateInc", nullptr);

    auto namespaceDeclare = toNativeNamespaceDeclare(this->getNativeNamespace());
    auto namespaceEnd = toNativeNamespaceEnd(this->getNativeNamespace());

    std::wofstream fileImplCppTemplateInc(ToUtf8String(implCppTemplateIncPath.data()), std::ios_base::out);
    fileImplCppTemplateInc << FormatW(_implCppTemplateInc_Template,
        namespaceDeclare.data(),
        _osHeaderImplTemplates.str().data(),
        namespaceEnd.data(),
        nullptr);

    fileImplCppTemplateInc.close();
}

void CGenerateImplCppTemplate::codeGenerateImpl(
    const std::wstring& baseClassType,
    const schema::CObjectDispatchInfo *pObjectDispatchInfo)
{
    std::wstring typeInfoName = toTypeInfoName(pObjectDispatchInfo->getName());
    std::wstring typeInfoNameGenerated = typeInfoName; // ToPrivateLowerCaseConvention(typeInfoName.c_str());

                                                       // default base container class
    std::wstring typeInfoBaseNameImpl = L"foundation::library::_ObjectDispatchHelperBase<T>";

    // Emit get/set value cases
    std::wostringstream getValueCases;
    std::wostringstream setValueCases;

    std::wostringstream osNotImplProperties;

    // Iterate Properties
    for (schema::_PropertyIteratorType::const_iterator iter = pObjectDispatchInfo->getProperties().begin();
        iter != pObjectDispatchInfo->getProperties().end();
        ++iter)
    {
        UINT32 flags = (*iter)->getFlags();
        bool canRead = (flags & PropertyFlagType_CanRead) != 0;
        bool canWrite = (flags & PropertyFlagType_CanWrite) != 0;

        if (canRead)
        {
            std::wostringstream os;
            emitDeclarePropertyTypeInfo(os, *iter, true);

            osNotImplProperties <<
                FormatW(_implGetPropertyTemplate,
                (*iter)->getName().data(),
                    os.str().data(),
                    nullptr);
        }
        if (canWrite)
        {
            std::wostringstream os;
            emitDeclarePropertyTypeInfo(os, *iter, false);

            osNotImplProperties <<
                FormatW(_implSetPropertyTemplate,
                (*iter)->getName().data(),
                    os.str().data(),
                    nullptr);
        }

        emitGetSetValuePropertyCases(
            canRead ? &getValueCases : nullptr,
            canWrite ? &setValueCases : nullptr,
            (*iter));
    }

    std::wostringstream osMethodHandler;
    std::wostringstream osAbstractMethods;

    // Iterate Methods
    for (schema::_MethodIteratorType::const_iterator iter = pObjectDispatchInfo->getMethods().begin();
        iter != pObjectDispatchInfo->getMethods().end();
        ++iter)
    {
        // emit method handlers
        emitMethodHandler(osMethodHandler, osAbstractMethods, *iter, true);
    }


    if (pObjectDispatchInfo->getMethods().size())
    {
        // default base container class
        std::wstring methodHandlerBase = L"foundation::library::_MethodHandlerBase<T>";

        _osHeaderImplTemplates << FormatW(
            _methodHandlerTemplate,
            typeInfoNameGenerated.data(),
            methodHandlerBase.data(),
            osMethodHandler.str().data(),
            nullptr);
    }


    _osHeaderImplTemplates << FormatW(_property_data_Template,
        typeInfoNameGenerated.data(),
        typeInfoBaseNameImpl.data(),
        getValueCases.str().length() != 0 ? FormatW(_implGetSetValueSwitchTemplate, getValueCases.str().data(), nullptr).data() : L"",
        setValueCases.str().length() != 0 ? FormatW(_implGetSetValueSwitchTemplate, setValueCases.str().data(), nullptr).data() : L"",
        nullptr
    );

    std::wstring methodHandlerBaseClass = L"foundation::library::_MethodHandlerBase";
    if (pObjectDispatchInfo->getMethods().size())
    {
        methodHandlerBaseClass = FormatW(L"_{0}MethodHandler", typeInfoNameGenerated.data(), nullptr);
    }

    //std::wstring baseClass = L"pmod::library::_ObservableObjectImplBase<TInterface>";
    _osHeaderImplTemplates << FormatW(
        _implObservableObjectImplTemplate,
        typeInfoNameGenerated.data(),
        pObjectDispatchInfo->getName().data(),
        baseClassType.data(),
        osNotImplProperties.str().data(),
        methodHandlerBaseClass.data(),
        osAbstractMethods.str().data(),
        nullptr
    );
}

void CGenerateImplCppTemplate::codeGenerate(const schema::CObjectDispatchInfo *pObjectDispatchInfo)
{
    codeGenerateImpl(
        L"foundation::library::_ObjectDispatchImplBase<TInterface>",
        pObjectDispatchInfo);
}

void CGenerateImplCppTemplate::codeGenerate(const schema::CObservableObjectInfo *pPropertyModelInfo)
{
    codeGenerateImpl(
        L"pmod::library::_ObservableObjectImplBase<TInterface>",
        pPropertyModelInfo);
}

std::wstring CGenerateImplCppTemplate::getMethodCallback(
    const schema::CMethodInfo*pMethodInfo,
    std::wstring& invokeInstance)
{
    invokeInstance = L"this->GetOuter()";
    return pMethodInfo->getName();
}
