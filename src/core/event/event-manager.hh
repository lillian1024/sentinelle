#pragma once

#include "core/event/event-handler.hh"
#include "core/event/events/event.hh"
#include "utils/singleton/singleton.hh"

#include <atomic>
#include <memory>
#include <mutex>
#include <optional>
#include <queue>
#include <vector>

namespace core
{
    namespace event
    {
        class EventManager : public utils::Singleton<EventManager>
        {
            public:
                void startEventManager();
                void stopEventManager();

                void registerEventHandler(EventHandler* handler);
                void removeEventHandler(EventHandler* handler);

                void registerEvent(std::unique_ptr<Event> event);
            private:
                static void startEventLoop(EventManager& eventManager);

                void eventHandlingLoop();

                std::optional<std::unique_ptr<Event>> popNextEvent();

                std::atomic_bool stop;

                std::queue<std::unique_ptr<Event>> event_queue;
                std::mutex event_queue_lock;

                std::vector<EventHandler*> handler_list;
                std::mutex handler_list_lock;
        };
    }
}
