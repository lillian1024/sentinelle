#include "middle-end-settings.hh"
#include "middle_end/middle_end.hh"

#include <functional>
#include <sstream>
#include <stdexcept>

namespace utils
{
    namespace config
    {
        namespace data
        {
            namespace middle_end
            {
                void MiddleEndSettings::readModule(YAML::Node node)
                {
                    if (!node.IsMap())
                    {
                        throw std::runtime_error("[MiddleEndSettings]: middle_end field should be a map!");
                    }

                    for (auto middle: node)
                    {
                        if (!middle.first.IsScalar())
                        {
                            throw std::runtime_error("[MiddleEndSettings]: elements of the field middle_end should have string names!");
                        }

                        auto name = middle.first.Scalar();

                        middle_ends.push_back(::middle_end::MiddleEnd::createNewMiddleEnd(name, middle.second));
                    }
                }

                std::vector<std::reference_wrapper<::middle_end::MiddleEnd>> MiddleEndSettings::getMiddleEnds()
                {
                    std::vector<std::reference_wrapper<::middle_end::MiddleEnd>> res;

                    for (size_t i = 0; i < middle_ends.size(); i++)
                    {
                        res.push_back(*middle_ends[i].get());
                    }

                    return res;
                }

                std::string MiddleEndSettings::dumpInfo()
                {
                    std::ostringstream sb;



                    return sb.str();
                }
            }
        }
    }
}
