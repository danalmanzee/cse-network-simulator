#pragma once

#include <list>
#include <random>
#include "node.hpp"

class Router : public Node {
public:
    explicit Router(Simulator *sim);

    ~Router() override;

    bool receive(const std::shared_ptr<Node> &from, const std::shared_ptr<Client> &client) override;

    virtual std::shared_ptr<Node> get_next_dst() = 0;

    void print_stats() override {}
};

class Dispatch : public Router {
    std::list<std::pair<std::weak_ptr<Node>, double>> _targets;

    std::random_device _rd;
    std::mt19937 _gen;
    std::uniform_real_distribution<double> _dis;

public:
    Dispatch(Simulator *sim, const std::string &arg);

    std::shared_ptr<Node> get_next_dst() override;

    void print_attributes() override;
};
