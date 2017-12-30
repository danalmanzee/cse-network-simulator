#pragma once

#include <functional>
#include <memory>
#include "client.hpp"

class Event {
    using EventCallback = std::function<void(Event &e)>;

    std::shared_ptr<Client> _client;
    double _time;
    EventCallback _cb;

protected:
    Simulator *_sim;

public:
    Event(Simulator *sim, double time, const std::shared_ptr<Client> &client, const EventCallback &callback);

    const std::shared_ptr<Client> &get_client() const { return _client; }

    void set_client(const std::shared_ptr<Client> &client) { _client = client; }

    double get_time() const { return _time; }

    void set_time(double time) { _time = time; }

    void trigger();

    inline friend bool operator<(const Event &a, const Event &b) {
        return a._time < b._time;
    }

    inline friend bool operator>(const Event &a, const Event &b) {
        return a._time > b._time;
    }
};
