
namespace Sherphy
{ 
	class LogMessager;
	enum class WarningStage;
	LogMessager* createLogMessager();
	SherphyNoReturn void LogMessage(std::string message, WarningStage stage);
	static LogMessager* s_lognn = createLogMessager();
}