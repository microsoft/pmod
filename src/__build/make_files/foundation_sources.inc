

# Foundation Source Files
FOUNDATION_PAL_SRC_FILES := \
    $(FOUNDATION_PAL_SRC)/hstring.cpp \
    $(FOUNDATION_PAL_SRC)/PropertyValue.cpp \
    $(FOUNDATION_PAL_SRC)/ComInterfaces.cpp \
    $(FOUNDATION_PAL_SRC)/FoundationExports.cpp \

# Foundation Source Generated Files
FOUNDATION_GENERATED_SRC_FILES := \
    $(CODE_GEN_SRC)/foundation/FoundationErrorsTypeInfo.g.cpp \

# Foundation library Source Files
FOUNDATION_SRC_FILES := \
    $(FOUNDATION_SRC)/ActivationFactory.cpp \
    $(FOUNDATION_SRC)/AdapterFactoryFactory.cpp \
    $(FOUNDATION_SRC)/AsyncOperationClassInfoCache.cpp \
    $(FOUNDATION_SRC)/AsyncOperationTypeInfo.cpp \
    $(FOUNDATION_SRC)/DebugUtil.cpp \
    $(FOUNDATION_SRC)/DictionaryFactory.cpp \
    $(FOUNDATION_SRC)/EnumTypeClassInfoCache.cpp \
    $(FOUNDATION_SRC)/EnumTypeInfo.cpp \
    $(FOUNDATION_SRC)/EnumValue.cpp \
    $(FOUNDATION_SRC)/EnumerableClassInfoCache.cpp \
    $(FOUNDATION_SRC)/EnumerableTypeInfo.cpp \
    $(FOUNDATION_SRC)/FoundationAssert.cpp \
    $(FOUNDATION_SRC)/FoundationInterfaces.cpp \
    $(FOUNDATION_SRC)/FoundationPrivateInterfaces.cpp \
    $(FOUNDATION_SRC)/FoundationLibraryInstance.cpp \
    $(FOUNDATION_SRC)/FoundationTracker.cpp \
    $(FOUNDATION_SRC)/GenericAdapterFactory.cpp \
    $(FOUNDATION_SRC)/InterfaceAdapterFactoryHost.cpp \
    $(FOUNDATION_SRC)/MethodInfo.cpp \
    $(FOUNDATION_SRC)/NamespaceStatics.cpp \
    $(FOUNDATION_SRC)/NodeStatics.cpp \
    $(FOUNDATION_SRC)/ObjectUniqueId.cpp \
    $(FOUNDATION_SRC)/ObjectFactory.cpp \
    $(FOUNDATION_SRC)/ObjectDispatchInfoBase.cpp \
    $(FOUNDATION_SRC)/ObjectDispatchClassInfoCache.cpp \
    $(FOUNDATION_SRC)/ObjectDispatchInfo.cpp \
    $(FOUNDATION_SRC)/PropertyInfo.cpp \
    $(FOUNDATION_SRC)/PropertyInfoProxy.cpp\
    $(FOUNDATION_SRC)/PropertyTypeInfo.cpp \
    $(FOUNDATION_SRC)/PropertyValueInternalStatics.cpp \
    $(FOUNDATION_SRC)/TypeAdapterFactory.cpp \
    $(FOUNDATION_SRC)/TypeInfo.cpp \
    $(FOUNDATION_SRC)/TypeInfoFactory.cpp \
    $(FOUNDATION_SRC)/TypeInfoStatics.cpp \
