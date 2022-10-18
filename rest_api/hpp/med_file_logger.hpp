#ifndef FILE_MED_LOGGER_IMPLEMENTATION_CLASS
#define FILE_MED_LOGGER_IMPLEMENTATION_CLASS

#include "med_logger_impl.hpp"
#include <fstream>

class FileMedLogger : public MedLoggerImpl {
    public:
        void log(std::string) override;
        void error(std::string) override;
        void warning(std::string) override;
        FileMedLogger();
        ~FileMedLogger();
    private:
        std::ofstream file;
};

#endif