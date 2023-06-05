
#include "App.hpp"

int main() {
    App myApp{};

    try {
        myApp.run();
    } catch (const std::exception& e) {
        std::cout << "ERROR ON RUN: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
