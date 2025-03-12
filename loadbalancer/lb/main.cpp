#include "lb.h"

int main() {
    try {
        LoadBalancer lb(8080);
        lb.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }

    return 0;
}