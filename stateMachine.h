#include "stateHandler.h"
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

template <typename T>
class StateMachine
{
private:
    std::vector<State<T> *> states;
    State<T> *currentState;
    std::vector<State<T> *> ends; // List of 'end' states for the machine, used for hasFinished

public:
    StateMachine(const std::vector<State<T> *> &states, State<T> *start, const std::vector<State<T> *> &ends) : states(states),
                                                                                                                currentState(start),
                                                                                                                ends(ends){};

    ~StateMachine()
    {
        for (State<T> *state : states)
            delete state;
    }

    void handleEvent(Event<T> &e)
    {
        this->currentState = this->currentState->handleEvent(e);
    };

    // Compare the current state's ID with any of the end states IDs to check if finished
    bool hasFinished()
    {
        for (auto &end : ends)
        {
            if (this->currentState->getID() == end->getID())
                return true;
        }
        return false;
    };

    // Method to save the state of the machine as a `JSON`
    // Assuming the states and state machine object are persistent and know of their type
    void saveState(const std::string &fileName)
    {
        std::string state = "{currentStateID: " + std::to_string(this->currentState->getID()) + "}";

        std::ofstream file(fileName);
        // Check if the file is open, if it isn't open it
        if (!file.is_open())
            file.open(fileName);
        // Seek the put stream to 0 to overwrite the file's content
        file.seekp(0);
        file << state;
        file.close();
    };

    // Method to resume a state machine from a specific state
    // Assuming the states are persistent or were retrieved into the object before, can retrieve the current state
    // from the state machine's states vector
    void resumeState(const std::string &fileName)
    {
        std::ifstream file(fileName);
        // Check if the file is open, if it isn't open it
        if (!file.is_open())
            file.open(fileName);
        std::string stateData;
        // Retrieve the data to the string
        for (;std::getline(file, stateData););
        file.close();

        // Handle resuming the state:
        // Find the current state ID
        size_t startIndexOfStateID = stateData.find(' ') + 1;
        size_t endIndexOfStateID = stateData.find(',');
        long currentStateID = std::stol(stateData.substr(startIndexOfStateID, endIndexOfStateID - startIndexOfStateID));

        // Find the current state within the states vector and place it within this machine
        for(State<T> *state : this->states) {
            if (state->getID() == currentStateID) {
                this->currentState = state;
                break;
            }
        }
    };
};
