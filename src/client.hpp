#pragma once

#include <string>

class Simulator;

class Client {
    friend class Simulator;

    Simulator *_sim;
    std::string _name;

    Client(Simulator *sim, const std::string &name);

    const std::string &get_name() const { return _name; }

    void set_name(const std::string &name) { _name = name; }
};
