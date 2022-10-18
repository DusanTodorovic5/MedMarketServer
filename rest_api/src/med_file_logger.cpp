#include "../hpp/med_file_logger.hpp"

FileMedLogger::FileMedLogger() : file("log.txt") { }

FileMedLogger::~FileMedLogger() {
    this->file.close();
}

void FileMedLogger::log(std::string message) {
    this->mutex.lock();

    std::stringstream ss;
    ss << std::this_thread::get_id();
    this->file << "[ " << this->threads[ss.str()] << " ]: " << message << "\n";
    this->file.flush();

    this->mutex.unlock();
}

void FileMedLogger::error(std::string message){
    this->mutex.lock();

    std::stringstream ss;
    ss << std::this_thread::get_id();
    this->file << "[ " << this->threads[ss.str()] << " - ERROR ]: " << message << "\n"; 
    this->file.flush();

    this->mutex.unlock();
}

void FileMedLogger::warning(std::string message){
    this->mutex.lock();

    std::stringstream ss;
    ss << std::this_thread::get_id();
    this->file << "[ " << this->threads[ss.str()] << " - WARNING ]: " << message << "\n"; 
    this->file.flush();

    this->mutex.unlock();
}

