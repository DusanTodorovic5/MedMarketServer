#include "../hpp/medicine_market.hpp"
#include <map>
#include <algorithm>
#include "../hpp/med_logger.hpp"

MedicineMarket::MedicineMarket() {
    stores.push_back(new MedicineStore("medicine1.txt"));
    stores.push_back(new MedicineStore("medicine2.txt"));
}

MedicineMarket::~MedicineMarket() {
    for (MedicineStore* ms : stores) {
        delete ms;
    }
}

std::vector<std::string> MedicineMarket::split(std::string s, std::string delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find (delimiter, pos_start)) != std::string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        if (token != "") res.push_back (token);
    }

    res.push_back (s.substr (pos_start));
    return res;
}

std::pair<std::string, std::vector<std::string>> MedicineMarket::parseSelect(std::string query) {
    std::vector<std::string> splittedQuery = split(query, "where");
    if (splittedQuery.size() < 2) {
        return {"", {}};
    }

    std::vector<std::string> splittedCriterium = split(splittedQuery[1], "and");

    return {splittedQuery[0], splittedCriterium};
}

std::pair<std::string, std::vector<std::string>> MedicineMarket::parseBuy(std::string query) {
    std::vector<std::string> splittedQuery = split(query, "where");

    if (splittedQuery.size() < 1) {
        return {"", {}};
    }

    std::vector<std::string> queries;
    
    for (int i=1;i<splittedQuery.size();i++) {
        queries.push_back(splittedQuery[i]);
    }

    return {splittedQuery[0], queries};
}

Query& MedicineMarket::getForMarket(std::vector<std::string> criteriums, Query& query, bool buy) {
    for (std::string criterium : criteriums) {
        std::vector<std::string> splitted = split(criterium, " ");
        if (splitted.size() != 3 && splitted.size() != 4) {
            return query.badQuery();
        }
        
        if (buy && splitted[0] == "boxes") {
            splitted[1] = ">=";
        }
        
        query.filter(splitted[0], Query::stringToOperation(splitted[1]), splitted[2]);
    }

    return query;
}

void MedicineMarket::select(std::string query, Message& message) {
    std::pair<std::string, std::vector<std::string>> retValue = parseSelect(query);
    
    if (retValue.first == "") {
        //MedLogger::instance()->error("Bad Query!");
        message << "Bad Query!\n";
        return;
    }

    ////MedLogger::instance()->log("Requester: " + retValue.first);
    message << "Requester: " << retValue.first << "\n";
    for (MedicineStore* ms : stores) {
        // //MedLogger::instance()->log("Query for market: " + ms->getName());
        message << "Query for market: " << ms->getName() << "\n";
        SelectQuery q = ms->select(message);
        getForMarket(retValue.second, q);
        q.get();
    }
}

struct QueryForLater {
    Medicine* med;
    std::string query;
    int amount;
    int pastAmount;
    bool bought;

    QueryForLater(Medicine* med, std::string query, int amount) {
        this->med = med;
        this->query = query;
        this->amount = amount;
        pastAmount = 0;
        bought = false;
    }

    friend std::ostream& operator<<(std::ostream& os, QueryForLater& q) {
        if (!q.med && !q.bought) {
            os << "Found no elements for query: " + q.query << std::endl;
            return os;
        }

        os << "Updating medicine for query: " + q.query << std::endl;
        os << *q.med << std::endl;
        os << "Past amount: " << std::to_string(q.pastAmount) << std::endl;
        os << "New amount: " << q.med->getAvailability();
        return os;
    }

    operator std::string() {
        std::string retValue;

        if ((!med || med->getAvailability() < amount) && !bought) {
            retValue = "Found no elements for query: " + query;
            return retValue;
        }

        retValue += "Updating medicine for query: " + query + "\n";
        retValue += std::string(*med) + "\n";
        retValue += "Past amount: " + std::to_string(pastAmount) + "\n";
        retValue += "New amount: " + std::to_string(med->getAvailability());
        return retValue;
    }

    void buy() {
        pastAmount = med->getAvailability();
        if (med) {
            bought = true;
            med->buy(amount);
        }
    }
};

void MedicineMarket::buy(std::string query, Message& message, bool buyAnyways) {
    std::pair<std::string, std::vector<std::string>> retValue = parseBuy(query);
    
    if (retValue.first == "") return;

    std::vector<QueryForLater> queryForLater;
    for (std::string queries : retValue.second) {
        std::vector<std::string> criteriums = split(queries, "and");
        int amount = findAmount(criteriums);
        int notFound = 0;
        Medicine* minimum = nullptr;

        for (MedicineStore* ms : stores) {
            Query q = ms->buy();
            getForMarket(criteriums, q, true);

            std::vector<Medicine*> medicines = q.get();
            if (medicines.size() == 0) {
                ++notFound;
                continue;
            }

            Medicine* min = *(std::min_element(medicines.begin(), medicines.end(), 
                [](const Medicine* a, const Medicine* b){
                    return a->getCost() < b->getCost();
                }));

            if (!minimum) {
                minimum = min;
                continue;
            }

            if (minimum->getCost() > min->getCost()) {
                minimum = min;
            }
        }

        
        if (!buyAnyways && notFound == stores.size()) {
            //MedLogger::instance()->error("Requester: " + retValue.first);
            message << "Requester: " << retValue.first << "\n";
            //MedLogger::instance()->error("Found no elements for query: " + queries);
            message << "Found no elements for query: " << queries << "\n";
            //MedLogger::instance()->error("Finishing query for " + retValue.first);
            message << "Finishing query for " << retValue.first << "\n";
            return;
        }

        queryForLater.push_back(QueryForLater(minimum, queries, amount));
    }

    //MedLogger::instance()->log("Requester: " + retValue.first);
    for (QueryForLater q : queryForLater) {
        q.buy();
        message << q << "\n";

        if (!q.med) {
            //MedLogger::instance()->warning(q);
            continue;
        }
        
        //MedLogger::instance()->log(q);
    }
}

int MedicineMarket::findAmount(std::vector<std::string> criteriums) {
    for (std::string criterium : criteriums) {
        std::vector<std::string> splitted = split(criterium, " ");
        if (splitted[0] == "boxes") {
            return std::stoi(splitted[2]);
        }
    }
    return 0;
}

std::ostream& operator<<(std::ostream& os, MedicineMarket& em) {
    for (MedicineStore* m : em.stores) {
        os << *m << std::endl << std::endl;
    }
    return os;
}

MedicineMarket::operator std::string() {
    std::string retValue;
    for (MedicineStore* m : stores) {
        retValue += std::string(*m) + "\n";
    }
    return retValue;
}

void MedicineMarket::query(std::string queryString, Message& message) {
    std::vector<std::string> splitted = split(queryString, " for ");

    if (splitted.size() != 2) {
        message << "Invalid query!\n";
        return;
    }

    if (splitted[0] == "select") {
        select(splitted[1], message);
        return;
    }

    if (splitted[0] == "buy") {
        buy(splitted[1], message);
        return;
    }

    if (splitted[0] == "buy anyways") {
        buy(splitted[1], message, true);
        return;
    }

    message << "Invalid query!\n";
}