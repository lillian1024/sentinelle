#pragma once

#include <ostream>
#include <string>

namespace core
{
    namespace event
    {
        enum class EventType
        {
            DETECTED,
            TRIGGERED,
            UNTRIGGERED,
            SOURCE_STOP,
            SOURCE_START
        };

        /**
         * \brief This class describes an event that can be triggered using the EventManager.
         */
        class Event
        {
            public:
                /**
                 * \brief Create a new event with the provided message.
                 */
                Event(std::string message);
                virtual ~Event() = default;

                /**
                 * \brief Get the type of the event.
                 */
                virtual EventType getType() = 0;

                /**
                 * \brief Get the message of the event.
                 */
                std::string getMessage();
            private:
                std::string message;
        };

        inline std::ostream& operator<<(std::ostream& os, const EventType& obj)
        {
            switch (obj)
            {
                case EventType::DETECTED:
                    os << "DETECTED";
                    break;
                case EventType::TRIGGERED:
                    os << "TRIGGERED";
                    break;
                case EventType::UNTRIGGERED:
                    os << "UNTRIGGERED";
                    break;
                case EventType::SOURCE_STOP:
                    os << "SOURCE_STOP";
                    break;
                case EventType::SOURCE_START:
                    os << "SOURCE_START";
                    break;
                }

            return os;
        }
    }
}
