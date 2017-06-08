/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:GenerateCSharpClasses.cpp
****/

#include "GenerateCSharpClasses.h"
#include "GenerateWindowsRuntimeAdapter.h"

// from Public
//#include <pmod/interfaces.h>

//using namespace pmod;

        /// <summary>
        /// {0}: Class Name
        /// {1}: Base Class
        /// {2}: IID Type
        /// {3}: Initialize Properties
        /// {4}: Internal Command Properties Implement
        /// {5}: Internal Collection Properties Implement
        /// {6}: OnInvoke Override
        /// {7}: Method callback initialize
        /// {8}: 'new' prefix
/// </summary>
const wchar_t *_implementModelClass_Template =
L"\
    public partial class {0} : {1}\n\
    {\n\
        public {8} static Guid IID { get { return TypeInfoClass.IID_{0}; } }\n\
\n\
        public {0}(Guid iid,ObservableObjectOptions modelOptions) :\n\
            base(iid,modelOptions)\n\
        {\n\
            InitializePropertiesInternal();\n\
            InitializeModel();\n\
{7}\n\
        }\n\
\n\
        public {0}(Guid iid) :\n\
            this(iid,ObservableObjectOptions.None)\n\
        {\n\
        }\n\
\n\
        public {0}(ObservableObjectOptions modelOptions) :\n\
            this(IID,modelOptions)\n\
        {\n\
        }\n\
\n\
        public {0}() :\n\
            this(ObservableObjectOptions.InitializeProperties)\n\
        {\n\
        }\n\
\n\
        private void InitializePropertiesInternal()\n\
        {\n\
{3}\n\
{4}\n\
        }\n\
\n\
        #region Internal Properties\n\
{5}\n\
        #endregion\n\
\n\
{6}\n\
    }\n\
";

        /// <summary>
        /// {0}: Class Name
        /// {1}: Base Class
        /// {2}: IID Type
        /// {3}: Initialize Properties
        /// {4}: Internal Command Properties Implement
        /// {5}: Internal Collection Properties Implement
        /// {6}: OnInvoke Override
        /// {7}: Method callback initialize
        /// {8}: 'new' prefix
        /// </summary>
const wchar_t *_implementViewModelClass_Template =
 L"\
    public partial class {0} : {1}\n\
    {\n\
        public {8} static Guid IID { get { return TypeInfoClass.IID_{0}; } }\n\
        \n\
        public {0}(Guid iidType, IObservableObject pSource, ObservableObjectOptions modelOptions,ViewModelOptions viewModelOptions) :\n\
            base(iidType,pSource,modelOptions,viewModelOptions)\n\
        {\n\
            InitializePropertiesInternal();\n\
            InitializeModel();\n\
{7}\n\
        }\n\
\n\
        public {0}(IObservableObject pSource,ObservableObjectOptions modelOptions,ViewModelOptions viewModelOptions) :\n\
            this(IID,pSource,modelOptions,viewModelOptions)\n\
        {\n\
        }\n\
\n\
        public {0}(IObservableObject pSource) :\n\
            this(IID,pSource,ObservableObjectOptions.None,ViewModelOptions.None)\n\
        {\n\
        }\n\
\n\
        private void InitializePropertiesInternal()\n\
        {\n\
{3}\n\
{4}\n\
        }\n\
\n\
        #region Internal Properties\n\
{5}\n\
        #endregion\n\
\n\
{6}\n\
    }\n\
";

const wchar_t * _onInvokeImplement_Template =
L"\
        protected override object OnInvoke(int methodId, object[] parameters)\n\
        {\n\
            object result = null;\n\
            switch(methodId)\n\
            {\n\
        {0}\n\
            default:\n\
                result = base.OnInvoke(methodId,parameters);\n\
                break;\n\
            }\n\
            return result;\n\
        }\n\
";

const wchar_t * _onInvokeInternal_Template =
L"\
            case (int){0}Methods.{1}:\n\
                result = this.InvokMethodHelper(\n\
                    {2}\n\
{3}\n\
                    );\n\
            break;\n\
";

const wchar_t * _onInvokeCallbackInternal_Template =
L"\
            case (int){0}Methods.{1}:\n\
                {3} _callback.{1}(\n\
{2}\n\
                    );\n\
            break;\n\
";


const wchar_t * _methodCallbackInterface_Template =
L"\
     interface I{0}MethodCallback {2}\n\
    {\n\
{1}\n\
    }\n\
";

const wchar_t * _methodCallbackClass_Template =
L"\
    class {0}MethodCallback<T> : {2}MethodCallback<T>\n\
         where T : I{0}MethodCallback\n\
    {\n\
        internal static {3} InvokeDelegate CreateInvokeDelegate(T callback)\n\
        {\n\
            var callbackClass = new {0}MethodCallback<T>(callback);\n\
            return callbackClass.InvokeDelegate;\n\
        }\n\
        internal {0}MethodCallback(T callback):\n\
            base(callback)\n\
        {\n\
        }\n\
\n\
        internal new object InvokeDelegate(int methodId, object[] parameters)\n\
        {\n\
            object result = null;\n\
            switch (methodId)\n\
            {\n\
{1}\n\
            default:\n\
                result = base.InvokeDelegate(methodId, parameters);\n\
                break;\n\
            }\n\
            return result;\n\
        }\n\
    }\n\
";

const wchar_t * _commandPropertyCallbackInterface_Template =
L"\
    interface I{0}CommandCallback {2}\n\
    {\n\
{1}\n\
    }\n\
";

const wchar_t * _commandPropertyCallbackClass_Template =
L"\
    class {0}CommandCallback<T> : {3}CommandCallback<T>\n\
         where T : I{0}CommandCallback\n\
    {\n\
        internal static Tuple<ExecuteProperty, CanExecuteProperty> CreateDelegates(T callback)\n\
        {\n\
            var callbackClass = new {0}CommandCallback<T>(callback);\n\
            return new Tuple<ExecuteProperty, CanExecuteProperty>(callbackClass.ExecuteProperty, callbackClass.CanExecuteProperty);\n\
        }\n\
 \n\
        internal {0}CommandCallback(T callback)\n\
            :base(callback)\n\
        {\n\
        }\n\
        internal new object ExecuteProperty(uint propertyid, object parameter)\n\
        {\n\
            object result = null;\n\
            switch (propertyid)\n\
            {\n\
{1}\n\
            default:\n\
                result = base.ExecuteProperty(propertyid, parameter);\n\
                break;\n\
            }\n\
            return result;\n\
        }\n\
        internal new bool CanExecuteProperty(uint propertyid, object parameter)\n\
        {\n\
            bool canExecute = true;\n\
            switch (propertyid)\n\
            {\n\
{2}\n\
            default:\n\
                canExecute = base.CanExecuteProperty(propertyid, parameter);\n\
                break;\n\
            }\n\
            return canExecute;\n\
        }\n\
        internal new static void InitializeCommandProperties(\n\
            IObservableObjectClass model,\n\
            Tuple<ExecuteProperty, CanExecuteProperty> callbacks)\n\
        {\n\
{4}\n\
            {3}CommandCallback<T>.InitializeCommandProperties(model,callbacks);\n\
        }\n\
        internal static void InitializeCommandProperties(IObservableObjectClass model,T callback)\n\
        {\n\
            InitializeCommandProperties(model,CreateDelegates(callback));\n\
        }\n\
    }\n\
";

const wchar_t * _onExecuteCommandPropertyCallbackInternal_Template =
L"\
            case (uint){0}Properties.{1}:\n\
                {3} _callback.Execute{1}({2});\n\
                break;\n\
";

const wchar_t * _onCanExecuteCommandPropertyCallbackInternal_Template =
L"\
            case (uint){0}Properties.{1}:\n\
                canExecute = _callback.CanExecute{1}({2});\n\
                break;\n\
";

const wchar_t * _initializeCommandPropertyCallbackInternal_Template =
L"\
            var commandPropertyCallback = this as I{0}CommandCallback;\n\
            if (commandPropertyCallback != null)\n\
            {\n\
                {0}CommandCallback<I{0}CommandCallback>.InitializeCommandProperties(this,commandPropertyCallback);\n\
            }\n\
";

const wchar_t * _initializeMethodCallbackInternal_Template =
L"\
            var methodCallback = this as I{0}MethodCallback;\n\
            if (methodCallback != null)\n\
            {\n\
                this.InvokeDelegate = {0}MethodCallback<I{0}MethodCallback>.CreateInvokeDelegate(methodCallback);\n\
            }\n\
";

        /// <summary>
        /// {0}: Property Name
        /// {1}: Property Type
        /// {2}: Namespace
        /// {3}: Class Name
        /// {4}: Property Access Name
        /// </summary>
const wchar_t * _internalProperty_Template =
L"\
        internal {1} {4}\n\
        {\n\
            get\n\
            {\n\
                return this.GetPropertyValue<{1}>((int){2}.{3}Properties.{0});\n\
            }\n\
            set\n\
            {\n\
                this.SetPropertyInternalHelper((int){2}.{3}Properties.{0}, value);\n\
            }\n\
        }\n\
";

const wchar_t *_csharpClassFile_Template =
L"\
using System;\n\
using Microsoft.PropertyModel;\n\
using Microsoft.PropertyModel.Library;\n\
using Windows.Foundation;\n\
\n\
namespace {0}\n\
{\n\
    public static class TypeInfoClass\n\
    {\n\
{1}\n\
        static TypeInfoClass()\n\
        {\n\
{2}\n\
        }\n\
    }\n\
    {3}\n\
}\n\
";

const wchar_t *_csharpClassEnumsFile_Template =
L"\
 using System;\n\
 \n\
 namespace {0}\n\
 {\n\
 {1}\n\
 }\n\
 ";

static int countCommandProperties(const schema::CObservableObjectInfo *pPropertyModelInfo)
{
    int count = 0;
    for (schema::_PropertyIteratorType::const_iterator iter = pPropertyModelInfo->getProperties().begin();
        iter != pPropertyModelInfo->getProperties().end();
        ++iter)
    {
        if ((*iter)->getModelType() && (*iter)->getModelType()->getModelType() == schema::ModelTypeInfoType_Command)
        {
            ++count;
        }
    }
    return count;
}

void CGenerateCSharpClasses::initialize(foundation::library::DictionaryClassPtr& parameters)
{
    CCodeGenToolBase::initialize(parameters);
    _useClassInterfaceCallback = getBoolParamater(parameters, L"useClassInterfaceCallback", true);
}

void CGenerateCSharpClasses::helpInternal(std::wostream& os)
{
    CCodeGenToolBase::helpInternal(os);
    os << "csharpClassSrc:C# classes file generation" << std::endl;
}

void CGenerateCSharpClasses::codeGenerateEnd(foundation::library::DictionaryClassPtr& parameters) 
{
    std::wstring csharpClassSrcPath = getStringParameter(parameters, L"csharpClassSrc", nullptr);

    std::wstring csharpClassEnumsSrcPath = L"";
    if (parameters.HasKey(U("csharpClassEnumsSrc")))
    {
        csharpClassEnumsSrcPath = getStringParameter(parameters, L"csharpClassEnumsSrc", nullptr);
    }

    std::wostringstream osIIdStaticProperties;
    std::wostringstream osIIdStaticInitializer;

    for (_TypeInfoClasses_MapType::const_iterator iter = _typeInfoClasses.begin();
        iter != _typeInfoClasses.end();
        ++iter)
    {
        osIIdStaticProperties << "\t\t";
        osIIdStaticProperties << FormatW(
            L"public static Guid IID_{0} { get; private set; }",
            (*iter).second.c_str(),
            nullptr);
        osIIdStaticProperties << std::endl;

        osIIdStaticInitializer << "\t\t\t";
        osIIdStaticInitializer << FormatW(
            L"IID_{0} = new Guid(\"{1}\");",
            (*iter).second.c_str(),
            toCSharpIIDType((*iter).first).c_str(),
            nullptr);
        osIIdStaticInitializer << std::endl;
    }
    // generate C# Classes file
    std::wofstream fileCSharpClasses(ToUtf8String(csharpClassSrcPath.data()),std::ios_base::out);
    fileCSharpClasses << FormatW(_csharpClassFile_Template,
        this->getNamespace().data(),
        osIIdStaticProperties.str().data(),
        osIIdStaticInitializer.str().data(),
        _osCSharpClassesSrc.str().data(),
        nullptr);
    fileCSharpClasses.close();

    if (csharpClassEnumsSrcPath.length() != 0)
    {
        // generate C# Enums file
        std::wofstream fileCSharpClassesEnums(ToUtf8String(csharpClassEnumsSrcPath.data()), std::ios_base::out);
        fileCSharpClassesEnums << FormatW(_csharpClassEnumsFile_Template,
            this->getNamespace().data(),
            _osEnums.str().data(),
            nullptr);
        fileCSharpClassesEnums.close();
    }

}
void CGenerateCSharpClasses::codeGenerate(const schema::CEnumTypeInfo *pEnumTypeInfo)
{
    emitEnum(_osEnums, pEnumTypeInfo);    
}

void CGenerateCSharpClasses::codeGenerate(const schema::CObservableObjectInfo *pPropertyModelInfo)
{
    bool isViewModel = pPropertyModelInfo->getModelType() == schema::ModelTypeInfoType_ViewModel;

    std::wstring typeInfoName = toTypeInfoName(pPropertyModelInfo->getName());

    _typeInfoClasses[pPropertyModelInfo->getType()] = typeInfoName;

    std::wstring baseClass;

    std::wostringstream osInternalProperties;
    std::wostringstream osInitializeCommandProperties;
    std::wostringstream osInitializeCollectionProperties;
    std::wostringstream osInvokeInternalMethods;

    std::wostringstream osCommandCallbackInterface;
    std::wostringstream osExecuteCommandPropertyCallback;
    std::wostringstream osCanExecuteCommandPropertyCallback;
    std::wostringstream osInitializeCommandPropertiesCallback;

    const schema::CObservableObjectInfo *pBaseType = (schema::CObservableObjectInfo *)pPropertyModelInfo->getBaseType();
    if (pBaseType != nullptr)
    {
        baseClass = this->getNamespaceLookup(pBaseType->getParent().getNamespace());
        baseClass += L".";
        baseClass += toTypeInfoName(pBaseType->getName());
    }
    else
    {
        baseClass = isViewModel ? L"ViewModel" : L"ObservableObject";
    }

    // Iterate Properties
    for(schema::_PropertyIteratorType::const_iterator iter = pPropertyModelInfo->getProperties().begin();
        iter != pPropertyModelInfo->getProperties().end();
        ++iter)
    {
        bool isAncestorProperty = ((*iter)->getFlags() & PropertyFlagType_IsAncestor) != 0;
        bool isParentProperty = (*iter)->isParent();

        bool isCoreReadyOnly = isAncestorProperty || isParentProperty;

        // Emit Internal Property Access
        std::wstring propertyAccessName = (*iter)->getName();
        if (propertyAccessName == L"Invoke")
        {
            // workaround to prevent compiler to fail on this keyword
            propertyAccessName += L"_";
        }
        osInternalProperties << FormatW(
            _internalProperty_Template,
            (*iter)->getName().data(),
            toCSharpPropertyTypeInfo(*iter).data(),
            this->getNamespace().data(),
            typeInfoName.data(),
            propertyAccessName.data(),
            nullptr);

        // Emit Initialize Property
        if(!isCoreReadyOnly && (*iter)->getModelType())
        {
            schema::ModelTypeInfoType modelTypeInfoType = (*iter)->getModelType()->getModelType();

            if(modelTypeInfoType == schema::ModelTypeInfoType_Command)
            {
                const schema::CCommandTypeInfo *pCommmandModelInfo = (schema::CCommandTypeInfo *)((*iter)->getModelType());
                const schema::CPropertyTypeInfo *pParameterTypeInfo = pCommmandModelInfo->getParameterTypeInfo();
                const schema::CPropertyTypeInfo *pResutTypeInfo = pCommmandModelInfo->getResultTypeInfo();

                const bool isAsync = pCommmandModelInfo->getIsAsync();

                std::wstring typeIID;
                if (isSimpleCommand(pCommmandModelInfo))
                {
                    typeIID = FormatW(
                        L"new Guid(\"{0}\")",
                        isAsync ? _simpleAsyncCommandTypeId : _simpleCommandTypeId,
                        nullptr);
                }
                else if (isObjectObjectCommand(pCommmandModelInfo))
                {
                    typeIID = FormatW(
                        L"new Guid(\"{0}\")",
                        isAsync ? _objectObjectAsyncCommandTypeId : _objectObjectCommandTypeId,
                        nullptr);
                }
                else
                {
                    std::wstring commandModelInfoNameInc = getModelTypeInfoIncName(pCommmandModelInfo);

                    typeIID = FormatW(L"{0}.TypeInfoClass.IID_{1}",
                        this->getNamespaceLookup(pCommmandModelInfo->getParent().getNamespace()).c_str(),
                        commandModelInfoNameInc.c_str(),
                        nullptr);
                }

                std::wstring commandModelOptions = isAsync ?
                        L"(CommandOptions)(CommonModelOptions.IsFreeThreadApartment)":
                        L"CommandOptions.None";

                std::wstring cSharpParameterTypeInfo = toCSharpInterfacePropertyTypeInfo(pParameterTypeInfo);
                std::wstring cSharpResultTypeInfo = toCSharpInterfacePropertyTypeInfo(pResutTypeInfo);

                if (!_useClassInterfaceCallback)
                {
                    osInitializeCommandProperties << "\t\t\t" <<
                        FormatW(L"{0} = new ContainedCommand<{1},{2},{3}>({4},{5},\"{0}\");",
                        propertyAccessName.data(),
                        typeInfoName.data(),
                        cSharpParameterTypeInfo.data(),
                        cSharpResultTypeInfo.data(),
                        commandModelOptions.data(),
                        typeIID.data(),
                        nullptr) << std::endl;
                }

                osCommandCallbackInterface << "\t\t";
                bool isVoidResult = pResutTypeInfo->getPropertyType() == foundation::PropertyType_Empty;
                bool isVoidParameter = pParameterTypeInfo->getPropertyType() == foundation::PropertyType_Empty;
                if (isAsync || isVoidResult)
                {
                    osCommandCallbackInterface << "void";
                }
                else
                {
                    osCommandCallbackInterface << cSharpResultTypeInfo;
                }
                osCommandCallbackInterface << " Execute" << propertyAccessName << "(";

                std::wstring executeCallbackParams;


                if (!isVoidParameter)
                {
                    osCommandCallbackInterface << cSharpParameterTypeInfo << " parameter";
                    executeCallbackParams = FormatW(
                        L"parameter.{0}<{1}>()", 
                        isAsync ? L"GetAsyncCommandParameter" : L"AssertCast",
                        cSharpParameterTypeInfo.c_str(), 
                        nullptr);
                }

                if (isAsync)
                {
                    if (!isVoidParameter)
                    {
                        osCommandCallbackInterface << ",";
                        executeCallbackParams += L",";
                    }
                    osCommandCallbackInterface << "AsyncOperation asyncOperation";
                    executeCallbackParams += L"parameter.GetAsyncCommandOperation()";
                }
                osCommandCallbackInterface << ");" << std::endl;

                osExecuteCommandPropertyCallback << FormatW(_onExecuteCommandPropertyCallbackInternal_Template,
                    toTypeInfoName(pPropertyModelInfo->getName()).c_str(),
                    (*iter)->getName().data(),
                    executeCallbackParams.c_str(),
                    isAsync || isVoidResult ? L"" : L"result =",
                    nullptr);

                if (pCommmandModelInfo->getIsUnknownState())
                {
                    osCommandCallbackInterface << "\t\tbool CanExecute" << propertyAccessName << "(";
                    if (!isVoidParameter)
                    {
                        osCommandCallbackInterface << cSharpParameterTypeInfo << " parameter";
                    }
                    osCommandCallbackInterface << ");" << std::endl;
                    osCanExecuteCommandPropertyCallback << FormatW(
                        _onCanExecuteCommandPropertyCallbackInternal_Template,
                        toTypeInfoName(pPropertyModelInfo->getName()).c_str(),
                        (*iter)->getName().data(),
                        isVoidParameter ? L"" : FormatW(L"parameter.AssertCast<{0}>()", cSharpParameterTypeInfo.c_str(), nullptr).c_str(),
                        nullptr);
                }
                osInitializeCommandPropertiesCallback << "\t\t\t" <<
                    FormatW(L"model.SetPropertyInternal((uint){3}Properties.{0},PropertyModelHelper.CreatePropertyCommand({1},{2},(uint){3}Properties.{0},callbacks));",
                    propertyAccessName.data(),
                    commandModelOptions.data(),
                    typeIID.data(),
                    typeInfoName.data(),
                    nullptr) << std::endl;
                
            }

            else if(modelTypeInfoType == schema::ModelTypeInfoType_ObservableCollection || modelTypeInfoType == schema::ModelTypeInfoType_ObservableList)
            {
                std::wstring typeIID;

                const schema::CEnumerableTypeInfo *pCollectionModelInfo = (schema::CEnumerableTypeInfo *)((*iter)->getModelType());
                const schema::CPropertyTypeInfo *pItemTypeInfo = pCollectionModelInfo->getItemTypeInfo();
                if(pItemTypeInfo->getModelType() != nullptr)
                {
                    std::wstring collectionModelInfoNameInc = getModelTypeInfoIncName(pCollectionModelInfo);
                    typeIID = FormatW(L"{0}.TypeInfoClass.IID_{1}",
                        this->getNamespaceLookup(pCollectionModelInfo->getParent().getNamespace()).c_str(),
                        collectionModelInfoNameInc.c_str(), 
                        nullptr);
                }
                else
                {
                    // a 'Core' type
                    foundation::PropertyType propertyType = pItemTypeInfo->getPropertyType();
                    // lookup for supported 'Core' types
                    if(propertyType == foundation::PropertyType::PropertyType_String)
                    {
                        typeIID = L"StringType";
                    }
                    else if(propertyType == foundation::PropertyType::PropertyType_Int16)
                    {
                        typeIID = L"Int16Type";
                    }
                    else if(propertyType == foundation::PropertyType::PropertyType_UInt16)
                    {
                        typeIID = L"UInt16Type";
                    }
                    else if(propertyType == foundation::PropertyType::PropertyType_Int32)
                    {
                        typeIID = L"Int32Type";
                    }
                    else if(propertyType == foundation::PropertyType::PropertyType_UInt32)
                    {
                        typeIID = L"UInt32Type";
                    }
                    else if(propertyType == foundation::PropertyType::PropertyType_Boolean)
                    {
                        typeIID = L"BooleanType";
                    }
                    else if(propertyType == foundation::PropertyType::PropertyType_DateTime)
                    {
                        typeIID = L"DateTimeType";
                    }
                    else if(propertyType == foundation::PropertyType::PropertyType_UInt8)
                    {
                        typeIID = L"ByteType";
                    }
                    else
                    {
                        typeIID = L"ObjectType";
                    }
                    typeIID = L"CollectionModel." + typeIID;
                }
                osInitializeCollectionProperties << "\t\t\t" <<
                    FormatW(L"{0} = new CollectionModel({1},ObservableCollectionOptions.None);",propertyAccessName.data(),typeIID.data(),nullptr) << std::endl;
            }

        }
    }

    if (osCommandCallbackInterface.tellp())
    {
        const schema::CObservableObjectInfo *pCommandCallbackBaseType = nullptr;
        for (pCommandCallbackBaseType = pBaseType;
            pCommandCallbackBaseType != nullptr;
            pCommandCallbackBaseType = (const schema::CObservableObjectInfo *)pCommandCallbackBaseType->getBaseType())
        {
            if (countCommandProperties(pCommandCallbackBaseType))
            {
                break;
            }
        }

        _osCSharpClassesSrc << FormatW(
            _commandPropertyCallbackInterface_Template,
            typeInfoName.data(),
            osCommandCallbackInterface.str().data(),
            pCommandCallbackBaseType ?
                FormatW(L": {0}.I{1}CommandCallback",
                    this->getNamespaceLookup(pCommandCallbackBaseType->getParent().getNamespace()).c_str(),
                    toTypeInfoName(pCommandCallbackBaseType->getName()).c_str(),
                    nullptr
                ).c_str() :
                L"",
            nullptr
            );
        _osCSharpClassesSrc << FormatW(
            _commandPropertyCallbackClass_Template,
            typeInfoName.data(),
            osExecuteCommandPropertyCallback.str().data(),
            osCanExecuteCommandPropertyCallback.str().data(),
            pCommandCallbackBaseType ?
                FormatW(L" {0}.{1}",
                    this->getNamespaceLookup(pCommandCallbackBaseType->getParent().getNamespace()).c_str(),
                    toTypeInfoName(pCommandCallbackBaseType->getName()).c_str(),
                    nullptr
                ).c_str() :
                L"",
            osInitializeCommandPropertiesCallback.str().c_str(),
            nullptr
            );

    }

    std::wostringstream osMethodCallbackInterface;
    std::wostringstream osMethodCallbackInvokeCallback;

    // Iterate Methods
    for(schema::_MethodIteratorType::const_iterator iter = pPropertyModelInfo->getMethods().begin();
        iter != pPropertyModelInfo->getMethods().end();
        ++iter)
    {
        bool isVoidResult =
            (*iter)->getResultType()->getPropertyType() == foundation::PropertyType_Empty;

        osMethodCallbackInterface << "\t\t";
        if ((*iter)->getIsAsync() || isVoidResult)
        {
            osMethodCallbackInterface << "void";
        }
        else
        {
            osMethodCallbackInterface << toCSharpInterfacePropertyTypeInfo((*iter)->getResultType());
        }
        osMethodCallbackInterface << " ";
        osMethodCallbackInterface << (*iter)->getName();
        osMethodCallbackInterface << "(";

        std::wostringstream osInvokeParameters;

        for(schema::_MethodParamIteratorType::const_iterator iterParam = (*iter)->getParameters().begin();
            iterParam != (*iter)->getParameters().end();
            ++iterParam)
        {
            std::wstring cSharpTypeInfo = toCSharpInterfacePropertyTypeInfo(*iterParam);
            if (iterParam != (*iter)->getParameters().begin())
            {
                osMethodCallbackInterface << ",";
                osInvokeParameters << "," << std::endl;
            }
            osMethodCallbackInterface << cSharpTypeInfo << " " << (*iterParam)->getParameterName();

            osInvokeParameters << "\t\t\t\t\t";
            osInvokeParameters << "parameters[" << (int)(iterParam-(*iter)->getParameters().begin())
                << "].AssertCast<" << cSharpTypeInfo << ">()";
        }

        if ((*iter)->getIsAsync())
        {
            if ((*iter)->getParameters().size())
            {
                osInvokeParameters << "," << std::endl;
                osMethodCallbackInterface << ",";
            }
            osMethodCallbackInterface << "AsyncOperation asyncOperation";
            osInvokeParameters << "\t\t\t\t\t";
            osInvokeParameters << "parameters[" << (int)((*iter)->getParameters().size()) << "].AssertCast<AsyncOperation>()";
        }
        osMethodCallbackInterface << ");" << std::endl;

        std::wstring invokeName = L"\"Invoke" + (*iter)->getName() + L"\"";
        if ((*iter)->getIsAsync() || (*iter)->getParameters().size())
        {
            invokeName += L",";
        }

        bool isVoidResultCallback = (*iter)->getIsAsync() || isVoidResult;

        osMethodCallbackInvokeCallback << FormatW(_onInvokeCallbackInternal_Template,
            toTypeInfoName(pPropertyModelInfo->getName()).c_str(),
            (*iter)->getName().data(),
            osInvokeParameters.str().data(),
            isVoidResultCallback ? L"":L"result =",
            nullptr);

        osInvokeInternalMethods << FormatW(_onInvokeInternal_Template,
            toTypeInfoName(pPropertyModelInfo->getName()).c_str(),
            (*iter)->getName().data(),
            invokeName.data(),
            osInvokeParameters.str().data(),
            nullptr);
    }

    if (pPropertyModelInfo->getMethods().size())
    {
        const schema::CObservableObjectInfo *pMethodCallbackBaseType =
            pBaseType && pBaseType->getMethods().size() ? pBaseType :
            nullptr;

        _osCSharpClassesSrc << FormatW(
            _methodCallbackInterface_Template,
            typeInfoName.data(),
            osMethodCallbackInterface.str().data(),
            pMethodCallbackBaseType ?
                FormatW(L": {0}.I{1}MethodCallback", 
                    this->getNamespaceLookup(pBaseType->getParent().getNamespace()).c_str(),
                    toTypeInfoName(pBaseType->getName()).c_str(),
                    nullptr
                    ).c_str() :
                L"",
            nullptr
            );
        _osCSharpClassesSrc << FormatW(
            _methodCallbackClass_Template,
            typeInfoName.data(),
            osMethodCallbackInvokeCallback.str().data(),
            pMethodCallbackBaseType ?
                FormatW(L"{0}.{1}",
                    this->getNamespaceLookup(pBaseType->getParent().getNamespace()).c_str(),
                    toTypeInfoName(pBaseType->getName()).c_str(),
                    nullptr
                    ).c_str() :
                L"",
            pMethodCallbackBaseType ? L"new" : L"",
            nullptr
            );

    }
    // Emit C# Class implementation
    _osCSharpClassesSrc << FormatW(
        isViewModel ? _implementViewModelClass_Template:_implementModelClass_Template,
        typeInfoName.data(),
        baseClass.data(),
        L"",
        _useClassInterfaceCallback && osCommandCallbackInterface.tellp() ?
        FormatW(_initializeCommandPropertyCallbackInternal_Template,
                    typeInfoName.data(),
                    nullptr).c_str() :
            osInitializeCommandProperties.str().data(),
        osInitializeCollectionProperties.str().data(),
        osInternalProperties.str().data(),
        !_useClassInterfaceCallback && pPropertyModelInfo->getMethods().size() ? 
            FormatW(_onInvokeImplement_Template,osInvokeInternalMethods.str().data(),nullptr).data(): 
            L"",
        _useClassInterfaceCallback && pPropertyModelInfo->getMethods().size() ?
            FormatW(_initializeMethodCallbackInternal_Template, typeInfoName.data(), nullptr).data() :
            L"",
        pPropertyModelInfo->getBaseType() ? L"new" : L"",
        nullptr
        );
}
void CGenerateCSharpClasses::codeGenerate(const schema::CEnumerableTypeInfo *pCollectionModelInfo)
{
    if (pCollectionModelInfo->getItemTypeInfo()->getModelType() != nullptr)
    {
        std::wstring collectionModelInfoNameInc = getModelTypeInfoIncName(pCollectionModelInfo);
        _typeInfoClasses[pCollectionModelInfo->getType()] = collectionModelInfoNameInc;
    }
}

void CGenerateCSharpClasses::codeGenerate(const schema::CCommandTypeInfo *pCommandModelInfo)
{
    if (isCoreCommand(pCommandModelInfo))
    {
        // no need to define type info for core commands
        return;
    }

    if (!pCommandModelInfo->getIsUnknownState() && isSimpleCommand(pCommandModelInfo))
    {
        // no need to define type info for simple commands
        return;
    }

    std::wstring commandModelInfoNameInc = getModelTypeInfoIncName(pCommandModelInfo);
    _typeInfoClasses[pCommandModelInfo->getType()] = commandModelInfoNameInc;
}

std::wstring CGenerateCSharpClasses::toCSharpModelTypeName(const schema::CTypeInfo*pModelTypeInfo)
{
    schema::ModelTypeInfoType modelTypeInfoType = pModelTypeInfo->getModelType();
    if(modelTypeInfoType == schema::ModelTypeInfoType_ObservableCollection || modelTypeInfoType == schema::ModelTypeInfoType_ObservableList)
    {
        return L"ObservableCollection";
    }
    else if(modelTypeInfoType == schema::ModelTypeInfoType_Command)
    {
        return L"Command";
    }
    else if(modelTypeInfoType == schema::ModelTypeInfoType_AsyncOperation)
    {
        return L"AsyncOperation";
    }
    else
    {
        std::wstring typeName;
		bool useInterface = false;
        if(&pModelTypeInfo->getParent() != this->getModelSchema())
        {
            typeName = this->getNamespaceLookup(pModelTypeInfo->getParent().getNamespace());
            typeName += L".";
            // the type is on a external namespace so we can't assume the type is a class
            useInterface = true;
        }
		if(!useInterface && modelTypeInfoType != schema::ModelTypeInfoType_Enum)
		{			
			typeName += toTypeInfoName(pModelTypeInfo->getName());
		}
		else
		{
			typeName += pModelTypeInfo->getName();
		}

        return typeName;
    }
}

std::wstring CGenerateCSharpClasses::toCSharpInterfaceModelTypeName(const schema::CTypeInfo*pModelTypeInfo)
{
    schema::ModelTypeInfoType modelTypeInfoType = pModelTypeInfo->getModelType();
    if(modelTypeInfoType == schema::ModelTypeInfoType_ObservableCollection)
    {
        return L"IObservableCollection";
    }
    else if(modelTypeInfoType == schema::ModelTypeInfoType_ObservableList)
    {
        return L"IObservableList";
    }
    else if(modelTypeInfoType == schema::ModelTypeInfoType_Command)
    {
        return L"ICommand";
    }
    else if(modelTypeInfoType == schema::ModelTypeInfoType_AsyncOperation)
    {
        return L"IAsyncOperation";
    }
    else
    {
        std::wstring typeName;
        if(&pModelTypeInfo->getParent() != this->getModelSchema())
        {
            typeName = this->getNamespaceLookup(pModelTypeInfo->getParent().getNamespace());
            typeName += L".";
        }
        typeName += pModelTypeInfo->getName();
        return typeName;
    }
}

std::wstring CGenerateCSharpClasses::toCSharpPropertyTypeInfo(
        const schema::CPropertyTypeInfo*pPropertyTypeInfo)
{
    if(pPropertyTypeInfo->getModelType())
    {
        std::wstring typeName = toCSharpModelTypeName(pPropertyTypeInfo->getModelType());
        if (isPropertyTypeInfoArray(pPropertyTypeInfo))
        {
            typeName += L"[]";
        }
        return typeName;
    }
    else
    {
        return toCSharpType(pPropertyTypeInfo->getPropertyType());
    }
}

std::wstring CGenerateCSharpClasses::toCSharpInterfacePropertyTypeInfo(
        const schema::CPropertyTypeInfo*pPropertyTypeInfo)
{
    if(pPropertyTypeInfo->getModelType())
    {
        return toCSharpInterfaceModelTypeName(pPropertyTypeInfo->getModelType());
    }
    else
    {
        return toCSharpType(pPropertyTypeInfo->getPropertyType());
    }
}

std::wstring CGenerateCSharpClasses::toCSharpType(foundation::PropertyType propertyType)
{
    // exceptions for Windows runtime
    if((propertyType & 0xff) == foundation::PropertyType::PropertyType_DateTime)
    {
        return toPropertyType(propertyType,L"DateTimeOffset",L"[]");
    }
    else
    {
        return CGenerateCSharpBase::toCSharpType(propertyType);
    }
}

std::wstring CGenerateCSharpClasses::getNamespaceLookup(const std::wstring& namespaceStr)
{
    if (namespaceStr == L"pmod")
    {
        return L"Microsoft.PropertyModel";
    }
    return CGenerateCSharpBase::getNamespaceLookup(namespaceStr);
}


