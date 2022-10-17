#include "../hpp/medicine.hpp"

Medicine::Medicine(int id, std::string name, Purpose purpose, char level, int cost, int avaliability) :
    id(id), medicineName(name), purpose(purpose), effectivness(level), cost(cost), availability(avaliability) { }

Medicine::Purpose Medicine::stringToPurpose(std::string purpose) {
    if (purpose == "T1" || purpose == "Flu") return Purpose::Flu;
    if (purpose == "T2" || purpose == "Throat-Infection") return Purpose::ThroatInfection;
    if (purpose == "T3" || purpose == "Nose-Drops") return Purpose::NoseDrops;
    if (purpose == "T4" || purpose == "Stomach-Virus") return Purpose::StomachVirus;
    if (purpose == "T5" || purpose == "High-Pressure") return Purpose::HighPressure;
    return Purpose::Headache;
}

std::string Medicine::purposeToString(Medicine::Purpose purpose) {
    switch (purpose) {
        case Purpose::Flu: return "Flu";
        case Purpose::ThroatInfection: return "Throat Infection";
        case Purpose::NoseDrops: return "Nose Drops";
        case Purpose::StomachVirus: return "Stomach Virus";
        case Purpose::HighPressure: return "High Pressure";
        case Purpose::Headache: return "Headache";
    }
    return "uknown";
}

std::ostream& operator<<(std::ostream& os, Medicine& em) {
    os << "Name: " << em.medicineName << " ";
    os << "Purpose: " << Medicine::purposeToString(em.purpose) << " ";
    os << "Cost: " << em.cost << " ";
    os << "Effectivness: " << int(em.effectivness);
    return os;
}

void Medicine::buy(int amount) {
    mutex.lock();

    this->availability -= amount;
    
    mutex.unlock();
}

Medicine::operator std::string() {
    int eff = int(effectivness);
    return "Name: " + medicineName + " Purpose: " + Medicine::purposeToString(purpose) +
            " Cost: " + std::to_string(cost) + " Effectivness: " + std::to_string(eff) + "\n";
}