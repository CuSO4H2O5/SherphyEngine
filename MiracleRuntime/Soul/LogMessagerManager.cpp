#include "LogMessagerImpl.h"
#include "LogMessagerManager.h"

namespace SherphyEngine(Miracle)
{
    LogMessager* createLogMessager()
    {
        return new LogMessagerImpl;
    }

    SherphyNoReturn void LogMessage(std::string message, WarningStage stage)
    {
        s_lognn->logMessage(message, stage);
    }
}