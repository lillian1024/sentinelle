#include "source-fail-event.hh"

#include "core/event/events/event.hh"

namespace core
{
    namespace event
    {
        namespace modules
        {
            SourceFailEvent::SourceFailEvent(components::source::Source& source)
                : Event("Source has failed")
                , source(source)
            {}

            components::source::Source& SourceFailEvent::getSource()
            {
                return source;
            }

            EventType SourceFailEvent::getType()
            {
                return EventType::SOURCE_FAIL;
            }
        } // namespace modules
    } // namespace event
} // namespace core
