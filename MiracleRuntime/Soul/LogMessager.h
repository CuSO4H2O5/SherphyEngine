#include <string>

namespace SherphyEngine(Miracle){
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
            LogMessager();
            ~LogMessager();
            virtual bool logMessage(std::string message, WarningStage stage);

            virtual bool logPreviousMessage();
            virtual bool logPreviousFatalMessage();
        private:
    };
}

