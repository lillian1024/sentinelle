#pragma once

#include <memory>
#include <yaml-cpp/node/node.h>
namespace middle_end
{
    /**
     * \brief This class represent a middle-end process that can be started.
     * \details The middle-ends are processes that allow communication and management of the software from external interfaces.
     */
    class MiddleEnd
    {
        public:
            MiddleEnd() = default;
            virtual ~MiddleEnd() = default;

            /**
             * \brief Create new middle-end from the provided configuration.
             * \details The method will select the correct class to create ht einstance depending on the configuration and call its constructor.
             */
            static std::unique_ptr<MiddleEnd> createNewMiddleEnd(std::string name, YAML::Node node);

            /**
             * \brief Attach to the running process.
             * \detail This method blocks until the middle-end stops.
             */
            virtual void AttachToRunning() = 0;

            /**
             * \brief Initialize the middle-end
             */
            virtual void Init() = 0;
            /**
             * \brief Starts the middle-end
             */
            virtual void Start() = 0;
            /**
             * \brief Request the middle-end to stop.
             * \details This method will not wait for the middle-end to stop.
             */
            virtual void Stop() = 0;
    };
}
