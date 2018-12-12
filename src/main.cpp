#include <simulator/Simulator.hpp>

int main() {
//    std::ios_base::sync_with_stdio(false);

    std::string settingsFilePrefix{"config"};
    settingsFilePrefix += '/';
    std::string settingsFile{"config.json"};
    Settings settings(settingsFilePrefix, settingsFile);
    
    Simulator simulator{settings};
    simulator.run();
}