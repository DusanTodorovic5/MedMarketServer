#include "../hpp/query.hpp"
#include <algorithm>
#include <functional>

Query::Query(std::vector<Medicine*> medicines) : medicines(medicines) { }

std::vector<Medicine*> Query::get(int count) {
    return this->medicines;
}

Query& Query::filterByPurpose(Medicine::Purpose purpose, Query::Operation operation) {

    std::function<bool(const Medicine*)> predicate = [&purpose](const Medicine* m) { return m->purpose != purpose; };
    if (operation != Query::Operation::Equal) {
        predicate = [&purpose](const Medicine* m) { return m->purpose == purpose; };
    }

    medicines.erase(std::remove_if(medicines.begin(), medicines.end(),
        predicate), medicines.end());

    return *this;
}

Query& Query::filterByName(std::string name, Query::Operation operation) {

    std::function<bool(const Medicine*)> predicate = [&name](const Medicine* m) { return m->medicineName != name; };
    if (operation != Query::Operation::Equal) {
        predicate = [&name](const Medicine* m) { return m->medicineName == name; };
    }

    medicines.erase(std::remove_if(medicines.begin(), medicines.end(),
        predicate), medicines.end());

    return *this;
}

Query& Query::filterByEffectivness(char effectivness, Query::Operation operation) {
    std::function<bool(const Medicine*)> predicate;
    switch (operation) {
        case Query::Operation::LessThan : predicate = [&effectivness](const Medicine* m) { return m->effectivness >= effectivness; }; break;
        case Query::Operation::BiggerThan : predicate = [&effectivness](const Medicine* m) { return m->effectivness <= effectivness; }; break;
        case Query::Operation::Equal : predicate = [&effectivness](const Medicine* m) { return m->effectivness != effectivness; }; break;
        case Query::Operation::NotLessThan : predicate = [&effectivness](const Medicine* m) { return m->effectivness < effectivness; }; break;
        case Query::Operation::NotBiggerThan : predicate = [&effectivness](const Medicine* m) { return m->effectivness > effectivness; }; break;
        case Query::Operation::NotEqual : predicate = [&effectivness](const Medicine* m) { return m->effectivness == effectivness; }; break;
    }
    medicines.erase(std::remove_if(medicines.begin(), medicines.end(), predicate),medicines.end());
    return *this;
}

Query& Query::filterByPrice(int price, Query::Operation operation) {
    std::function<bool(const Medicine*)> predicate;
    switch (operation) {
        case Query::Operation::LessThan : predicate = [&price](const Medicine* m) { return m->cost >= price; }; break;
        case Query::Operation::BiggerThan : predicate = [&price](const Medicine* m) { return m->cost <= price; }; break;
        case Query::Operation::Equal : predicate = [&price](const Medicine* m) { return m->cost != price; }; break;
        case Query::Operation::NotLessThan : predicate = [&price](const Medicine* m) { return m->cost < price; }; break;
        case Query::Operation::NotBiggerThan : predicate = [&price](const Medicine* m) { return m->cost > price; }; break;
        case Query::Operation::NotEqual : predicate = [&price](const Medicine* m) { return m->cost == price; }; break;
    }
    medicines.erase(std::remove_if(medicines.begin(), medicines.end(), predicate),medicines.end());
    return *this;
}

Query& Query::filterByAvailability(int boxes, Query::Operation operation) {
    std::function<bool(const Medicine*)> predicate;
    switch (operation) {
        case Query::Operation::LessThan : predicate = [&boxes](const Medicine* m) { return m->availability >= boxes; }; break;
        case Query::Operation::BiggerThan : predicate = [&boxes](const Medicine* m) { return m->availability <= boxes; }; break;
        case Query::Operation::Equal : predicate = [&boxes](const Medicine* m) { return m->availability != boxes; }; break;
        case Query::Operation::NotLessThan : predicate = [&boxes](const Medicine* m) { return m->availability < boxes; }; break;
        case Query::Operation::NotBiggerThan : predicate = [&boxes](const Medicine* m) { return m->availability > boxes; }; break;
        case Query::Operation::NotEqual : predicate = [&boxes](const Medicine* m) { return m->availability == boxes; }; break;
    }
    medicines.erase(std::remove_if(medicines.begin(), medicines.end(), predicate),medicines.end());
    return *this;
}

std::string Query::operationToString(Query::Operation operation) {
    switch (operation) {
        case Query::Operation::LessThan : return "Less than";
        case Query::Operation::BiggerThan : return "Bigger than";
        case Query::Operation::Equal : return "Equal to";
        case Query::Operation::NotLessThan : return "Not less than";
        case Query::Operation::NotBiggerThan : return "Not bigger than";
        case Query::Operation::NotEqual : return "Not Equal to";
    }
    return "uknown";
}
Query::Operation Query::stringToOperation(std::string operation) {
    if (operation == "<" || operation == "Less than") {
        return Query::Operation::LessThan;
    } 

    if (operation == ">" || operation == "Bigger than") {
        return Query::Operation::BiggerThan;
    } 

    if (operation == "<=" || operation == "Not bigger than") {
        return Query::Operation::NotBiggerThan;
    } 

    if (operation == ">=" || operation == "Not less than") {
        return Query::Operation::NotLessThan;
    } 

    if (operation == "!=" || operation == "Not Equal to") {
        return Query::Operation::NotEqual;
    } 

    return Query::Operation::Equal;
}
Query& Query::filter(std::string what, Query::Operation operation, std::string value) {
    if (what == "name") {
        return this->filterByName(value, operation);
    }

    if (what == "boxes") {
        return this->filterByAvailability(std::stoi(value), operation);
    }

    if (what == "price") {
        return this->filterByPrice(std::stoi(value), operation);

    }

    if (what == "purpose") {
        return this->filterByPurpose(Medicine::stringToPurpose(value), operation);
    }

    if (what == "effectiveness") {
        return this->filterByEffectivness(value[0] - '0', operation);
    }
    
    return *this;
}