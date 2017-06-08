/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:GenerateJava.cpp
****/

#include "GenerateJava.h"
#include "guidhelper.h"

const wchar_t* _javaTypeInfo_Template =
L"\
package {0};\n\
\n\
public class {1} {\n\
\n\
{2}\n\
\n\
}\n\
";

const wchar_t* _javaTypeInfoEntry_Template =
L"\
    public static class {0} {\n\
\n\
        public static final String IID = \"{1}\";\n\
\n\
        //\n\
        // property ids\n\
        //\n\
{2}\n\
        //\n\
        // method ids\n\
        //\n\
{3}\n\
        //\n\
        // event ids\n\
        //\n\
{4}\n\
    }\n\
\n\
";

const wchar_t* _javaDeclareEnum_Template =
L"\
package {0};\n\
\n\
import java.lang.reflect.Array;\n\
\n\
public enum {2} implements {1}.EnumValue {\n\
{3};\n\
    \n\
    private final int value;\n\
    {2}(int value) { this.value = value; }\n\
    public int getValue() { return value; }\n\
\n\
    private static final java.util.Map<Integer, {2}> typesByValue = new java.util.HashMap<Integer, {2}>();\n\
\n\
    static {\n\
        for ({2} type : {2}.values()) {\n\
            typesByValue.put(type.value, type);\n\
        }\n\
    }\n\
    public static {2} forValue(Object value) {\n\
        return typesByValue.get(((Integer)value).intValue());\n\
    }\n\
    public static {2} forValue(int value) {\n\
        return typesByValue.get(value);\n\
    }\n\
    public static {2}[] forValues(Object value) {\n\
        assert value instanceof int[];\n\
        return forValues((int[])value);\n\
    }\n\
    public static {2}[] forValues(int[] value) {\n\
        {2}[] values = ({2}[])Array.newInstance({2}.class, value.length);\n\
        for (int i = 0; i < value.length; ++i) {\n\
            values[i] = forValue(value[i]);\n\
        }\n\
        return values;\n\
    }\n\
    public static Integer[] getValues({2}[] value) {\n\
        Integer[] values = (Integer[])Array.newInstance(Integer.class, value.length);\n\
        for (int i = 0; i < value.length; ++i) {\n\
            values[i] = new Integer(value[i].getValue());\n\
        }\n\
        return values;\n\
    }\n\
}\n\
";

const wchar_t * _javaInnerEnum_Template =
L"\
        public enum {0} {\n\
{1};\n\
     \n\
            private final int id;\n\
            {0}(int id) { this.id = id; }\n\
            public int getValue() { return id; }\n\
        }\n\
";

const wchar_t * _javaModelInterfaceType_Declare =
L"\
package {0};\n\
\n\
public interface {1} extends {2} {\n\
\n\
    //\n\
    // properties\n\
    //\n\
{3}\n\
\n\
    //\n\
    // methods\n\
    //\n\
{4}\n\
\n\
}\n\
";

const wchar_t * _javaModelType_Declare =
L"\
package {0};\n\
\n\
public class {3}Impl extends {2} implements {3} {\n\
\n\
    // public constructor\n\
    public {3}Impl(\n\
        {1}.ObservableObjectImpl observableObject,\n\
        {1}.ClassFactory classFactory) {\n\
        super(observableObject, classFactory);\n\
    }\n\
\n\
    //\n\
    // properties\n\
    //\n\
{5}\n\
\n\
    //\n\
    // methods\n\
    //\n\
{6}\n\
\n\
}\n\
";

// {0}: property name
// {1}: property type
const wchar_t * _javaGetPropertyDeclareTemplate =
L"\n\
    public {1} get{0}();\n\
";

// {0}: property name
// {1}: property type
// {2}: cast func
// {3}: TypeInfo class name
// {4}: this interface name
// {5}: array class
const wchar_t * _javaGetPropertyTemplate =
L"\n\
    @Override\n\
    public {1} get{0}() {\n\
        return {2}(this.getProperty({3}.{4}.Properties.{0}.getValue()){5});\n\
    }\n\
";

// {0}: property name
// {1}: property type
const wchar_t * _javaSetPropertyDeclareTemplate =
L"\n\
    public void set{0}({1} value);\n\
";

// {0}: property name
// {1}: property type
// {2}: TypeInfo class name
// {3}: this interface name
const wchar_t * _javaSetPropertyTemplate =
L"\n\
    @Override\n\
    public void set{0}({1} value) {\n\
        this.setProperty({2}.{3}.Properties.{0}.getValue(), value);\n\
    }\n\
";

// {0}: property name
// {1}: property type
const wchar_t * _javaSetPropertyEnumArrayDeclareTemplate =
L"\n\
    public void set{0}({1}[] value);\n\
";

// {0}: property name
// {1}: property type
// {2}: TypeInfo class name
// {3}: this interface name
const wchar_t * _javaSetPropertyEnumArrayTemplate =
L"\
    public void set{0}({1}[] value) {\n\
        this.setProperty({2}.{3}.Properties.{0}.getValue(), {1}.getValues(value));\n\
    }\n\
";

// {0}: method name
// {1}: result type
// {2}: parameters declare
const wchar_t * _javaMethodDeclareTemplate =
L"\n\
    public {1} {0}({2});\n\
";

// {0}: method name
// {1}: method id name
// {2}: result type
// {3}: parameters declare
// {4}: parameters array
// {5}: cast func (unused)
// {6}: TypeInfo class name
// {7}: this interface name
const wchar_t * _javaMethodTemplate =
L"\n\
    @Override\n\
    public {2} {0}({3}) {\n\
        this.invoke({6}.{7}.Methods.{1}.getValue(), new Object[]{ {4} });\n\
    }\n\
";

// {0}: method name
// {1}: method id name
// {2}: result type
// {3}: parameters declare
// {4}: parameters array
// {5}: cast func
// {6}: TypeInfo class name
// {7}: this interface name
const wchar_t * _javaMethodWithResultTemplate =
L"\n\
    @Override\n\
    public {2} {0}({3}) {\n\
        return {5}(this.invoke({6}.{7}.Methods.{1}.getValue(), new Object[]{ {4} }));\n\
    }\n\
";

const wchar_t * _javaAddTypeTemplate =
L"\
        _types.put({1}.{0}.IID, {0}Impl.class);\n\
";

const wchar_t * _javaFactoryTypesTemplate =
L"\
package {0};\n\
\n\
import java.util.HashMap;\n\
import java.util.Map;\n\
\n\
import {1}.FoundationObjectImpl;\n\
\n\
public final class {2} { \n\
\n\
    private static Map<String, Class<? extends FoundationObjectImpl>> _types;\n\
\n\
    static {\n\
        _types = new HashMap<String, Class<? extends FoundationObjectImpl>>();\n\
{3}\
    }\n\n\
    public static Map<String, Class<? extends FoundationObjectImpl>> getTypes() {\n\
        return _types;\n\
    }\n\
}\n\
";

const wchar_t* _packagePropertyModel = L"com.microsoft.pmod";
const wchar_t* _observableObjectInterfaceName = L"ObservableObject";

const wchar_t* _observableCollectionInterfaceName = L"ObservableCollection";
const wchar_t* _observableListInterfaceName = L"ObservableList";

const wchar_t* _commandInterfaceName = L"Command";
const wchar_t* _commandResultInterfaceName = L"CommandResult";
const wchar_t* _commandParameterInterfaceName = L"CommandParameter";
const wchar_t* _simpleCommandInterfaceName = L"SimpleCommand";

const wchar_t* _objectInterfaceName = L"FoundationObject";
const wchar_t* _dictionaryClassName = L"Dictionary";

const wchar_t* _asyncOperationInterfaceName = L"AsyncOperation";
const wchar_t* _asyncBaseOperationInterfaceName = L"BaseAsyncOperation";

const wchar_t* _typeInfoClassName = L"TypeInfo";

static std::wstring formatPackagePropertyModelName(const wchar_t* name)
{
    return FormatW(L"{0}.{1}", _packagePropertyModel, name, nullptr);
}

static std::wstring formatPackagePropertyModelGenericType(const wchar_t* name,const wchar_t* genericType)
{
    return FormatW(L"{0}.{1}<{2}>", _packagePropertyModel, name, genericType,nullptr);
}

static std::wstring formatPackagePropertyModelGenericType2(const wchar_t* name, const wchar_t* genericType1, const wchar_t* genericType2)
{
    return FormatW(L"{0}.{1}<{2},{3}>", _packagePropertyModel, name, genericType1, genericType2, nullptr);
}

void CGenerateJava::initialize(foundation::library::DictionaryClassPtr& parameters)
{
    CCodeGenToolBase::initialize(parameters);

    _javaPackageName = getStringParameter(parameters, L"javapackagename", nullptr);
    _javaSourceDir = getStringParameter(parameters, L"javasourcedir", nullptr);

#if defined(_MSC_VER)
    const wchar_t* dir_sep = L"\\";
#else
    const wchar_t* dir_sep = L"/";
#endif
    std::wstring package_source_dir = _javaPackageName;
    replace(package_source_dir, L".", dir_sep);

    _javaSourceDir += dir_sep;
    _javaSourceDir += package_source_dir;
    _javaSourceDir += dir_sep;
}

void CGenerateJava::helpInternal(std::wostream& os)
{
    CCodeGenToolBase::helpInternal(os);
    os << "javapackagename: Java package name" << std::endl;
    os << "javasourcedir: Java source directory of generated files" << std::endl;
}

void CGenerateJava::codeGenerateEnd(foundation::library::DictionaryClassPtr& parameters) 
{
    std::wstring factoryTypesName = L"FactoryTypes";

    std::wstring factoryTypesContent = FormatW(
        _javaFactoryTypesTemplate,
        _javaPackageName.data(),         // {0}
        _packagePropertyModel,           // {1}
        factoryTypesName.data(),         // {2}
        _osAddFactoryTypes.str().data(), // {3}
        nullptr
        ).data();

    // generate factory types
    std::wofstream fileJavaFactoryTypes(toJavaSourceFile(factoryTypesName.data()), std::ios_base::out);
    fileJavaFactoryTypes << factoryTypesContent;
    fileJavaFactoryTypes.close();

    // generate type info
    std::wstring typeInfoContent = FormatW(
        _javaTypeInfo_Template,
        _javaPackageName.data(),         // {0}
        _typeInfoClassName,              // {1}
        _osTypeInfoEntries.str().data(), // {2}
        nullptr
        ).data();

    std::wofstream fileJavaTypeInfo(toJavaSourceFile(_typeInfoClassName), std::ios_base::out);
    fileJavaTypeInfo << typeInfoContent;
    fileJavaTypeInfo.close();
}

void CGenerateJava::codeGenerate(const schema::CEnumTypeInfo *pEnumTypeInfo)
{
    std::wostringstream osEnums;

    // Iterate Enums
    for(schema::_EnumInfoVectorType::const_iterator iter = pEnumTypeInfo->getEnums().begin();
        iter != pEnumTypeInfo->getEnums().end();
        ++iter)
    {
        // generate enum
        if(iter != pEnumTypeInfo->getEnums().begin())
        {
            osEnums << "," << std::endl;
        }
        INT32 enumValue = (INT32)std::get<1>(*iter);
        osEnums << "\t" << std::get<0>(*iter) << "(" << enumValue << ")";
    }

    std::wstring declareEnumContent = FormatW(_javaDeclareEnum_Template,
        _javaPackageName.data(),            // {0}
        _packagePropertyModel,              // {1}
        pEnumTypeInfo->getName().data(),    // {2}
        osEnums.str().data(), // {3}
        nullptr
        ).data();

    std::wofstream fileJavaModelType(toJavaSourceFile(pEnumTypeInfo->getName().data()),std::ios_base::out);
    fileJavaModelType << declareEnumContent;
    fileJavaModelType.close();
}

void CGenerateJava::codeGenerate(const schema::CObservableObjectInfo *pPropertyModelInfo)
{
    bool isViewModel = pPropertyModelInfo->getModelType() == schema::ModelTypeInfoType_ViewModel;

    std::wstring typeInfoName = toTypeInfoName(pPropertyModelInfo->getName());

    _osAddFactoryTypes << FormatW(
        _javaAddTypeTemplate,
        typeInfoName.data(),
        _typeInfoClassName,
        nullptr);

    std::wostringstream osPropertiesEnum;
    std::wostringstream osMethodsEnum;

    std::wostringstream osPropertiesDeclare;
    std::wostringstream osPropertiesImplement;
    std::wostringstream osMethodsDeclare;
    std::wostringstream osMethodsImplement;

    // Iterate Properties
    for(schema::_PropertyIteratorType::const_iterator iter = pPropertyModelInfo->getProperties().begin();
        iter != pPropertyModelInfo->getProperties().end();
        ++iter)
    {
        // generate enum
        if(iter != pPropertyModelInfo->getProperties().begin())
        {
            osPropertiesEnum << "," << std::endl;
        }
        osPropertiesEnum << "            " << (*iter)->getName() << "(";
        if((*iter)->isParent())
        {
            osPropertiesEnum << "0x2000";
        }
        else
        {
            osPropertiesEnum << (*iter)->getId();
        }
        
        if(isViewModel && !(*iter)->isParent())
        {
            osPropertiesEnum << " + 0x1000";
        }
        osPropertiesEnum << ")";

        // generate implementation
        std::wstring javaPropertyType = toJavaPropertyTypeInfo(*iter);
        UINT32 flags = (*iter)->getFlags();
        if (flags & PropertyFlagType_CanRead)
        {
            bool isFoundationArray = false;
            std::wstring arrayClass = L"";
            std::wstring toCast = toCastFunc(*iter, isFoundationArray);

            if (isFoundationArray)
            {
                arrayClass += L", ";
                arrayClass += javaPropertyType.data();
                arrayClass += L".class";
            }

            osPropertiesImplement << FormatW(
                _javaGetPropertyTemplate,
                (*iter)->getName().data(), // {0}
                javaPropertyType.data(),   // {1}
                toCast.data(),             // {2}
                _typeInfoClassName,        // {3}
                typeInfoName.data(),       // {4}
                arrayClass.data(),         // {5}
                nullptr);

            osPropertiesDeclare << FormatW(
                _javaGetPropertyDeclareTemplate,
                (*iter)->getName().data(),
                javaPropertyType.data(),
                nullptr);
        }

        if (flags & PropertyFlagType_CanWrite)
        {
            const bool isEnumArray = isEnumArrayProperty(*iter);
            osPropertiesImplement << FormatW(
                isEnumArray ? _javaSetPropertyEnumArrayTemplate : _javaSetPropertyTemplate,
                (*iter)->getName().data(),
                isEnumArray ? javaPropertyType.substr(0, javaPropertyType.length() - 2).data() : javaPropertyType.data(),
                _typeInfoClassName,
                typeInfoName.data(),
                nullptr);

            osPropertiesDeclare << FormatW(
                isEnumArray ? _javaSetPropertyEnumArrayDeclareTemplate : _javaSetPropertyDeclareTemplate,
                (*iter)->getName().data(),
                isEnumArray ? javaPropertyType.substr(0, javaPropertyType.length() - 2).data() : javaPropertyType.data(),
                nullptr);
        }
    }

    // Iterate Methods
    for(schema::_MethodIteratorType::const_iterator iter = pPropertyModelInfo->getMethods().begin();
        iter != pPropertyModelInfo->getMethods().end();
        ++iter)
    {
        // generate enum
        if(iter != pPropertyModelInfo->getMethods().begin())
        {
            osMethodsEnum << "," << std::endl;
        }
        osMethodsEnum << "\t\t" << (*iter)->getName() << "(" << (*iter)->getId() << ")";

        bool isAsync = (*iter)->getIsAsync();
        bool hasResult = isAsync || ((*iter)->getResultType()->getPropertyType() != foundation::PropertyType_Empty);

        std::wstring javaReturnType = isAsync ? 
            toJavaAsyncOperationTypeInfo((*iter)->getResultType()) :
            toJavaPropertyTypeInfo((*iter)->getResultType());

        std::wostringstream osDeclareParameters;
        std::wostringstream osInvokeParameters;

        for(schema::_MethodParamIteratorType::const_iterator iterParam = (*iter)->getParameters().begin();
            iterParam != (*iter)->getParameters().end();
            ++iterParam)
        {
            if(iterParam != (*iter)->getParameters().begin())
            {
                osDeclareParameters << ",";
                osInvokeParameters << ",";
            }
            std::wstring javaParameterType = toJavaPropertyTypeInfo(*iterParam);
            osDeclareParameters << javaParameterType << " " << (*iterParam)->getParameterName();
            osInvokeParameters << (*iterParam)->getParameterName();
        }
        std::wstring methodName = (*iter)->getName();
        // use java convention for methods by making first letter lowercase
        methodName[0] = tolower(methodName[0]);

        // TODO: ...
        bool dummy = false;

        osMethodsImplement << FormatW(
            hasResult ? _javaMethodWithResultTemplate : _javaMethodTemplate,
            methodName.data(),
            (*iter)->getName().data(),
            javaReturnType.data(),
            osDeclareParameters.str().data(),
            osInvokeParameters.str().data(),
            hasResult 
                ? (isAsync
                    ? L"toFoundationObject"
                    : toCastFunc((*iter)->getResultType(), dummy).data())
                : L"",
            _typeInfoClassName,
            typeInfoName.data(),
            nullptr);

        osMethodsDeclare << FormatW(
            _javaMethodDeclareTemplate,
            methodName.data(),
            javaReturnType.data(),
            osDeclareParameters.str().data(),
            nullptr);

    }

    // Declare a class for model type definition
    std::wstring  typeId = ToWideString(pPropertyModelInfo->getType());

    // Extend class name
    std::wstring javaExtendsClass = pPropertyModelInfo->getBaseType() ?
        toTypeInfoName(pPropertyModelInfo->getBaseType()->getName() + L"Impl") :
        formatPackagePropertyModelName(L"ObservableObjectImpl");

    std::wstring modelTypeDeclare = FormatW(
        _javaModelType_Declare,
        _javaPackageName.data(),            // {0}
        _packagePropertyModel,              // {1}
        javaExtendsClass.data(),            // {2}
        typeInfoName.data(),                // {3}
        typeId.data(),                      // {4}
        // Properties implement             // {5}
        osPropertiesImplement.str().data(),
        // Methods implement                // {6}
        osMethodsImplement.str().data(),
        // who extends
        nullptr);

    std::wstring interfaceTypeDeclare = FormatW(
        _javaModelInterfaceType_Declare,
        // Java package name {0}
        _javaPackageName.data(),
        // Java interface name {1}
        typeInfoName.data(),
        // Base interface (ObservableObject) {2}
        formatPackagePropertyModelName(_observableObjectInterfaceName).data(),
        // Properties {3}
        osPropertiesDeclare.str().data(),
        // Methods {4}
        osMethodsDeclare.str().data(),
        nullptr);

    _osTypeInfoEntries << FormatW(
        _javaTypeInfoEntry_Template,
        // Java interface name              // {0}
        typeInfoName.data(),
        // IID
        typeId.data(),                      // {1}
        // Properties                       // {2}
        pPropertyModelInfo->getProperties().size() ? 
            FormatW(_javaInnerEnum_Template,
                L"Properties",
                osPropertiesEnum.str().data(),
                nullptr
                ).data() :
            L"",
        // Methods                          // {3}
        pPropertyModelInfo->getMethods().size() ? 
            FormatW(_javaInnerEnum_Template,
                L"Methods",
                osMethodsEnum.str().data(),
                nullptr
                ).data() :
            L"",
        // Events                           // {4}   
        L"",
        nullptr);

        std::wofstream fileJavaModelType(toJavaSourceFile((typeInfoName + L"Impl").data()),std::ios_base::out);
        fileJavaModelType << modelTypeDeclare;
        fileJavaModelType.close();

        std::wofstream fileJavaInterfaceType(toJavaSourceFile(typeInfoName.data()), std::ios_base::out);
        fileJavaInterfaceType << interfaceTypeDeclare;
        fileJavaInterfaceType.close();
}

std::string CGenerateJava::toJavaSourceFile(const wchar_t *name)
{
    return ToUtf8String((_javaSourceDir + name + L".java").data());
}

std::wstring CGenerateJava::toJavaPropertyTypeInfo(
    const schema::CPropertyTypeInfo* pPropertyTypeInfo,
    bool isObject)
{
    assert(pPropertyTypeInfo);

    if (pPropertyTypeInfo->getModelType())
    {
        std::wstring modelType = toJavaModelTypeName(pPropertyTypeInfo->getModelType());
        if ((pPropertyTypeInfo->getPropertyType() & 0x400))
        {
            modelType += L"[]";
        }
        return std::move(modelType);
    }

    return toJavaType(pPropertyTypeInfo->getPropertyType(), isObject);
}

std::wstring CGenerateJava::toJavaAsyncOperationTypeInfo(const schema::CPropertyTypeInfo*pPropertyTypeInfo)
{
    if (pPropertyTypeInfo == nullptr || pPropertyTypeInfo->getPropertyType() == foundation::PropertyType_Empty)
    {
        return formatPackagePropertyModelName(_asyncBaseOperationInterfaceName);
    }
    return formatPackagePropertyModelGenericType(
        _asyncOperationInterfaceName,
        toJavaPropertyTypeInfo(pPropertyTypeInfo, true).c_str());
}

std::wstring CGenerateJava::toJavaModelTypeName(const schema::CTypeInfo* pModelTypeInfo)
{
    assert(pModelTypeInfo);

    schema::ModelTypeInfoType modelTypeInfoType = pModelTypeInfo->getModelType();

    if (modelTypeInfoType == schema::ModelTypeInfoType_Dictionary)
    {
        return formatPackagePropertyModelName(_dictionaryClassName);
    }

    if (modelTypeInfoType == schema::ModelTypeInfoType_Object)
    {
        return formatPackagePropertyModelName(_objectInterfaceName);
    }

    if (modelTypeInfoType == schema::ModelTypeInfoType_ObservableCollection)
    {
        return formatPackagePropertyModelGenericType(
            _observableCollectionInterfaceName,
            toJavaPropertyTypeInfo(((const::schema::CEnumerableTypeInfo *)pModelTypeInfo)->getItemTypeInfo(),true).c_str());
    }

    if (modelTypeInfoType == schema::ModelTypeInfoType_ObservableList)
    {
        return formatPackagePropertyModelGenericType(
            _observableListInterfaceName,
            toJavaPropertyTypeInfo(((const::schema::CEnumerableTypeInfo *)pModelTypeInfo)->getItemTypeInfo(), true).c_str());
    }

    if (modelTypeInfoType == schema::ModelTypeInfoType_Command)
    {
        const schema::CCommandTypeInfo *pCommandModelInfo = (const::schema::CCommandTypeInfo *)pModelTypeInfo;
        const schema::CPropertyTypeInfo *pResultTypeInfo = pCommandModelInfo->getResultTypeInfo();
        const schema::CPropertyTypeInfo *pParameterTypeInfo = pCommandModelInfo->getParameterTypeInfo();

        if (isSimpleCommand(pCommandModelInfo))
        {
            return formatPackagePropertyModelName(_simpleCommandInterfaceName);
        }
        else if (isCommandParameter(pCommandModelInfo))
        {
            return formatPackagePropertyModelGenericType(
                _commandParameterInterfaceName,
                toJavaPropertyTypeInfo(pParameterTypeInfo, true).c_str());
        }
        else if (isCommandResult(pCommandModelInfo))
        {
            return formatPackagePropertyModelGenericType(
                _commandResultInterfaceName,
                toJavaPropertyTypeInfo(pResultTypeInfo, true).c_str());
        }
        return formatPackagePropertyModelGenericType2(
            _commandInterfaceName,
            toJavaPropertyTypeInfo(pResultTypeInfo, true).c_str(),
            toJavaPropertyTypeInfo(pParameterTypeInfo, true).c_str()
            );
    }

    if (modelTypeInfoType == schema::ModelTypeInfoType_AsyncOperation)
    {
        return toJavaAsyncOperationTypeInfo(((const::schema::CAsyncOperationTypeInfo *)pModelTypeInfo)->getResultTypeInfo());
    }

    std::wstring typeName;
    if (&pModelTypeInfo->getParent() != this->getModelSchema())
    {
        typeName = this->getNamespaceLookup(pModelTypeInfo->getParent().getNamespace());
        typeName += L".";
    }

    if (modelTypeInfoType == schema::ModelTypeInfoType_Enum)
    {
        typeName += pModelTypeInfo->getName();
    }
    else
    {
        typeName += toTypeInfoName(pModelTypeInfo->getName());
    }

    return typeName;
}

std::wstring CGenerateJava::toJavaType(foundation::PropertyType propertyType, bool isObject)
{
    std::wstring javaType;
    switch(propertyType & 0xff)
    {
    case foundation::PropertyType::PropertyType_Empty:
        javaType = L"void";
        break;
    case foundation::PropertyType::PropertyType_UInt8:
        javaType = isObject ? L"Byte":L"byte";
        break;
    case foundation::PropertyType::PropertyType_Int16:
    case foundation::PropertyType::PropertyType_UInt16:
        javaType = isObject ? L"Short" : L"short";
        break;
    case foundation::PropertyType::PropertyType_UInt32:
    case foundation::PropertyType::PropertyType_Int32:
        javaType = isObject ? L"Integer" : L"int";
        break;
    case foundation::PropertyType::PropertyType_Int64:
    case foundation::PropertyType::PropertyType_UInt64:
        javaType = isObject ? L"Long" : L"long";
        break;
    case foundation::PropertyType::PropertyType_Single:
        javaType = isObject ? L"Float" : L"float";
        break;
    case foundation::PropertyType::PropertyType_Double:
        javaType = isObject ? L"Double" : L"double";
         break;
    case foundation::PropertyType::PropertyType_Char16:
        javaType = isObject ? L"Character" : L"char";
        break;
    case foundation::PropertyType::PropertyType_Boolean:
        javaType = isObject ? L"Boolean" : L"boolean";
        break;
    case foundation::PropertyType::PropertyType_String:
        javaType = L"String";
        break;
    case foundation::PropertyType::PropertyType_Inspectable:
        javaType = L"Object";
        break;
    case foundation::PropertyType::PropertyType_DateTime:
        javaType = L"java.util.Date";
        break;
    case foundation::PropertyType::PropertyType_Guid:
        // TODO: support a GUID type in Java
        javaType = L"String";
        break;
    case foundation::PropertyType::PropertyType_Point:
    case foundation::PropertyType::PropertyType_Size:
    case foundation::PropertyType::PropertyType_Rect:
    default:
        javaType = L"";
        assert(false);
    }

    return toPropertyType(propertyType, javaType.c_str(), L"[]");
}

std::wstring CGenerateJava::toCastFunc(
    const schema::CPropertyTypeInfo * pPropertyTypeInfo,
    bool& isFoundationArray)
{
    std::wstring toCast;
    const bool isArray = (pPropertyTypeInfo->getPropertyType() & 0x400) != 0;

    isFoundationArray = false;

    std::wstring javaPropertyType = toJavaPropertyTypeInfo(pPropertyTypeInfo);

    if (pPropertyTypeInfo->getModelType())
    {
        if (pPropertyTypeInfo->getModelType()->getModelType() == schema::ModelTypeInfoType_Enum)
        {
            if (isArray)
            {
                toCast = javaPropertyType.substr(0, javaPropertyType.length() - 2) + L".forValues";
            }
            else
            {
                toCast = javaPropertyType + L".forValue";
            }
        }
        else
        {
            toCast = L"toFoundationObject";
            if (isArray)
            {
                isFoundationArray = true;
                toCast += L"Array";
            }
            else
            {
                const std::wstring modelTypeName = toJavaModelTypeName(pPropertyTypeInfo->getModelType());
                if (modelTypeName.find_first_of(L"com") != 0)
                {
                    toCast = std::wstring(L"(") + modelTypeName + std::wstring(L"Impl)") + toCast;
                }
            }
        }
    }
    else
    {
        if(pPropertyTypeInfo->getPropertyType() == foundation::PropertyType::PropertyType_DateTime)
        {
            toCast = L"toDate";
        }
        else
        {
            javaPropertyType[0] = toupper(javaPropertyType[0]);
            if (isArray)
            {
                javaPropertyType = javaPropertyType.substr(0, javaPropertyType.length() - 2) + L"Array";
            }
            toCast = L"to" + javaPropertyType;
        }
    }

    return toCast;
}

bool CGenerateJava::isEnumArrayProperty(const schema::CPropertyTypeInfo * pPropertyTypeInfo)
{
    if (pPropertyTypeInfo->getModelType())
    {
        if (pPropertyTypeInfo->getModelType()->getModelType() == schema::ModelTypeInfoType_Enum)
        {
            if ((pPropertyTypeInfo->getPropertyType() & 0x400) != 0)
            {
                return true;
            }
        }
    }

    return false;
}

void CGenerateJava::generateTypeInfo() const
{

}
