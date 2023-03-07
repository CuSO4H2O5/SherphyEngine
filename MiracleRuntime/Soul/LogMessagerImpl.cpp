#include "LogMessager.h"
#include <iostream>
#include <format>

namespace Sherphy {
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
}