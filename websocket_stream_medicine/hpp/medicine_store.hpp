#ifndef MEDICINE_STORE_CLASS
#define MEDICINE_STORE_CLASS

#include "select_query.hpp"
class MedicineStore {
    public:
        MedicineStore(std::string);
        std::string getName() const;
        SelectQuery select(Message&);
        Query buy();
        ~MedicineStore();
        friend std::ostream& operator<<(std::ostream&, MedicineStore&);
        operator std::string();
    private:
        std::string name;
        std::vector<Medicine*> medicines;
};

#endif