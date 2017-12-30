#include <iostream>
#include <sstream>
#include <stdexcept>
#include "router.hpp"
#include "simulator.hpp"

Router::Router(Simulator *sim) : Node(sim) {}

Router::~Router() = default;

bool Router::receive(const std::shared_ptr<Node> &from, const std::shared_ptr<Client> &client) {
    log(client) << "received client." << std::endl;

    stat_incr_received();

    // Generate event.
    _sim->add_event(Event(_sim, _sim->get_time(), client, [this](Event &e) {
        const auto &client = e.get_client();
        if (auto dst = get_next_dst()) {
            log(client, dst) << "sending to destination." << std::endl;
            if (!dst->receive(shared_from_this(), client)) {
                log(client, dst) << "destination rejected. eating." << client.use_count() << std::endl;
            } else {
                stat_incr_sent();
            }
        } else
            log(client) << "no destination. client eaten." << client.use_count() << std::endl;
    }));

    return true;
}

Dispatch::Dispatch(Simulator *sim, const std::string &arg)
        : Router(sim), _rd(), _gen(_rd()), _dis(0.0, 1.0) {
    size_t open = 0, close = 0, incom, outcom;
    double prob;
    do {
        // Parse (a,b), from Dispatch[(A,0.2),(E,0.4),etc])
        open = arg.find('(', close); // open paren
        incom = arg.find(',', open); // comma on the inside
        close = arg.find(')', incom); // close paren
        outcom = arg.find(',', close); // comma after if any. if not, defaults to npos, which is fine.

        // Validate positions of the characters.
        if (open == arg.npos || !(open < incom && incom < close && close < outcom)) break;

        // Parse the name and probability strings.
        std::string name = arg.substr(open + 1, (incom - (open + 1)));
        std::string prob_str = arg.substr(incom + 1, (close - (incom + 1)));
        std::istringstream in(prob_str);
        in >> prob;


        _targets.emplace_back(std::make_pair(_sim->get_node(name), prob));
    } while (true);
}

std::shared_ptr<Node> Dispatch::get_next_dst() {
    double r = _dis(_gen);

    double total = 0.0;
    for (auto p : _targets) {
        total += p.second;
        if (r <= total)
            return p.first.lock();
    }

    throw std::runtime_error("rand > total");
}

void Dispatch::print_attributes() {
    std::cout << '[';
    bool first = true;
    for (auto p : _targets) {
        auto dst = p.first.lock();
        auto prob = p.second;

        if (!first)
            std::cout << ',';

        std::cout << '(' << _sim->get_name(dst) << ',' << prob << ')';

        first = false;
    }
    std::cout << ']';
}
