#include "core/logging/log_file.h"

using namespace rend::core::logging;

LogFile::LogFile(const std::string& filepath)
    :
        _filepath(filepath)
{
    _stream.open(_filepath, std::ios::out | std::ios::trunc);
}

LogFile::~LogFile(void)
{
    _stream.flush();
    _stream.close();
}

const std::string& LogFile::get_filepath(void) const
{
    return _filepath;
}

void LogFile::write(const std::string& output)
{
    _stream << output << std::endl;
    _stream.flush();
}
