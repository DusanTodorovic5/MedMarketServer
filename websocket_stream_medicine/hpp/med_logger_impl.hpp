#ifndef MED_LOGGER_IMPLEMENTATION_CLASS
#define MED_LOGGER_IMPLEMENTATION_CLASS

#include <iostream>
#include <sstream>
#include <thread>
#include <map>
#include <mutex>

class MedLoggerImpl {
    public:
        virtual void log(std::string) = 0;
        virtual void error(std::string) = 0;
        virtual void warning(std::string) = 0;
        void setName(std::string name) {
            mutex.lock();
            std::stringstream ss;
            ss << std::this_thread::get_id();
            threads[ss.str()] = name;
            mutex.unlock();
        }
    protected:
        std::map<std::string, std::string> threads;
        std::mutex mutex;
};

#endif