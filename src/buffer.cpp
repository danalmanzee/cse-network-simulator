#include "buffer.hpp"
#include "simulator.hpp"

Buffer::Buffer(Simulator *sim) : Node(sim) {}

//Buffer::~Buffer() = default;

void Buffer::print_stats() {
    stat("queue length", _stat_len_count, _stat_len_count ? (double) _stat_len_total / _stat_len_count : 0);
    stat("wait time", _stat_wait_count, _stat_wait_count ? _stat_wait_total / _stat_wait_count : 0);
}

void Buffer::stat_incr_wait(double wait_time) {
    _stat_wait_total += wait_time;
    ++_stat_wait_count;
}

void Buffer::stat_incr_len(size_t len) {
    _stat_len_total += len;
    ++_stat_len_count;
}

FIFO::FIFO(Simulator *sim, const std::string &arg) : Buffer(sim) {}

bool FIFO::request(const std::shared_ptr<Node> &to) {
    // Send to node.
    stat_incr_len(_queue.size());
    return pop_queue(to);
}

bool FIFO::receive(const std::shared_ptr<Node> &from, const std::shared_ptr<Client> &client) {
    if (!client) {
        log() << "rejecting null client from " << _sim->get_name(from) << std::endl;
        return false;
    }

    _queue.push(std::make_pair(client, _sim->get_time()));
    log(client) << "entered the queue." << std::endl;

    // Try to pop the queue.
    _sim->add_event(Event(_sim, _sim->get_time(), client, [this](Event &e) {
        // Try to pop the queue.
        stat_incr_len(_queue.size());
        pop_queue(get_dst());
    }));

    // Accept the node.
    return true;
}

bool FIFO::pop_queue(const std::shared_ptr<Node> &dst) {
    if (_queue.empty()) {
        log() << "queue empty, can't pop." << std::endl;
        return false;
    }

    if (!dst) {
        log() << "queue has no destination, can't pop." << std::endl;
        return false;
    }

    const auto p = _queue.front();
    const auto &client = p.first;
    const auto &entry_time = p.second;

    log(client, dst) << "attempting to send." << std::endl;

    if (!dst->receive(shared_from_this(), client)) {
        log(client, dst) << "destination rejected." << std::endl;
        return false;
    }

    stat_incr_wait(_sim->get_time() - entry_time);
    _queue.pop();
    log(client) << "no longer in queue." << std::endl;

    return true;
}
