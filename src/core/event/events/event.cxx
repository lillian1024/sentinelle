#include "event.hh"

namespace core
{
    namespace event
    {
        Event::Event(std::string message)
            : message(message)
        {

        }

        std::string Event::getMessage()
        {
            return message;
        }
    }
}
