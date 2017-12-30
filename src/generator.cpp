#include <iostream>
#include <sstream>
#include "generator.hpp"
#include "simulator.hpp"

Generator::Generator(Simulator *sim) : Node(sim) {}

//Generator::~Generator() = default;

Poisson::Poisson(Simulator *sim, const std::string &lambda)
        : Generator(sim), _rd(), _gen(_rd()), _dis(0.0, 1.0), _last(0.0) {
    std::istringstream in(lambda);
    in >> _lambda;
}

double Poisson::get_inter_arrival() {
    double rnd = _dis(_gen);
    return -std::log(1.0 - rnd) / _lambda;
}

Event Poisson::generate_event() {
    return Event(_sim, _last += get_inter_arrival(), _sim->add_client(), [this](Event &e) {
        const auto &client = e.get_client();
        log(client) << "entered the network." << std::endl;
        if (auto dst = get_dst()) {
            log(client, dst) << "forwarding to destination." << std::endl;
            if (!dst->receive(shared_from_this(), e.get_client()))
                log(client, dst) << "destination rejected client. client died." << std::endl;
            else
                stat_incr_sent();
        }

        // Generate next arrival event.
        _sim->add_event(generate_event());
    });
}

bool Poisson::receive(const std::shared_ptr<Node> &from, const std::shared_ptr<Client> &client) {
    return false; // Generators don't receive.
}

void Poisson::print_attributes() {
    std::cout << _lambda;
    print_target(true);
}
