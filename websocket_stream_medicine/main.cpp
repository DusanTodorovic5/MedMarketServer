#include "hpp/server.hpp"
#include "../hpp/med_logger.hpp"
#include "../hpp/med_file_logger.hpp"

int main(int, char**) {
    MedLogger::init(new FileMedLogger());
    Server server;

    server.run(9002);

    return 0;
}
