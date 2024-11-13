#include "core/logging/log_channel.h"

#include "core/logging/log_file.h"

using namespace rend::core::logging;

LogChannel::LogChannel(const std::string& channel_name)
    :
        _channel_name(channel_name)
{
}

const std::string& LogChannel::get_channel_name(void) const
{
    return _channel_name;
}

void LogChannel::bind_log_file(LogFile& log_file)
{
    _log_files.push_back(&log_file);
}

void LogChannel::write(const std::string& output)
{
    for(auto* log_file : _log_files)
    {
        log_file->write(output);
    }
}

