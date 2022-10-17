#ifndef MEDICINE_CLASS
#define MEDICINE_CLASS

#include <iostream>
#include <mutex>

class Medicine {
    public:
        enum Purpose {
            Flu = 1,
            ThroatInfection,
            NoseDrops,
            StomachVirus,
            HighPressure,
            Headache
        };
        Medicine(int, std::string, Purpose, char, int, int);
        static Purpose stringToPurpose(std::string);
        static std::string purposeToString(Medicine::Purpose);
        friend class Query;
        friend class BuyQuery;
        friend std::ostream& operator<<(std::ostream&, Medicine&);
        inline int getCost() const { return this->cost; };
        inline int getAvailability() { return this->availability; }
        void buy(int);
        operator std::string();
    private:
        int id;
        std::string medicineName;
        Purpose purpose;
        char effectivness;
        int cost;
        int availability;
        std::mutex mutex;
};

#endif