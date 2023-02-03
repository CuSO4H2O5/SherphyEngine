#include "LogMessager.h"

namespace SherphyEngine(Miracle) {
	LogMessager* createLogMessager();
	void LogMessage(std::string message, WarningStage stage);
	static LogMessager* s_lognn = createLogMessager();
}