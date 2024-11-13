#ifndef REND_CORE_LOGGING_LOG_MANAGER_H
#define REND_CORE_LOGGING_LOG_MANAGER_H

#include "core/logging/log_channel.h"
#include "core/logging/log_file.h"

#include <string>
#include <unordered_map>

namespace rend::core::logging
{
    class LogManager
    {
        public: // funcs
            static void initialise(void);
            static void uninitialise(void);
            static void write(const std::string& channel_name, const std::string& message);
            static LogManager& get_instance(void);

            void add_log_channel(const std::string& channel_name);
            void add_log_file(const std::string& file_path);
            bool bind_file_to_channel(const std::string& file_path, const std::string& channel_name);
            //void write(const std::string& channel_name, const std::string& message);

        private: // vars
            std::unordered_map<std::string, LogChannel> _name_to_log_channel_mapping;
            std::unordered_map<std::string, LogFile>    _name_to_log_file_mapping;

            static LogManager* _instance;
    };
}

#endif
