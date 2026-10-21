namespace core
{
    namespace components
    {
        namespace analyzer
        {
            template<class T>
            T* Analyzer::getInputTypeFromInputs(std::map<std::string, utils::io_data::IOData*> inputs, std::string name)
            {
                if (inputs.find(name) == inputs.end())
                {
                    return nullptr;
                }

                T* res = dynamic_cast<T*>(inputs.find(name)->second);

                return res;
            }
        }
    }
}
