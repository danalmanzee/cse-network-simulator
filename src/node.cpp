#include <iostream>
#include <iomanip>
#include "node.hpp"
#include "simulator.hpp"

Node::Node(Simulator *sim) : _sim(sim) {}

Node::~Node() = default;

std::shared_ptr<Node> Node::get_dst() {
    return _dst.lock();
}

void Node::set_dst(const std::shared_ptr<Node> &dst) {
    _dst = dst;
}

bool Node::request(const std::shared_ptr<Node> &to) {
    return false;
}

bool Node::receive(const std::shared_ptr<Node> &from, const std::shared_ptr<Client> &client) {
    log(client) << "received client." << std::endl;

    stat_incr_received();

    // Generate event.
    _sim->add_event(Event(_sim, _sim->get_time(), client, [this](Event &e) {
        const auto &client = e.get_client();
        if (auto dst = get_dst()) {
            log(client, dst) << "sending to destination." << std::endl;
            if (!dst->receive(shared_from_this(), client)) {
                log(client, dst) << "destination rejected. eating." << client.use_count() << std::endl;
            } else {
                stat_incr_sent();
            }
        } else
            log(client) << "no destination. client eaten." << client.use_count() << std::endl;
    }));

    // Accept node.
    return true;
}

void Node::print() {
    const auto thisptr = shared_from_this();

    std::cout << _sim->get_name(thisptr) << '=' << _sim->get_type(thisptr) << '(';
    print_attributes();
    std::cout << ')' << std::endl;
}

void Node::print_attributes() {
    print_target(false);
}

void Node::print_target(bool with_comma) {
    if (auto dst = get_dst()) {
        if (with_comma)
            std::cout << ',';

        std::cout << "target=" << _sim->get_name(dst);
    }
}

void Node::print_stats() {
    stat("received", _stat_received + _stat_sent, _stat_received);
    stat("sent", _stat_sent + _stat_received, _stat_sent);
}

std::ostream &Node::log() {
    return std::cout << "[N" << _sim->get_name(shared_from_this()) << "] ";
}

std::ostream &Node::log(const std::shared_ptr<Client> &client) {
    return std::cout << "[N" << _sim->get_name(shared_from_this()) << '/' << _sim->get_client_name(client) << "] ";
}

std::ostream &Node::log(const std::shared_ptr<Client> &client, const std::shared_ptr<Node> &dst) {
    return std::cout << "[N" << _sim->get_name(shared_from_this()) << '/'
                     << _sim->get_client_name(client) << " -> " << _sim->get_name(dst) << "] ";
}

void Node::stat(const char *name, size_t total, double average) {
    std::ios s(nullptr);
    s.copyfmt(std::cout);

    std::ostringstream left, right;
    left << _sim->get_name(shared_from_this()) << '-' << name << '(' << total << ')';
    right << ": " << std::fixed << average;

    std::cout << std::setfill(' ')
              << std::left << std::setw(30)
              << left.str()
              << std::left << std::setw(10)
              << right.str()
              << std::endl;
    std::cout.copyfmt(s);
}

void Node::stat_incr_received() {
    ++_stat_received;
}

void Node::stat_incr_sent() {
    ++_stat_sent;
}
