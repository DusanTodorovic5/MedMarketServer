#ifndef MESSAGE_CLASS
#define MESSAGE_CLASS

#include <iostream>

class Message {
    public:
        Message& operator<<(const std::string& _rhs) {
            buffer += _rhs;
            return *this;
        }
        
        inline char* str() {
            return &buffer[0];
        }

        inline int size() const {
            return buffer.length();
        }
    private:
        std::string buffer;
};

#endif