/***
* Copyright (C) Microsoft. All rights reserved.
* Licensed under the MIT license. See LICENSE.txt file in the project root for full license information.
*
* File:JniLoadLibrary.cpp
****/
#include <jni.h>

#include <foundation/ctl/com_library.h>
#include <foundation/com_ptr.h>
#include <foundation/interfaces/logger.h>
#include <foundation/library/logger_library.h>
#include <foundation/library/logger_util.h>

#include <android/log.h>
using namespace foundation;

class CAndroidLoggerEventHandler : 
    public ctl::ComInspectableBase,
    public ILogCreatedEventHandler
{
protected:
    STDMETHOD(Invoke)(IUnknown* pSender, ILogCreatedEventArgs* pArgs)
    {
        ComPtr<ILogRecord> log_record_ptr;
        pArgs->GetLogRecord(log_record_ptr.GetAddressOf());
        LoggingLevel level;
        log_record_ptr->GetLevel(&level);

        HStringPtr text;
        logger_util::to_text(log_record_ptr,text.GetAddressOf());

        __android_log_write(ToAndroidPrio(level),"pmod_lib",text.GetRawBuffer());
        return S_OK;
    }
private:
    static int ToAndroidPrio(LoggingLevel level)
    {
        switch(level)
        {
        case LoggingLevel::Off:
            return ANDROID_LOG_DEFAULT;
        case LoggingLevel::Error:
            return ANDROID_LOG_ERROR;
        case LoggingLevel::Info:
            return ANDROID_LOG_INFO;
        case LoggingLevel::Debug:
            return ANDROID_LOG_DEBUG;
        case LoggingLevel::Warning:
            return ANDROID_LOG_WARN;
        case LoggingLevel::Fatal:
            return ANDROID_LOG_FATAL;
        case LoggingLevel::Trace:
            return ANDROID_LOG_VERBOSE;
        }
        return ANDROID_LOG_UNKNOWN;
    }
};
extern "C" {

    JNIEXPORT void JNICALL Java_com_microsoft_pmod_LoggerUtil_enableAndroidLog
    (JNIEnv *env, jclass)
    {
        // attach Android logger
        CAndroidLoggerEventHandler *pEventHandler = nullptr;
        ctl::ComInspectableObject<CAndroidLoggerEventHandler>::CreateInstance(&pEventHandler);
        LoggerListenerBase::AttachEventHandler(logger_util::GetFoundationLogger(), pEventHandler);
        ctl::inspectable::ReleaseInterface(pEventHandler);

        __android_log_write(ANDROID_LOG_DEBUG, "pmod_lib", "enableAndroidLog");
    }

JNIEXPORT jint JNICALL
	JNI_OnLoad(JavaVM* vm, void* reserved)
{
    __android_log_write(ANDROID_LOG_DEBUG, "pmod_lib", "JNI_OnLoad");
    return JNI_VERSION_1_6;
}

}
