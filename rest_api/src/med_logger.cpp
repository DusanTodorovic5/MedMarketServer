#include "../hpp/med_logger.hpp"

MedLoggerImpl* MedLogger::implementation = nullptr;

MedLoggerImpl* MedLogger::instance() {
    return implementation;
}

void MedLogger::init(MedLoggerImpl* impl) {
    implementation = impl;
}

void MedLogger::finish() {
    delete implementation;
}