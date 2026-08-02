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
            UNTRIGGERED
        };

        class Event
        {
            public:
                Event(std::string message);
                virtual ~Event() = default;

                virtual EventType getType() = 0;

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
            }

            return os;
        }
    }
}
