
#ifndef BILLLOG_R_HPP
#define BILLLOG_R_HPP

#include <pthread.h>

#include <string>
#include <log4cpp/Category.hh>
#include <log4cpp/CategoryStream.hh>
#include <log4cpp/Priority.hh>
#include <log4cpp/threading/PThreads.hh>

using namespace log4cpp;

#define Log_r_Debug(_fmt_, args...) \
    { if (Log_r::isEnabledDebug){ \
    Log_r::Debug(" [%s:%s:%d][%u:%lu] " _fmt_, __FILE__,__FUNCTION__, __LINE__, Log_r::m_pid, pthread_self(), ##args); }}

#define Log_r_Info(_fmt_, args...) \
    { if (Log_r::isEnabledInfo) { \
    Log_r::Info(" [%s:%s:%d][%u:%lu] " _fmt_, __FILE__, __FUNCTION__, __LINE__, Log_r::m_pid,  pthread_self(), ##args); } }

#define Log_r_Notice(_fmt_, args...) \
    { if (Log_r::isEnabledNotice) { \
    Log_r::Notice(" [%s:%s:%d][%u:%lu] " _fmt_, __FILE__, __FUNCTION__, __LINE__, Log_r::m_pid,  pthread_self(), ##args); }}

#define Log_r_Warn(_fmt_, args...) \
    { if (Log_r::isEnabledWarn) {\
    Log_r::Warn(" [%s:%s:%d][%u:%lu] " _fmt_,  __FILE__, __FUNCTION__, __LINE__, Log_r::m_pid,  pthread_self(), ##args); }}

#define Log_r_Error(_fmt_, args...) \
    { if (Log_r::isEnabledError) { \
    Log_r::Error(" [%s:%s:%d][%u:%lu] " _fmt_, __FILE__, __FUNCTION__, __LINE__, Log_r::m_pid,  pthread_self(), ##args); }}

#define Log_r_Crit(_fmt_, args...) \
    { if (Log_r::isEnabledCrit) { \
    Log_r::Crit(" [%s:%s:%d][%u:%lu] " _fmt_, __FILE__, __FUNCTION__, __LINE__, Log_r::m_pid,  pthread_self(), ##args); }}

#define Log_r_Fatal(_fmt_, args...) \
    { if (Log_r::isEnabledFatal) {\
    Log_r::Fatal(" [%s:%s:%d][%u:%lu] " _fmt_, __FILE__, __FUNCTION__, __LINE__, Log_r::m_pid,  pthread_self(), ##args); }}


class Log_r
{
public:
	static int Init(const char* configFile);

    static void Debug(const char* format, ...) __attribute__((format(printf, 1, 2)));
	static void Info(const char* format, ...)  __attribute__((format(printf, 1, 2)));
	static void Notice(const char* format, ...)__attribute__((format(printf, 1, 2)));
	static void Warn(const char* format, ...)  __attribute__((format(printf, 1, 2)));
	static void Error(const char* format, ...) __attribute__((format(printf, 1, 2)));
	static void Crit(const char* format, ...)  __attribute__((format(printf, 1, 2)));
	static void Fatal(const char* format, ...) __attribute__((format(printf, 1, 2)));

    static void SetProgName(const std::string& name);
    static void SetModName(const std::string& name);
    static void Shutdown();

    static int isEnabledDebug;    
    static int isEnabledInfo;    
    static int isEnabledNotice;    
    static int isEnabledWarn;    
    static int isEnabledError;    
    static int isEnabledCrit;    
    static int isEnabledFatal;    
    
    static uint32_t m_pid;    

    static log4cpp::Category* m_root;
    static log4cpp::threading::Mutex* m_mutex;
    static std::string* m_progName;
    static std::string* m_modName;
    static std::string FormatMessage(const std::string& msg);
};		

#endif
