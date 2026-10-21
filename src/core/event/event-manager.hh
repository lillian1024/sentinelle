#pragma once

#include "core/event/event-handler.hh"
#include "core/event/events/event.hh"
#include "utils/singleton/singleton.hh"

#include <atomic>
#include <memory>
#include <mutex>
#include <optional>
#include <queue>
#include <thread>
#include <vector>

namespace core
{
    namespace event
    {
        /**
         * \brief This class is reponsible to manage Event.
         * \details This class can be used to register new events. Those events will be sent to every registered EventHandler.
         */
        class EventManager : public utils::Singleton<EventManager>
        {
            public:
                /**
                 * \brief Start EventManager thread. This method is thread-safe.
                 */
                void startEventManager();
                /**
                 * \brief Request EventManager to stop. This method is thread-safe.
                 * \details This function will not wait for the EventManager to stop.
                 */
                void stopEventManager();

                /**
                 * \brief Register a new EventHandler that will receive the events. This method is thread-safe.
                 */
                void registerEventHandler(EventHandler* handler);
                /**
                 * \brief WIP: Unregister a previously registered EventManager. This method is thread-safe.
                 */
                void removeEventHandler(EventHandler* handler);

                /**
                 * \brief Register a new event to be processed by the registered EventManagers
                 */
                void registerEvent(std::unique_ptr<Event> event);

                /**
                 * \brief Attach to the running EventManager thread
                 * \details This method blocks until the EventManager stops.
                 */
                void AttachToRunning();
            private:
                static void startEventLoop(EventManager& eventManager);

                void eventHandlingLoop();

                std::optional<std::unique_ptr<Event>> popNextEvent();

                std::atomic_bool stop;

                std::queue<std::unique_ptr<Event>> event_queue;
                std::mutex event_queue_lock;

                std::vector<EventHandler*> handler_list;
                std::mutex handler_list_lock;

                std::thread event_manager_thread;
        };
    }
}
