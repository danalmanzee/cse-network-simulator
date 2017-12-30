#pragma once

#include <functional>
#include <list>
#include <map>
#include <memory>
#include <ostream>
#include <string>
#include <deque>
#include <queue>
#include "event.hpp"
#include "node.hpp"

class Client;

class Generator;

class Simulator {
public:
    using NodeCreateFunc = std::function<std::shared_ptr<Node>(const std::string &)>;
    using NodeCreateMap = std::map<std::string, NodeCreateFunc>;
    using NodeMap = std::map<std::string, std::shared_ptr<Node> >;
    using NodeStringMap = std::map<std::shared_ptr<Node>, std::string>;

    Simulator();

    void read(const char *fn);

    void create_node(const std::string &name, const std::string &type);

    void direct_node(const std::string &src, const std::string &dst);

    double get_time() const;

    void print_nodes() const;

    std::shared_ptr<Node> get_node(const std::string &node) const;

    const std::string &get_name(const std::shared_ptr<Node> &node) const;

    const std::string &get_type(const std::shared_ptr<Node> &node) const;

    const std::string &get_client_name(const std::shared_ptr<Client> &client) const;

    void run(double duration);

    std::shared_ptr<Client> add_client();

    void add_event(const Event &event);

private:
    void direct_node(const std::shared_ptr<Node> &src, const std::shared_ptr<Node> &dst);

    void add_generator(const std::shared_ptr<Generator> &gen);

    void cleanup();

    template<typename T>
    NodeCreateFunc _make_generic_creator();

    template<typename T>
    NodeCreateFunc _make_generic_generator_creator();

    std::ostream &log();

    std::ostream &log(const std::shared_ptr<Client> &client);

    NodeCreateMap _creators;

    NodeMap _nodes;
    NodeStringMap _names;
    NodeStringMap _types;

    double _time;

    std::deque<std::shared_ptr<Client>> _clients;
    size_t _next_client;

    std::priority_queue<Event, std::vector<Event>, std::greater<>> _events;
    size_t _next_event;

    std::list<std::shared_ptr<Generator>> _generators;
};

template<typename T>
Simulator::NodeCreateFunc Simulator::_make_generic_creator() {
    return [this](const std::string &arg) { return std::make_shared<T>(this, arg); };
}

template<typename T>
Simulator::NodeCreateFunc Simulator::_make_generic_generator_creator() {
    return [this](const std::string &arg) {
        auto gen = std::make_shared<T>(this, arg);
        add_generator(gen);
        return gen;
    };
}
