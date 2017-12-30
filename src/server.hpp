#pragma once

#include <random>
#include "node.hpp"

class Server : public Node {
protected:
    std::random_device _rd;
    std::mt19937 _gen;

    std::shared_ptr<Node> _source;
    std::shared_ptr<Client> _client;

public:
    explicit Server(Simulator *sim);

    ~Server() override = default;

    bool receive(const std::shared_ptr<Node> &from, const std::shared_ptr<Client> &client) override;

    virtual double get_completion_time() = 0;

    void print_stats() override;

protected:
    void request_from_source();

    void stat_add_service_time(double service_time);

    double _stat_service_total;
    size_t _stat_service_count;
};

class ServerCst : public Server {
    double _rate;

public:
    ServerCst(Simulator *sim, const std::string &arg);

    double get_completion_time() override;

    double get_rate() const { return _rate; }

    void set_rate(double rate) { _rate = rate; }

    void print_attributes() override;
};

class ServerExp : public Server {
    double _exp;
    std::exponential_distribution<double> _dis;

public:
    ServerExp(Simulator *sim, const std::string &arg);

    double get_completion_time() override;

    double get_exp() const { return _exp; }

    void set_exp(double exp) { _exp = exp; }

    void print_attributes() override;
};

class ServerNormal : public Server {
    double _mean;
    double _variance;

    std::normal_distribution<double> _dis;

public:
    ServerNormal(Simulator *sim, const std::string &arg);

    double get_completion_time() override;

    double get_mean() const { return _mean; }

    void set_mean(double mean) { _mean = mean; }

    double get_variance() const { return _variance; }

    void set_variance(double variance) { _variance = variance; }

    void print_attributes() override;
};


class ServerExit : public Server {
public:
    explicit ServerExit(Simulator *sim, const std::string &arg);

    double get_completion_time() override;

    void print_stats() override {}
};
