#include "../hpp/medicine_store.hpp"
#include <sys/stat.h>
#include <fstream>
#include <string>
#include <sstream>

MedicineStore::MedicineStore(std::string filename) : name(filename) {
    struct stat buffer;
    if (stat (filename.c_str(), &buffer) != 0) return;

    std::fstream medicineFile;
    medicineFile.open(filename,std::ios::in);

    std::string line;
    std::string part;
    while(getline(medicineFile, line)){
        std::vector<std::string> splitted;
        std::stringstream s(line);

        while (getline(s, part, ' ')) {
            splitted.push_back(part);
        }

        this->medicines.push_back(
            new Medicine(
                std::stoi(splitted[0]),
                splitted[1],
                Medicine::stringToPurpose(splitted[2]),
                splitted[3][0] - '0',
                std::stoi(splitted[4]),
                std::stoi(splitted[5])
        ));
    }

    medicineFile.close();
}

MedicineStore::~MedicineStore() {
    for (Medicine* medicine : medicines) {
        delete medicine;
    }
}

std::string MedicineStore::getName() const {
    return name;
}

SelectQuery MedicineStore::select(Message& message) {
    return SelectQuery(this->medicines, message);
}

Query MedicineStore::buy() {
    return Query(this->medicines);
}

std::ostream& operator<<(std::ostream& os, MedicineStore& em) {
    os << "Name of store: " << em.name << std::endl;

    for (Medicine* m : em.medicines) {
        os << "\t" << *m << std::endl;
    }
    
    return os;
}

MedicineStore::operator std::string() {
    std::string retValue = "Name of store: " + this->name + "\n";
    for (Medicine* m : medicines) {
        retValue += "\t" + std::string(*m) + "\n";
    }
    return retValue;
}