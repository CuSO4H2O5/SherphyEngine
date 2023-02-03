#include "LogMessager.h"
#include "RingBuffer.h"

namespace SherphyEngine(Miracle)
{
    using namespace std;
    const int message_box_size = 16;
    class LogMessagerImpl : public LogMessager
    {
        public:
            LogMessagerImpl();
            ~LogMessagerImpl();
            virtual bool logMessage(std::string message, WarningStage stage) override final;
            virtual bool logPreviousMessage() override final;
        private:
            RingBufferString m_message_box[message_box_size];
    };
} // namespace Miracle
