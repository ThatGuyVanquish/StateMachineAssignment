#include "state.h"
#include <map>
#include <stdexcept>

template <typename T>
class StateHandler : public State<T>
{
private:
    long id;
    std::map<T, State<T> *> eventResults; // Mapping the value of an event to the State the machine should move to
public:
    StateHandler(long id, const std::map<T, State<T> *> &results) : id(id)
    {
        for (auto result : results)
        {
            eventResults[result.first] = result.second;
        }
    };
    explicit StateHandler(long id) : id(id){}; // An end state constructor

    ~StateHandler() = default;

    State<T> *handleEvent(Event<T> &e)
    {
        auto resultingState = this->eventResults.find(e.getVal());
        if (resultingState != this->eventResults.end())
            return resultingState->second;
        // If event isn't relevant to this state, throw error
        throw std::invalid_argument("State can't handle received event, exiting.");
    }

    long getID() override { return this->id; };

    // Method to add an event.value->result pair to the state
    void addEvent(T val, State<T> *result)
    {
        this->eventResults[val] = result;
    }

    std::string toString() const
    {
        std::ostringstream oss;
        oss << "StateHandler Object { Address: " << this;
        oss << ", ID: " << this->id;
        oss << ", Event Results: { ";
        for (const auto &result : this->eventResults)
        {
            oss << "'" << result.first << "' : " << result.second << " ";
        }
        oss << "} }";
        return oss.str();
    }
};
