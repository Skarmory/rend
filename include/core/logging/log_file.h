#ifndef REND_CORE_LOGGING_LOG_FILE_H
#define REND_CORE_LOGGING_LOG_FILE_H

#include <iostream>
#include <fstream>
#include <string>

namespace rend::core::logging
{
    class LogFile
    {
        public: // funcs
            LogFile(const std::string& filepath);
            ~LogFile(void);

            const std::string& get_filepath(void) const;
            void write(const std::string& output);

        private: // vars
            std::string _filepath;
            std::ofstream _stream;
    };
}

#endif
