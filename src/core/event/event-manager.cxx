#include "event-manager.hh"
#include "utils/logger/logger.hh"
#include "utils/thread/thread-manager.hh"
#include <chrono>
#include <functional>
#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <thread>

//#define ON_PROCESSED_SLEEP_FOR_MILLIS 1
#define ON_EMPTY_SLEEP_FOR_MILLIS 5

namespace core
{
    namespace event
    {
        void EventManager::startEventManager()
        {
            if (!utils::thread::ThreadManager::instance().CanCreateNewThread())
            {
                throw std::runtime_error("[EventManager]: thread limit too low to start the event manager!");
            }

            utils::thread::ThreadManager::instance().CreateThread(false, startEventLoop, std::ref(*this));
        }

        void EventManager::stopEventManager()
        {
            stop = true;
        }

        void EventManager::registerEventHandler(EventHandler* handler)
        {
            std::lock_guard<std::mutex> lock(handler_list_lock);

            handler_list.push_back(handler);
        }
        void EventManager::removeEventHandler(EventHandler*)
        {
            std::lock_guard<std::mutex> lock(handler_list_lock);

            // TODO: finish this function
        }

        void EventManager::registerEvent(std::unique_ptr<Event> event)
        {
            std::lock_guard<std::mutex> lock(event_queue_lock);

            event_queue.push(std::move(event));
        }

        void EventManager::startEventLoop(EventManager& eventManager)
        {
            eventManager.eventHandlingLoop();
        }

        void EventManager::eventHandlingLoop()
        {
            while (!stop)
            {
                auto next_event = popNextEvent();

                if (next_event.has_value())
                {
                    std::lock_guard<std::mutex> lock(handler_list_lock);

                    std::ostringstream sb;

                    sb << "Handling event of type: ";
                    sb << next_event.value()->getType();
                    sb << " with ";
                    sb << handler_list.size();
                    sb << " handlers";

                    utils::logger::Logger::instance().Log("EventManager", sb.str(), utils::logger::Logger::LogLevel::DEBUG);

                    for (auto* handler: handler_list)
                    {
                        if (handler == nullptr)
                        {
                            continue;
                        }

                        handler->handleEvent(*next_event.value().get());
                    }

                    if (handler_list.empty())
                    {
                        std::this_thread::sleep_for(std::chrono::milliseconds(ON_EMPTY_SLEEP_FOR_MILLIS));
                    }
                }
                else
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(ON_EMPTY_SLEEP_FOR_MILLIS));
                }
            }
        }

        std::optional<std::unique_ptr<Event>> EventManager::popNextEvent()
        {
            std::lock_guard<std::mutex> lock(event_queue_lock);

            if (event_queue.empty())
            {
                return std::nullopt;
            }

            std::unique_ptr<Event> front = std::move(event_queue.front());

            event_queue.pop();

            return front;
        }
    }
}
