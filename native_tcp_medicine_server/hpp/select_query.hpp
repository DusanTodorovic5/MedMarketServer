#ifndef SELECT_QUERY_CLASS
#define SELECT_QUERY_CLASS

#include "query.hpp"
#include "message.hpp"

class SelectQuery : public Query {
    public:
        SelectQuery(std::vector<Medicine*>, Message&);
        virtual std::vector<Medicine*> get(int count = 0) override;
    private:
        Message& message;
};

#endif