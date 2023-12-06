#include <string>
#include <sstream>

template <typename T>
class Event
{
private:
    long id;
    T val;

public:
    Event(long id, T val) : id(id), val(val){};

    T getVal() const { return this->val; };

    long getID() const { return this->id; };

    std::string toString() const
    {
        std::ostringstream oss;
        oss << "Event Object { Address: " << this
            << ", ID: " << this->id
            << ", Val: " << this->val << " }";
        return oss.str();
    }
};