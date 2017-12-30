#pragma once

#include <queue>
#include "client.hpp"
#include "node.hpp"

class Buffer : public Node {
public:
    explicit Buffer(Simulator *sim);

    ~Buffer() override = default;

    virtual size_t get_size() const = 0;

    void print_stats() override;

protected:
    void stat_incr_wait(double wait_time);

    void stat_incr_len(size_t len);

    double _stat_wait_total;
    size_t _stat_wait_count;
    size_t _stat_len_total;
    size_t _stat_len_count;
};


class FIFO : public Buffer {
    std::queue<std::pair<std::shared_ptr<Client>, double>> _queue;

public:
    explicit FIFO(Simulator *sim, const std::string &arg);

    bool request(const std::shared_ptr<Node> &to) override;

    bool receive(const std::shared_ptr<Node> &from, const std::shared_ptr<Client> &client) override;

    size_t get_size() const override { return _queue.size(); }

private:
    bool pop_queue(const std::shared_ptr<Node> &dst);
};
