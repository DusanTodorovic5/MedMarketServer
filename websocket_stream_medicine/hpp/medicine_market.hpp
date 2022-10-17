#ifndef MEDICINE_MARKET_CLASS
#define MEDICINE_MARKET_CLASS

#include "medicine_store.hpp"

class MedicineMarket {
    public:
        MedicineMarket();
        void query(std::string, Message&);
        void select(std::string, Message&);
        void buy(std::string, Message&, bool buyAnyways = false);
        friend std::ostream& operator<<(std::ostream&, MedicineMarket&);
        operator std::string();
        ~MedicineMarket();
    private:
        std::pair<std::string, std::vector<std::string>> parseSelect(std::string); 
        std::pair<std::string, std::vector<std::string>> parseBuy(std::string); 
        int findAmount(std::vector<std::string>);
        std::vector<std::string> split (std::string, std::string);
        Query& getForMarket(std::vector<std::string>, Query&, bool buy = false);
        std::vector<MedicineStore*> stores;
};

#endif