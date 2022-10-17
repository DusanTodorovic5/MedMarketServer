#ifndef QUERY_CLASS
#define QUERY_CLASS

#include "medicine.hpp"
#include <vector>

class Query {
    public:
        enum class Operation {
            LessThan,
            BiggerThan,
            Equal,
            NotLessThan,
            NotBiggerThan,
            NotEqual
        };
        Query(std::vector<Medicine*>);
        virtual Query& filterByPurpose(Medicine::Purpose, Query::Operation operation = Query::Operation::Equal);
        virtual Query& filterByName(std::string, Query::Operation operation = Query::Operation::Equal);
        virtual Query& filterByEffectivness(char, Query::Operation);
        virtual Query& filterByAvailability(int, Query::Operation);
        virtual Query& filterByPrice(int, Query::Operation);
        virtual Query& filter(std::string, Query::Operation, std::string);
        virtual std::vector<Medicine*> get(int count = 0);
        static std::string operationToString(Query::Operation);
        static Query::Operation stringToOperation(std::string);
    protected:
        std::vector<Medicine*> medicines;
};

#endif