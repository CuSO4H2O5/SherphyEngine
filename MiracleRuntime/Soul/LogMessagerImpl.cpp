#include "LogMessagerImpl.h"
#include <iostream>
#include <format>

namespace Sherphy {
    using namespace std;
    bool LogMessagerImpl::logMessage(std::string message, WarningStage stage)
    {
        // m_message_box.push_back();
        cout << message << endl;
        return true;
    }

    bool LogMessagerImpl::logPreviousMessage()
    {
        return true;
    }

    bool LogMessagerImpl::logPreviousFatalMessage()
    {
        return true;
    }

    LogMessagerImpl::LogMessagerImpl()
    {
    }

    LogMessagerImpl::~LogMessagerImpl()
    {
    }
}