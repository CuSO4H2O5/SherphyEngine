#include "LogMessager.h"
#include "LogMessagerManager.h"

namespace Sherphy {
    static LogMessager* s_lognn;

    SherphyNoReturn void LogMessage(std::string message, WarningStage stage)
    {
        s_lognn->logMessage(message, stage);
    }
}