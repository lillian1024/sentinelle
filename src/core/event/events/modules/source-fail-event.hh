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
             * \brief An event triggered by a source stoppng.
             */
            class SourceFailEvent : public Event
            {
                public:
                    SourceFailEvent(components::source::Source& source);
                    ~SourceFailEvent() = default;

                    components::source::Source& getSource();

                    EventType getType();
                private:
                    components::source::Source& source;
            };
        }
    }
}
