#include <string>
#include "RingBuffer.h"

namespace Sherphy {
    const size_t message_box_size = 16;

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
            virtual ~LogMessager();
            virtual bool logMessage(std::string message, WarningStage stage);

            virtual bool logPreviousMessage();
            virtual bool logPreviousFatalMessage();
        private:
            RingBufferString m_message_box[message_box_size];
    };
}