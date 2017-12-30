#pragma once

#include <random>
#include "event.hpp"
#include "node.hpp"

class Generator : public Node {
public:
    explicit Generator(Simulator *sim);

    ~Generator() override = default;

    virtual Event generate_event() = 0;

    void print_stats() override {}
};

class Poisson : public Generator {
    double _lambda;
    std::random_device _rd;
    std::mt19937 _gen;
    std::uniform_real_distribution<double> _dis;

    double _last;

public:
    Poisson(Simulator *sim, const std::string &rate);

    double get_lambda() const { return _lambda; }

    void set_lambda(double lambda) { _lambda = lambda; }

    double get_inter_arrival();

    Event generate_event() override;

    void print_attributes() override;

    bool receive(const std::shared_ptr<Node> &from, const std::shared_ptr<Client> &client) override;
};
