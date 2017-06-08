/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:pmodcodegentask.cpp
****/
#include "ExePath.h"
#include "ToolUtil.h"
#include <stdlib.h>
#include <foundation/string_t.h>


#ifdef _WIN32
#include <Windows.h>

std::string ExePath() {
	CHAR_t buffer[MAX_PATH];
    GetModuleFileName(NULL, buffer, MAX_PATH);

	std::string bufferStr;
#ifdef __UTF16_STRINGS
	bufferStr = foundation::to_utf8_string(buffer);
#else
	bufferStr = std::string(buffer);
#endif

    std::string::size_type pos = bufferStr.find_last_of("\\/");
    return bufferStr.substr(0, pos);
}
#else
#include <sys/stat.h>
#include <mach-o/dyld.h>
#include <mach-o/getsect.h>
#include <dlfcn.h>

std::string GetModuleFileName(const void *module)
{
    if (!module) { return std::string(); }
    uint32_t count = _dyld_image_count();
    for (uint32_t i = 0; i < count; ++i) {
        const mach_header *header = _dyld_get_image_header(i);
        if (!header) { break; }
        char *code_ptr = NULL;
        if ((header->magic & MH_MAGIC_64) == MH_MAGIC_64) {
            uint64_t size;
            code_ptr = getsectdatafromheader_64((const mach_header_64 *)header, SEG_TEXT, SECT_TEXT, &size);
        } else {
            uint32_t size;
            code_ptr = getsectdatafromheader(header, SEG_TEXT, SECT_TEXT, &size);
        }
        if (!code_ptr) { continue; }
        const uintptr_t slide = _dyld_get_image_vmaddr_slide(i);
        const uintptr_t start = (const uintptr_t)code_ptr + slide;
        Dl_info info;
        if (dladdr((const void *)start, &info)) {
//            if (dlopen(info.dli_fname, RTLD_NOW) == module) {
            if(info.dli_saddr == module) {
                return std::string(info.dli_fname);
            }
        }
    }
    return std::string();
}

std::string ExePath() {
    auto exePath = GetModuleFileName((void *)&CreateParameters);
    std::string::size_type pos = exePath.find_last_of("/");
    return exePath.substr(0, pos);
}

#endif
