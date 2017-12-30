#pragma once

#include <memory>
#include <string>
#include <ostream>

class Client;

class Simulator;

class Node : public std::enable_shared_from_this<Node> {
    friend class Simulator;

    std::weak_ptr<Node> _dst;

public:
    explicit Node(Simulator *sim);

    virtual ~Node();

    virtual std::shared_ptr<Node> get_dst();

    void set_dst(const std::shared_ptr<Node> &node);

    virtual void print();

    virtual void print_attributes();

    virtual bool request(const std::shared_ptr<Node> &to);

    virtual bool receive(const std::shared_ptr<Node> &from, const std::shared_ptr<Client> &client);

    virtual void print_stats();

protected:
    void print_target(bool with_comma);

    Simulator *_sim;

    std::ostream &log();

    std::ostream &log(const std::shared_ptr<Client> &client);

    std::ostream &log(const std::shared_ptr<Client> &client, const std::shared_ptr<Node> &dst);

    void stat(const char *name, size_t total, double average);

    void stat_incr_received();

    void stat_incr_sent();

    size_t _stat_received;
    size_t _stat_sent;
};
