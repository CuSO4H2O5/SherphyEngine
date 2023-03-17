#include "LogMessager.h"
#include <iostream>
//#include <format>

namespace Sherphy {
    static LogMessager s_lognn;
    bool LogMessager::logMessage(std::string message, WarningStage stage)
    {
        // m_message_box.push_back();
        std::cout << message << std::endl;
        return true;
    }

    bool LogMessager::logPreviousMessage()
    {
        return true;
    }

    bool LogMessager::logPreviousFatalMessage()
    {
        return true;
    }

    LogMessager::~LogMessager()
    {
    }

    LogMessager* GetLogMessagerInstance()
    {
        return &s_lognn;
    }
    SherphyNoReturn void LogMessage(std::string message, WarningStage stage)
    {
        GetLogMessagerInstance()->logMessage(message, stage);
    }
}