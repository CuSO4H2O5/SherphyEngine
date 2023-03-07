#include <Soul/PreCompile/predefinedMacro.h>

#include <string>

namespace Sherphy
{ 
	class LogMessager;
	enum class WarningStage;
	SherphyNoReturn void LogMessage(std::string message, WarningStage stage);
}