
#include <string>
#include <functional>
#include <vector>
#include "respparser.h"
#include <mutex>
#include <thread>
struct commandinfo{
    std::string name;
    std::function<RESPValue(const RESPArray&)> handler;
    size_t minArgs = 0;
    std::string description;
};
class commandprocessor{
    public:
    commandprocessor(std::shared_ptr<std::unordered_map<std::string, RESPValue>> store,std::mutex* m){
        for (auto& cmd : commandTable){
            commandmap[cmd.name] = cmd;
        }
        this->store = store;
        this->store_mutex = m;
    }
    std::string help(){
        std::string output;
        for (const auto& cmd : commandTable) {
            output += cmd.name + " - " + cmd.description + "\n";
        }
        return output;
    }
    RESPValue process(const RESPValue& input) {
        const std::shared_ptr<RESPArray>* arrPtr = std::get_if<std::shared_ptr<RESPArray>>(&input);
        if (!arrPtr || !(*arrPtr) || (*arrPtr)->values.empty()) {
            return RESPError{"Invalid command format"};
        }
    
        const RESPArray& arr = *(*arrPtr);

        const RESPString* cmdStr = std::get_if<RESPString>(&arr.values[0]);
        if (!cmdStr) {
            return RESPError{"Command must be a string"};
        }
    
        auto it = commandmap.find(cmdStr->value);
        if (it == commandmap.end()) {
            return RESPError{"Unknown command: " + cmdStr->value};
        }
    
        const commandinfo& info = it->second;
    
        if (arr.values.size() - 1 < info.minArgs) {
            return RESPError{"Wrong number of arguments for command " + info.name};
        }
    
        return info.handler(arr);
    }
    void registerCommand(const commandinfo& info) {
        commandTable.push_back(info);
        commandmap[info.name] = info;
    }
    private:
        std::shared_ptr<std::unordered_map<std::string, RESPValue>> store;
        std::mutex* store_mutex;
        std::vector<commandinfo> commandTable = {
            {
                "PING",
                [](const RESPArray&) {
                    return RESPString{"PONG", RESPStringType::SIMPLE};
                },
                0,
                "Returns PONG"
            },
            {
                "ECHO",
                [](const RESPArray& arr) -> RESPValue {
                    if (arr.values.size() < 2) {
                        return RESPValue{RESPError{"ECHO requires an argument"}};
                    }
                    const RESPString* arg = std::get_if<RESPString>(&arr.values[1]);
                    if (!arg) return RESPValue{RESPError{"Invalid argument"}};
                
                    return RESPValue{RESPString{arg->value, RESPStringType::BULK}};
                },
                1,
                "Echoes a string"
            },
            {
                "SET",
                [this](const RESPArray& arr) -> RESPValue {
                    if (arr.values.size() < 3) {
                        return RESPError{"SET requires 2 arguments"};
                    }
                    const RESPString* key = std::get_if<RESPString>(&arr.values[1]);
                    if (!key) return RESPError{"Invalid key (must be string)"};
            
                    const RESPString* value = std::get_if<RESPString>(&arr.values[2]);
                    if (!value) return RESPError{"Invalid value (must be string)"};                    
                    {
                        std::lock_guard<std::mutex> lock(*store_mutex);
                        (*store)[key->value] = RESPString{value->value, RESPStringType::BULK};
                    }
                    return RESPString{"OK", RESPStringType::SIMPLE};
                },
                2,
                "Sets the value for a key"
            },
            {
                "GET",
                [this](const RESPArray& arr) -> RESPValue{
                    if (arr.values.size() < 2){
                        return RESPError{"GET requires 1 arguments"};
                    }
                    const RESPString* key = std::get_if<RESPString>(&arr.values[1]);
                    if (!key) return RESPError{"Invalid key (must be string)"};

                    {
                        std::lock_guard<std::mutex> lock(*store_mutex);
                    
                        auto it = store->find(key->value);
                        if (it == store->end()) {
                            return RESPError{"Key not found"};
                        }
                    
                        const RESPString* value = std::get_if<RESPString>(&(it->second));
                        if (!value) {
                            return RESPError{"Stored value is not a string"};
                        }
                    
                        return *value;
                    }
                },
                1,
                "Gets the value from a key"
            }
            };
        std::unordered_map<std::string,commandinfo> commandmap;
};
