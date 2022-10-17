#include "../hpp/med_console_logger.hpp"


void ConsoleMedLogger::log(std::string message) {
    this->mutex.lock();
    std::stringstream ss;
    ss << std::this_thread::get_id();
    std::cout << "[ " << this->threads[ss.str()] << " ]: " << message  << std::endl;
    this->mutex.unlock();
}

void ConsoleMedLogger::error(std::string message){
    this->mutex.lock();
    std::stringstream ss;
    ss << std::this_thread::get_id();
    std::cout << "[ " << this->threads[ss.str()] << " ]: " << "\033[91m" << message << "\033[0m" << std::endl;
    this->mutex.unlock();
}

void ConsoleMedLogger::warning(std::string message){
    this->mutex.lock();
    std::stringstream ss;
    ss << std::this_thread::get_id();
    std::cout << "[ " << this->threads[ss.str()] << " ]: " << "\033[93m" << message << "\033[0m" << std::endl;
    this->mutex.unlock();
}
