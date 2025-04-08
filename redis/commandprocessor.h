#include <string>
#include <functional>
#include <vector>
#include "respparser.h"
#include <mutex>
#include <thread>
#include <chrono>
#include <optional>
struct commandinfo{
    std::string name;
    std::function<RESPValue(const RESPArray&)> handler;
    size_t minArgs = 0;
    std::string description;
};

struct Entry {
    RESPValue value;
    std::optional<std::chrono::system_clock::time_point> expiration;
};

class commandprocessor{
    public:
    commandprocessor(std::shared_ptr<std::unordered_map<std::string, Entry>> store,std::mutex* storemutex){
        for (auto& cmd : commandTable){
            commandmap[cmd.name] = cmd;
        }
        this->store = store;
        this->store_mutex = storemutex;
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
        std::shared_ptr<std::unordered_map<std::string, Entry>> store;
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
                        return RESPError{"SET requires at least 2 arguments"};
                    }
                    if (arr.values.size() > 5) {
                        return RESPError{"SET takes at most 4 arguments"};
                    }
            
                    const RESPString* key = std::get_if<RESPString>(&arr.values[1]);
                    if (!key) return RESPError{"Key must be a string"};
            
                    const RESPValue& value = arr.values[2];  
            
                    std::optional<std::chrono::system_clock::time_point> expiration;
            
                    if (arr.values.size() == 5) {
                        const RESPString* time_type = std::get_if<RESPString>(&arr.values[3]);
                        const RESPString* durationStr = std::get_if<RESPString>(&arr.values[4]);
                        if (!time_type || !durationStr) {
                            return RESPError{"Invalid expiration arguments"};
                        }
                        int duration = std::stoi(durationStr->value);

                        if (!time_type || !duration) {
                            return RESPError{"Invalid expiration arguments"};
                        }
            
                        auto now = std::chrono::system_clock::now();
            
                        if (time_type->value == "EX") {
                            expiration = now + std::chrono::seconds(duration);
                        } else if (time_type->value == "PX") {
                            expiration = now + std::chrono::milliseconds(duration);
                        } else if (time_type->value == "EXAT") {
                            expiration = std::chrono::system_clock::time_point{std::chrono::seconds(duration)};
                        } else if (time_type->value == "PXAT") {
                            expiration = std::chrono::system_clock::time_point{std::chrono::milliseconds(duration)};
                        } else {
                            return RESPError{"Unsupported expiration type"};
                        }
                    }
            
                    {
                        std::lock_guard<std::mutex> lock(*store_mutex);
                        (*store)[key->value] = Entry{value, expiration};
                    }
            
                    return RESPString{"OK", RESPStringType::SIMPLE};
                },
                2,
                "Sets the value for a key"
            },
            {
                "GET",
                [this](const RESPArray& arr) -> RESPValue {
                    if (arr.values.size() < 2) {
                        return RESPError{"GET requires 1 argument"};
                    }
            
                    const RESPString* key = std::get_if<RESPString>(&arr.values[1]);
                    if (!key) return RESPError{"Key must be a string"};
            
                    std::lock_guard<std::mutex> lock(*store_mutex);
            
                    auto it = store->find(key->value);
                    if (it == store->end()) {
                        return RESPError{"Key not found"};
                    }
            
                    const Entry& entry = it->second;

                    if (entry.expiration.has_value() && std::chrono::system_clock::now() > entry.expiration.value()) {
                        store->erase(it);
                        return RESPError{"Key expired"};
                    }
            
                    return entry.value;
                },
                1,
                "Gets the value for a key"
            },

            };
        std::unordered_map<std::string,commandinfo> commandmap;
};
