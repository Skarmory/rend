#include "core/logging/log_manager.h"

using namespace rend::core::logging;

LogManager* LogManager::_instance = nullptr;

void LogManager::initialise(void)
{
    if (_instance == nullptr)
    {
        _instance = new LogManager;
    }
}

void LogManager::uninitialise(void)
{
    if (_instance != nullptr)
    {
        delete _instance;
        _instance = nullptr;
    }
}

LogManager& LogManager::get_instance(void)
{
    return *_instance;
}

void LogManager::add_log_channel(const std::string& channel_name)
{
    if(!_name_to_log_channel_mapping.contains(channel_name))
    {
        _name_to_log_channel_mapping.emplace(channel_name, channel_name);
    }
}

void LogManager::add_log_file(const std::string& file_path)
{
    if(!_name_to_log_file_mapping.contains(file_path))
    {
        auto it  = _name_to_log_file_mapping.emplace(file_path, file_path);
        it.first->second.write("Log file initialised");
    }
}

bool LogManager::bind_file_to_channel(const std::string& file_path, const std::string& channel_name)
{
    auto file_it = _name_to_log_file_mapping.find(file_path);
    if(file_it == _name_to_log_file_mapping.end())
    {
        return false;
    }

    auto channel_it = _name_to_log_channel_mapping.find(channel_name);
    if(channel_it == _name_to_log_channel_mapping.end())
    {
        return false;
    }

    channel_it->second.bind_log_file(file_it->second);
    channel_it->second.write("Log file " + file_path + " bound to channel " + channel_name);
    return true;
}

void LogManager::write(const std::string& channel_name, const std::string& message)
{
    if(auto channel_it = _instance->_name_to_log_channel_mapping.find(channel_name); channel_it != _instance->_name_to_log_channel_mapping.end())
    {
        channel_it->second.write(message);
    }
}
