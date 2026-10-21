#pragma once

#include "core/components/sources/source.hh"
#include "core/event/events/event.hh"

namespace core
{
    namespace event
    {
        namespace modules
        {
            /**
             * \brief An event triggered by a source starting.
             */
            class SourceStartEvent : public Event
            {
                public:
                    SourceStartEvent(components::source::Source& source);
                    ~SourceStartEvent() = default;

                    components::source::Source& getSource();

                    EventType getType();
                private:
                    components::source::Source& source;
            };
        }
    }
}
