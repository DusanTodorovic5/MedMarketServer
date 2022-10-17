#ifndef MED_LOGGER_CLASS
#define MED_LOGGER_CLASS

#include "med_logger_impl.hpp"

class MedLogger {
    public:
        static MedLoggerImpl* instance();
        static void init(MedLoggerImpl*);
        static void finish();
    private:
        MedLogger() { }
        static MedLoggerImpl* implementation;
};

#endif