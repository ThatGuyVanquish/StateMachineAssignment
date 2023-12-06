#include "stateMachine.h"
#include <vector>
#include <map>
#include <iostream>

// Test runner method: Receives a state machine, a list of events value to print at the end of the run if finished
template <typename T>
int run(StateMachine<T> *sm, const std::vector<MyEvent<T>> &events, const std::string &printOnFinish)
{
    int counter = 0;
    for (auto e : events)
    {
        counter++;
        if (counter == 500)
            // Going up a folder because CLion's root is the build-debug folder
            sm->saveState("./../500StepsSequence.txt");
        sm->handleEvent(e);
    }
    if (sm->hasFinished())
        std::cout << printOnFinish << std::endl;
    else
        std::cout << "Did not reach an end of the state machine" << std::endl;
    return 0;
}

StateMachine<char>* createSM()
{
    // States list to hold all the created states
    std::vector<State<char> *> states;
    // Ends of sequences
    StateHandler<char> *endSeq1 = new StateHandler<char>(1);
    states.push_back(endSeq1);
    StateHandler<char> *endSeq2 = new StateHandler<char>(2);
    states.push_back(endSeq2);
    StateHandler<char> *endSeq3 = new StateHandler<char>(3);
    states.push_back(endSeq3);

    std::vector<State<char> *> ends({endSeq1, endSeq2, endSeq3});

    // T -> State<T>* map used as the StateHandler's eventResults
    std::map<char, State<char> *> eventResults;

    /*
     * Creating the states of all the sequences:
     * The sequences will be created up to the first state (The first received event A),
     * then said state will be created alongside the start state.
     */

    // Creating the states of Sequence 1 from back to front
    eventResults['A'] = endSeq1;
    StateHandler<char> *seq1C = new StateHandler<char>(4, eventResults);
    states.push_back(seq1C);
    eventResults.clear();

    eventResults['C'] = seq1C;
    StateHandler<char> *seq1B = new StateHandler<char>(5, eventResults);
    states.push_back(seq1B);
    eventResults.clear();

    eventResults['B'] = seq1B;
    StateHandler<char> *seq1SecondA = new StateHandler<char>(6, eventResults);
    states.push_back(seq1SecondA);
    eventResults.clear();

    // Creating the states of Sequence 2:
    // First creating the 1000 C states and attaching the end of the sequence to the last event C state
    // Then creating the second state (B event) and connecting it to the first C event state
    long id = 7;
    StateHandler<char> *prevC = nullptr;
    StateHandler<char> *firstC = nullptr;
    for (; id < 1007; id++)
    {
        std::map<char, State<char> *> cStatesResults;
        StateHandler<char> *currentC = new StateHandler<char>(id, cStatesResults);
        states.push_back(currentC);
        if (id == 7)
            firstC = currentC;
        if (prevC != nullptr)
            prevC->addEvent('C', currentC);
        if (id == 1006)
            currentC->addEvent('A', endSeq2);
        prevC = currentC;
    }
    eventResults['C'] = firstC;
    StateHandler<char> *seq2B = new StateHandler<char>(id, eventResults);
    states.push_back(seq2B);
    id++;

    // Creating the states of Sequence 3 from back to front, first connecting the end state with
    // an *ALL* events connection.
    eventResults['A'] = endSeq3;
    eventResults['B'] = endSeq3;
    eventResults['C'] = endSeq3;
    StateHandler<char> *seq3C = new StateHandler<char>(id, eventResults);
    states.push_back(seq3C);
    eventResults.clear();

    // Setting up the first A event state, hooking it up to the 3 viable sequences
    eventResults['A'] = seq1SecondA;
    eventResults['B'] = seq2B;
    eventResults['C'] = seq3C;
    StateHandler<char> *firstA = new StateHandler<char>(496351, eventResults);
    states.push_back(firstA);
    eventResults.clear();

    // Creating the start state that has to have an A event to reach.
    eventResults['A'] = firstA;
    StateHandler<char> *start = new StateHandler<char>(0, eventResults);
    states.push_back(start);

    StateMachine<char> *sm = new StateMachine<char>(states, start, ends);
    return sm;
}

// Test the 4 runs in the doc
int part1(StateMachine<char> *sm)
{
    MyEvent<char> eventA(1, 'A');
    MyEvent<char> eventB(2, 'B');
    MyEvent<char> eventC(3, 'C');

    // Run 1 is A->A->B->C->A
    std::vector<MyEvent<char>> events1({eventA, eventA, eventB, eventC, eventA});

    // Run 2 is A->B->C^1000->A
    std::vector<MyEvent<char>> events2({eventA, eventB});
    for (int i = 0; i < 1000; i++)
    {
        events2.push_back(eventC);
    }
    events2.push_back(eventA);

    // Run 3 is A->C->ANY
    std::vector<MyEvent<char>> events3({eventA, eventC, eventA});

    // Run 4 would be a fail run, so just anything that doesn't start with an A event should get thrown an error
    std::vector<MyEvent<char>> events4({eventB, eventA});

//     Comment out 3 of the 4 runs to run the tests
    std::cout << "Running sequence 1" << std::endl;
    run(sm, events1, "Seq1");
    sm->resumeState("./../InitialState.txt");

    std::cout << "\nRunning sequence 2" << std::endl;
    run(sm, events2, "Seq2");
    sm->resumeState("./../InitialState.txt");

    std::cout << "\nRunning sequence 3" << std::endl;
    run(sm, events3, "Seq3:" + events3.back().toString());
    sm->resumeState("./../InitialState.txt");

    std::cout << "\nRunning sequence 4. This should throw an error, catching and printing instead" << std::endl;
    try
    {
        run(sm, events4, "Seq4");
    }
    catch (std::invalid_argument &test) {
        std::cout << test.what() << std::endl;
    }
    std::cout << "\n";

    return 0;
}

// Test that returning to a previous state can be resumed and continued
int part2(StateMachine<char> *sm)
{
    MyEvent<char> eventA(1, 'A');
    MyEvent<char> eventB(2, 'B');
    MyEvent<char> eventC(3, 'C');

    // Run is A->B->C^1000->A
    std::vector<MyEvent<char>> events({eventA, eventB});
    std::vector<MyEvent<char>> resumedEvents;
    for (int i = 0; i < 1000; i++)
    {
        events.push_back(eventC);
        if (i > 496)
            resumedEvents.push_back(eventC);
    }
    events.push_back(eventA);
    resumedEvents.push_back(eventA);
    // Comment out 3 of the 4 runs to run the tests
    std::cout << "Running save+resume test:" << std::endl;
    run(sm, events, "Sequence finished");

    sm->resumeState("./../500StepsSequence.txt");
    std::cout << "\nResuming back to the 500th step.\nShould print `Sequence finished again`" << std::endl;
    run(sm, resumedEvents, "Sequence finished again");

    return 0;
}

int main()
{
    StateMachine<char> *sm = createSM();
    // Saving the initial state and resetting it before testing again, this is actually another test for part 2
    sm->saveState("./../InitialState.txt");
    part1(sm);
    sm->resumeState("./../InitialState.txt");
    part2(sm);
    delete sm;
    return 0;
}