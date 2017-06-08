/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:Md5Util.cpp
****/

#include "md5Util.h"
#include "StringUtils.h"

extern "C"
{
    #include "../md5/md5.h"
}

IID CreateIIDTypeFromString(const wchar_t *pString)
{
    std::string utf8str =ToUtf8String(pString);

    MD5_CTX context;
    unsigned char digest[16];

    MD5Init (&context);
    MD5Update (&context, (unsigned char *)utf8str.data(), (unsigned int)utf8str.size());
    MD5Final (digest, &context);

    IID iid;
    memcpy(&iid,&digest,sizeof(IID));

    return iid;
}
