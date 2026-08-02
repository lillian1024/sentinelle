#pragma once

#include "core/event/events/event.hh"

namespace core
{
    namespace event
    {
        class EventHandler
        {
            public:
                virtual void handleEvent(Event& event) = 0;
        };
    }
}
