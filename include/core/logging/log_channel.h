#ifndef REND_CORE_LOGGING_LOG_CHANNEL_H
#define REND_CORE_LOGGING_LOG_CHANNEL_H

#include <string>
#include <vector>

namespace rend::core::logging
{
    class LogFile;

    class LogChannel
    {
        public: //funcs
            LogChannel(const std::string& channel_name);
            ~LogChannel(void) = default;

            const std::string& get_channel_name(void) const;
            void bind_log_file(LogFile& log_file);
            void write(const std::string& output);

        private: //vars
            std::string _channel_name;
            std::vector<LogFile*> _log_files;
    };
}

#endif
