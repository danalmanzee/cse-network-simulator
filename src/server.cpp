#include <iostream>
#include <sstream>
#include <stdexcept>
#include "server.hpp"
#include "simulator.hpp"

Server::Server(Simulator *sim)
        : Node(sim), _rd(), _gen(_rd()), _stat_service_total(0), _stat_service_count(0) {}

bool Server::receive(const std::shared_ptr<Node> &from, const std::shared_ptr<Client> &client) {
    log(client) << "receving client..." << std::endl;
    if (_client) {
        log(client) << "rejecing. already holding " << _sim->get_client_name(_client) << std::endl;
        return false;
    }

    // Save source for requests in the future if not preset.
    if (!_source)
        _source = from;

    // Hold onto the client and wait for the request.
    _client = client;

    log(client) << "accepted client." << std::endl;
    stat_incr_received();

    // Generate event.
    _sim->add_event(Event(_sim, _sim->get_time() + get_completion_time(), client,
      [this, entry_time = _sim->get_time()](Event &e) {
          const auto &client = e.get_client();
          if (_client != client)
              throw std::runtime_error("server trying to process client it doesn't own.");

          _client.reset();

          if (auto dst = get_dst()) {
              log(client, dst) << "sending to destination." << std::endl;
              if (!dst->receive(shared_from_this(), client))
                  log(client, dst) << "destination rejected. client died." << std::endl;
              else
                  stat_incr_sent();
          } else
              log(client) << "no destination. client died." << std::endl;

          stat_add_service_time(e.get_time() - entry_time);

          // Request next client.
          _sim->add_event(Event(_sim, e.get_time(), nullptr, [this](Event &e) {
              request_from_source();
          }));
      }));

    // Accept node.
    return true;
}

void Server::request_from_source() {
    if (!_source) {
        log() << "could not request client from null source." << std::endl;
        return;
    }

    log() << "requesting client from " << _sim->get_name(_source) << std::endl;
    if (_source->request(shared_from_this()))
        log() << "request successful from " << _sim->get_name(_source) << std::endl;
    else
        log() << "request failed from " << _sim->get_name(_source) << std::endl;

}

void Server::print_stats() {
    stat("service time", _stat_service_count, _stat_service_count ? _stat_service_total / _stat_service_count : 0);
}

void Server::stat_add_service_time(double service_time) {
    _stat_service_total += service_time;
    ++_stat_service_count;
}

ServerCst::ServerCst(Simulator *sim, const std::string &arg) : Server(sim) {
    std::istringstream in(arg);
    in >> _rate;
}

double ServerCst::get_completion_time() {
    return _rate;
}

void ServerCst::print_attributes() {
    std::cout << _rate;
    print_target(true);

}

double parse_exp(const std::string &arg) {
    double exp;
    std::istringstream in(arg);
    in >> exp;
    return exp;
}

ServerExp::ServerExp(Simulator *sim, const std::string &arg)
        : Server(sim), _exp(parse_exp(arg)), _dis(_exp) {

}

double ServerExp::get_completion_time() {
    return _dis(_gen);
}

void ServerExp::print_attributes() {
    std::cout << _exp;
    print_target(true);
}

double parse_mean(const std::string &arg) {
    double mean;
    std::istringstream in(arg);
    in >> mean;
    return mean;
}

double parse_variance(const std::string &arg) {
    double variance;
    char ign;//ore.
    std::istringstream in(arg);
    in >> variance >> ign >> variance;
    return variance;
}

ServerNormal::ServerNormal(Simulator *sim, const std::string &arg)
        : Server(sim), _mean(parse_mean(arg)), _variance(parse_variance(arg)), _dis(_mean, _variance) {

}

double ServerNormal::get_completion_time() {
    return _dis(_gen);
}

void ServerNormal::print_attributes() {
    std::cout << _mean << ',' << _variance;
    print_target(true);
}

ServerExit::ServerExit(Simulator *sim, const std::string &arg) : Server(sim) {}

double ServerExit::get_completion_time() {
    return 0.0;
}
