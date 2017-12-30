#include <iostream>
#include <iomanip>
#include <fstream>

#include "buffer.hpp"
#include "generator.hpp"
#include "router.hpp"
#include "server.hpp"
#include "simulator.hpp"

using namespace std;
using NodeCreateFunc = Simulator::NodeCreateFunc;


Simulator::Simulator()
        : _next_client(0), _next_event(0) {
    // Register types.
    _creators = {
            {"Poisson",      _make_generic_generator_creator<Poisson>()},
            {"FIFO",         _make_generic_creator<FIFO>()},
            {"ServerCst",    _make_generic_creator<ServerCst>()},
            {"ServerNormal", _make_generic_creator<ServerNormal>()},
            {"ServerExp",    _make_generic_creator<ServerExp>()},
            {"Dispatch",     _make_generic_creator<Dispatch>()},
            {"Exit",         _make_generic_creator<ServerExit>()}
    };
    // Fallback names and types.
    _names.insert(std::make_pair(nullptr, "?"));
    _types.insert(std::make_pair(nullptr, "?"));
}

void Simulator::read(const char *fn) {
    ifstream file(fn);
    string line;

    while (getline(file, line)) {
        const char *s = line.c_str();
        string name(1, *s++);
        char op = *s++;

        if (op == '=')
            create_node(name, s);
        else if (op == '-' && *s++ == '>')
            direct_node(name, s);
        else
            cerr << "Unknown line: " << line << endl;
    }
}

void Simulator::create_node(const std::string &name, const std::string &type) {
    size_t ppos = type.find('(');
    size_t endpos = type.rfind(')');

    string tname = type.substr(0, ppos);
    string targs = (ppos < endpos) ? type.substr(ppos + 1, (endpos - ppos) - 1) : "";

    auto it = _creators.find(tname);
    if (it != _creators.end()) {
        auto node = it->second(targs);

        _nodes.insert(std::make_pair(name, node));
        _names.insert(std::make_pair(node, name));
        _types.insert(std::make_pair(node, tname));
    }
}

void Simulator::direct_node(const std::string &src, const std::string &dst) {
    direct_node(get_node(src), get_node(dst));
}

void Simulator::direct_node(const std::shared_ptr<Node> &src, const std::shared_ptr<Node> &dst) {
    if (src)
        src->set_dst(dst);
}

void Simulator::add_generator(const std::shared_ptr<Generator> &gen) {
    if (gen)
        _generators.emplace_back(gen);
}

std::shared_ptr<Client> Simulator::add_client() {
    std::ostringstream o;
    o << "C" << _next_client++;
    std::shared_ptr<Client> client(new Client(this, o.str())); // Private constructor.
    _clients.push_back(client);

    log(client) << "client created." << std::endl;
    return client;
}

void Simulator::add_event(const Event &event) {
    _events.push(event);
    log() << "Added event " << _next_event++ << " at time " << event.get_time() << std::endl;
}

void Simulator::print_nodes() const {
    for (auto it : _nodes) {
        std::cout << it.first << " --> ";
        it.second->print();
    }
}

std::shared_ptr<Node> Simulator::get_node(const std::string &name) const {
    auto it = _nodes.find(name);
    if (it != _nodes.end())
        return it->second;
    else
        return nullptr;
}

const std::string &Simulator::get_name(const std::shared_ptr<Node> &node) const {
    auto it = _names.find(node);
    if (it != _names.end())
        return it->second;
    else
        return get_name(nullptr);
}

const std::string &Simulator::get_type(const std::shared_ptr<Node> &node) const {
    auto it = _types.find(node);
    if (it != _types.end())
        return it->second;
    else
        return get_type(nullptr);
}

const std::string &Simulator::get_client_name(const std::shared_ptr<Client> &client) const {
    static const std::string null_client_name = "C?";
    return client ? client->get_name() : null_client_name;
}

double Simulator::get_time() const {
    return _time;
}

void Simulator::run(double duration) {
    std::cout << "Running simulation for " << duration << " units." << std::endl;

    // Generate.
    for (const auto &gen : _generators) {
        auto evt = gen->generate_event();
        add_event(evt);
        std::cout << "Added event: " << _events.size() << " at time " << evt.get_time() << std::endl;
    }

    // Simulate.
    while (!_events.empty()) {
        cleanup();

        auto evt = _events.top();
        double time = evt.get_time();

        if (time > duration)
            break;

        _time = time;
        _events.pop();

        log() << "processing next event." << std::endl;

        evt.trigger();
    }

    log() << "simulation ended." << std::endl;

    // Print stats.
    for (auto it : _nodes) {
        it.second->print_stats();
    }
}

void Simulator::cleanup() {
    for (auto it = _clients.begin(); it != _clients.end();) {
        if ((*it).use_count() < 2) {
            log(*it) << "left the network." << std::endl;
            it = _clients.erase(it);
        } else
            ++it;
    }
}

std::ostream &Simulator::log() {
    return std::cout << "[sim/" << _time << "] ";
}

std::ostream &Simulator::log(const std::shared_ptr<Client> &client) {
    return std::cout << "[sim/" << _time << '/' << get_client_name(client) << "] ";
}
