#include "middle_end.hh"

#include "components/mqtt/middle_end_mqtt.hh"
#include <memory>
#include <sstream>
#include <stdexcept>

namespace middle_end
{
    std::unique_ptr<MiddleEnd> MiddleEnd::createNewMiddleEnd(std::string name, YAML::Node node)
    {
        if (name == mqtt::MiddleEndMQTT::middle_end_mqtt_name)
        {
            return std::make_unique<mqtt::MiddleEndMQTT>(node);
        }
        else
        {
            std::ostringstream sb;

            sb << "[MiddleEnd]: Unkown middle name: ";
            sb << name;
            sb << " !";

            throw std::runtime_error(sb.str());
        }
    }
}
