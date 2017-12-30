#include <iostream>
#include <iomanip>
#include "simulator.hpp"

int main(int argc, char *argv[]) {
    Simulator s;

    if (argc < 3) {
        std::string path(argv[0]);
        std::string seps("/\\");
        std::string name(std::find_first_of(path.rbegin(), path.rend(), seps.begin(), seps.end()).base(), path.end());
        std::cerr << name << " <model filename> <duration>" << std::endl;
        return 1;
    }

    double dur = 0.0;
    std::istringstream in(argv[2]);
    in >> dur;

    s.read(argv[1]);
    s.print_nodes();
    s.run(dur);

    return 0;
}
