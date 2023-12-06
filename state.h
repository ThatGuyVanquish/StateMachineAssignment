#include "event.h"

template <typename T>
class State
{
public:
    virtual ~State() = default;
    virtual State *handleEvent(Event<T> &e) = 0;
    virtual long getID() = 0;
};