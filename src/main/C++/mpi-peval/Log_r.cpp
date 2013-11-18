#include <log4cpp/PropertyConfigurator.hh>
#include <iostream>
#ifndef WIN32
#include <unistd.h>
#else
#include <process.h>
#endif
#include <stdio.h>

#include "Log_r.h"
#include "log4cpp/OstreamAppender.hh"

using namespace log4cpp;
using namespace std;

Category* Log_r::m_root = NULL;
string* Log_r::m_progName = NULL;
string* Log_r::m_modName = NULL;
threading::Mutex* Log_r::m_mutex = NULL;

int Log_r::isEnabledDebug = 0;    
int Log_r::isEnabledInfo = 0;    
int Log_r::isEnabledNotice = 0;    
int Log_r::isEnabledWarn = 0;    
int Log_r::isEnabledError = 0;    
int Log_r::isEnabledCrit = 0;    
int Log_r::isEnabledFatal = 0;    

uint32_t Log_r::m_pid = 0;

void Log_r::SetProgName(const std::string& name)
{
    *(Log_r::m_progName)=name;
}

void Log_r::SetModName(const std::string& name)
{
    *(Log_r::m_modName)=name;
}

int Log_r::Init(const char* configFile)
{
    try    {
        string confFileName = configFile;
        PropertyConfigurator::configure(confFileName);
    }
    catch(ConfigureFailure& f)    {
        cerr << "Log4Cpp Configure Problem !\n";
        return -1;
    }

    Log_r::m_root=&(Category::getRoot());

    if(Log_r::m_mutex == NULL)
    {
        Log_r::m_mutex = new log4cpp::threading::Mutex();
    }
    if(Log_r::m_progName == NULL)
    {
        Log_r::m_progName = new std::string;
    }
    if(Log_r::m_modName == NULL)
    {
        Log_r::m_modName = new std::string;
    }

    Log_r::isEnabledDebug = 0;    
    if (Log_r::m_root->isPriorityEnabled(Priority::DEBUG)) {
        Log_r::isEnabledDebug = 1;
    } 

    Log_r::isEnabledInfo = 0;    
    if (Log_r::m_root->isPriorityEnabled(Priority::INFO)) {
        Log_r::isEnabledInfo = 1;
    } 

    Log_r::isEnabledNotice = 0;    
    if (Log_r::m_root->isPriorityEnabled(Priority::NOTICE)) {
        Log_r::isEnabledNotice = 1;
    } 

    Log_r::isEnabledWarn = 0;    
    if (Log_r::m_root->isPriorityEnabled(Priority::WARN)) {
        Log_r::isEnabledWarn = 1;
    } 

    Log_r::isEnabledError = 0;    
    if (Log_r::m_root->isPriorityEnabled(Priority::ERROR)) {
        Log_r::isEnabledError = 1;
    } 

    Log_r::isEnabledCrit = 0;    
    if (Log_r::m_root->isPriorityEnabled(Priority::CRIT)) {
        Log_r::isEnabledCrit = 1;
    } 

    Log_r::isEnabledFatal = 0;    
    if (Log_r::m_root->isPriorityEnabled(Priority::FATAL)) {
        Log_r::isEnabledFatal = 1;
    } 
    Log_r::m_pid = getpid();

    return 0;
}

void Log_r::Shutdown()
{
    log4cpp::Category::shutdown(); 
    if(m_mutex != NULL)
    {
        delete m_mutex; 
    }
    if(m_progName != NULL)
    {
        delete m_progName;
    }
    if(m_modName != NULL)
    {
        delete m_modName;
    }
}

void Log_r::Warn(const char* format, ...)
{
    {
        threading::ScopedLock lock(*m_mutex);
        va_list ap;
        va_start(ap,format);
        Log_r::m_root->logva(Priority::WARN,(format),ap);
        va_end(ap);
    }
}

void Log_r::Error(const char* format, ...)
{
    {
        threading::ScopedLock lock(*m_mutex);
        va_list ap;
        va_start(ap,format);
        Log_r::m_root->logva(Priority::ERROR,(format),ap);
        va_end(ap);
    }
}

void Log_r::Info(const char* format, ...)
{
    {
        threading::ScopedLock lock(*m_mutex);
        va_list ap;
        va_start(ap,format);
        Log_r::m_root->logva(Priority::INFO,(format),ap);
        va_end(ap);
    }
}

void Log_r::Notice(const char* format, ...)
{
    {
        threading::ScopedLock lock(*m_mutex);
        va_list ap;
        va_start(ap,format);
        Log_r::m_root->logva(Priority::NOTICE,(format),ap);
        va_end(ap);
    }
}
void Log_r::Debug(const char* format, ...)
{
    {
        threading::ScopedLock lock(*m_mutex);
        va_list ap;
        va_start(ap,format);
        Log_r::m_root->logva(Priority::DEBUG,(format),ap);
        va_end(ap);
    }
}
void Log_r::Crit(const char* format, ...)
{
    {
        threading::ScopedLock lock(*m_mutex);
        va_list ap;
        va_start(ap,format);
        Log_r::m_root->logva(Priority::CRIT,(format),ap);
        va_end(ap);
    }
}

void Log_r::Fatal(const char* format, ...)
{
    {
        threading::ScopedLock lock(*m_mutex);
        va_list ap;
        va_start(ap,format);
        Log_r::m_root->logva(Priority::FATAL,(format),ap);
        va_end(ap);
    }

}

