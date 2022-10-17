#ifndef CONSOLE_MED_LOGGER_IMPLEMENTATION_CLASS
#define CONSOLE_MED_LOGGER_IMPLEMENTATION_CLASS

#include "med_logger_impl.hpp"

class ConsoleMedLogger : public MedLoggerImpl {
    public:
        void log(std::string) override;
        void error(std::string) override;
        void warning(std::string) override;
};

#endif