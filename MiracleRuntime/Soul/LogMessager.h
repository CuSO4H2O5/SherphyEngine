#include <string>

namespace Sherphy {
    enum class WarningStage
    {
        Normal, // just debug message
        Low,    // a normal warning
        Medium, // a warning may cause other system error
        High,   // a error but not affact the other system
        Fatal   // fatal error that may affact other system
    };

    class LogMessager
    {
        public:
            LogMessager() {};
            virtual ~LogMessager() = 0;
            virtual bool logMessage(std::string message, WarningStage stage) = 0;

            virtual bool logPreviousMessage() = 0;
            virtual bool logPreviousFatalMessage() = 0;
        private:
    };
}