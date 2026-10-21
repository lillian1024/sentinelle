#pragma once

#include "core/event/events/event.hh"

namespace core
{
    namespace event
    {
        class EventHandler
        {
            public:
                /**
                 * \brief A function called when an event is registered.
                 * \detail This function is called on the EventManager thread.
                 * \param event The regstered event
                 */
                virtual void handleEvent(Event& event) = 0;
        };
    }
}
