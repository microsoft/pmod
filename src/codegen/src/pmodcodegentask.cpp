/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:pmodcodegentask.cpp
****/
#include <stdio.h>
#include <stdlib.h>

#include "ToolUtil.h"
#include "CodeGenUtil.h"
#include "ExePath.h"

using namespace foundation::library;

#ifdef _WIN32
#include <direct.h>
const std::string dirSeparator("\\");
#else
#include <sys/stat.h>
const std::string dirSeparator("/");
#endif

void MakeDirectory(const char* dir)
{
    auto subDirs = foundation::library::string_util::split(dir, dirSeparator[0]);
    std::string outputDirPath;
    for (auto iter = subDirs.begin();
    iter != subDirs.end();
        ++iter)
    {
        outputDirPath += *iter;
#ifdef _WIN32
        _mkdir(outputDirPath.c_str());
#else
        mkdir(outputDirPath.c_str(), 0755);
#endif
        outputDirPath += dirSeparator;
    }
}

static std::string QuoteIfPath(std::string path)
{
    if (path.length() == 0)
    {
        return std::string();
    }
    std::string quotePath = "\"";
    quotePath += path;
    if (path.back() == '\\')
    {
        quotePath += "\\";
    }
    quotePath += "\"";
    return quotePath;
}

int main(int argc, const char* argv[])
{
    printf("_pmodcodegentask->\n");
    for(int i = 1;i< argc; ++i)
    {
        printf("index:%d=%s\n",i, argv[i]);
    }
    printf("\n");
    try
    {
        _ToolParameterType parameters;
        CreateParameters(argc,argv,parameters);

        std::string codeGenDir = GetParameterValue(parameters, "CodeGenDir", ExePath().c_str()) + dirSeparator;

        std::string modelSchemaPath = GetParameterValue(parameters, "ModelSchema", nullptr);
        std::string productName = GetParameterValue(parameters, "ProductName", nullptr);

        std::string checkDependencies = GetParameterValue(parameters, "SchemaCheckDependencies", "true");
        std::string modelSchemaAttributes = GetParameterValue(parameters, "ModelSchemaAttributes", "");
        std::string outputDir = GetParameterValue(parameters, "OutputDir", nullptr);
        std::string configFile = GetParameterValue(parameters, "ConfigFile", (codeGenDir + "codegen.json").c_str());
        std::string javaPackageName = GetParameterValue(parameters, "JavaPackageName", "");

        if (javaPackageName.length())
        {
            std::string javaSourceDir = javaPackageName;
            string_util::find_and_replace(javaSourceDir, std::string("."), dirSeparator);
            std::string javaOutputDir = outputDir + dirSeparator + "java" + dirSeparator + productName + dirSeparator + javaSourceDir;
            MakeDirectory(javaOutputDir.c_str());

            std::string cmdDeleteJavaFiles;
#ifdef _WIN32
            cmdDeleteJavaFiles = "del ";
#else
            cmdDeleteJavaFiles = "exec rm -r";
#endif
            cmdDeleteJavaFiles += javaOutputDir + dirSeparator + "*.java";
            system(cmdDeleteJavaFiles.c_str());
        }
        std::string jsonSchemaOutput = outputDir + dirSeparator + productName + ".schema.json";

        std::string parserCmd;
#if defined(_WINDOWS)
        parserCmd += "\"";
#endif
#if !defined(_WINDOWS)
        parserCmd += "exec ";
#endif
        parserCmd += "\"";
        parserCmd += codeGenDir;
            
        parserCmd += "pmodschemaparser";
#if defined(_WINDOWS)
        parserCmd += ".exe";
#endif
        parserCmd += "\"";

        parserCmd += " -printParameters";
        parserCmd += " -pschema=" + QuoteIfPath(modelSchemaPath);
        parserCmd += " -pcheckdependencies=" + checkDependencies;
        parserCmd += " -pschema_attributes=" + modelSchemaAttributes;
        parserCmd += " -pgrammars=" + QuoteIfPath(codeGenDir);
        parserCmd += " -poutput=" + QuoteIfPath(jsonSchemaOutput);
        parserCmd += " -pgenerated_dir=" + QuoteIfPath(outputDir);

#if defined(_WINDOWS)
        parserCmd += "\"";
#endif
        MakeDirectory(outputDir.c_str());

        printf("run parserCmd:%s\n", parserCmd.c_str());
        auto result = system(parserCmd.c_str());

        if (result < 0 || result > 0x100)
        {
            throw std::string(foundation::library::string_util::format_string("pmodschemaparser returned: %d", result));
        }
        else if(result == 256 || result == 1)
        {
            // schema up to date
            return 0;
        }

        std::string codegenCmd;
#if defined(_WINDOWS)
        codegenCmd += "\"";
#endif
#if !defined(_WINDOWS)
        codegenCmd += "exec ";
#endif
        codegenCmd += "\"";
        codegenCmd += codeGenDir;

        codegenCmd += "pmodcodegen";
#if defined(_WINDOWS)
        codegenCmd += ".exe";
#endif
        codegenCmd += "\"";

        codegenCmd += " -printParameters";
        codegenCmd += " -pconfig_file=" + QuoteIfPath(configFile);
        codegenCmd += " -pmodelschema=" + QuoteIfPath(jsonSchemaOutput);
        codegenCmd += " -poutputdir=" + QuoteIfPath(outputDir + dirSeparator);
        codegenCmd += " -pproductname=" + productName;
        codegenCmd += " -pgenjava.run=";
        codegenCmd += javaPackageName.length() ? "true" : "false";
        codegenCmd += " -pjavapackagename=" + javaPackageName;
        codegenCmd += " -ptypeInfoFileReferences=" + GetParameterValue(parameters,"TypeInfoFileReferences","");
        codegenCmd += " -ptypeInterfaceAdapterReferences=" + GetParameterValue(parameters, "TypeInterfaceAdapterReferences", "");
        codegenCmd += " -pwinrtNamespacelookup=" + GetParameterValue(parameters, "WinrtNamespacelookup", "");

#if defined(_WINDOWS)
        replace(codegenCmd, "<", "^<");
        replace(codegenCmd, ">", "^>");
#else
        replace(codegenCmd, "<", "'<'");
        replace(codegenCmd, ">", "'>'");
#endif
#if defined(_WINDOWS)
        codegenCmd += "\"";
#endif

        printf("run codegenCmd:%s\n", codegenCmd.c_str());
        result = system(codegenCmd.c_str());
        if (result < 0)
        {
            throw std::string(foundation::library::string_util::format_string("pmodcodegen returned: %d", result));
        }
    }
    catch(std::string& error)
    {
        printf(error.data());
        return -1;
    }    
    return 0;
}
