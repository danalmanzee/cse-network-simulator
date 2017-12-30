#include "event.hpp"

Event::Event(Simulator *sim, double time, const std::shared_ptr<Client> &client, const EventCallback &cb)
        : _sim(sim), _time(time), _client(client), _cb(cb) {}

void Event::trigger() {
    if (_cb)
        _cb(*this);
}
