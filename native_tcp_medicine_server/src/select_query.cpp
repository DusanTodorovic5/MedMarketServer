#include "../hpp/select_query.hpp"
#include "../hpp/med_logger.hpp"
SelectQuery::SelectQuery(std::vector<Medicine*> medicines, Message& message) : Query(medicines), message(message) { }

std::vector<Medicine*> SelectQuery::get(int count) {
    if (this->medicines.size() == 0) {
        MedLogger::instance()->log("No medicines found");
        message << "No medicines found" << "\n";
        return {};
    }

    if (count != 0) {
        medicines.resize(count);
    }
    
    for (Medicine* medicine : medicines) {
        MedLogger::instance()->log(*medicine);
        message << *medicine << "\n";
    }

    return {};
}