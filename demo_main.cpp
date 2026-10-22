// demo_main.cpp -- live demo driver for the ServerConfig class.
//
// Build:   g++ -std=c++20 -Wall -Wextra -Werror -pedantic -O2 -o demo demo_main.cpp ServerConfig.cpp
// Run:     ./demo ../data/server.properties
//
// The demo script (see the Demo handout) walks through each numbered block.
#include "ServerConfig.h"
#include <iostream>

int main(int argc, char* argv[]) {
    std::string path = (argc > 1) ? argv[1] : "server.properties";

    // 1. Construct the object. The constructor opens and parses the file.
    ServerConfig cfg(path);
    if (cfg.settingCount() == 0) {
        std::cout << "Nothing loaded. Is the path right? (" << path << ")\n";
        return 1;
    }
    std::cout << "Loaded " << cfg.settingCount() << " settings from " << path << "\n\n";

    // 2. Read a few settings, each converted to the right type.
    std::cout << "motd         = \"" << cfg.get("motd") << "\"\n";
    std::cout << "max-players  = " << cfg.getInt("max-players") << "\n";
    std::cout << "difficulty   = " << cfg.get("difficulty") << "\n";
    std::cout << "white-list   = " << (cfg.getBool("white-list") ? "on" : "off") << "\n";
    std::cout << "level-seed   = \"" << cfg.get("level-seed") << "\"  (empty = random)\n";
    std::cout << "query.port   = " << cfg.getInt("query.port") << "\n";
    std::cout << "level-type   = " << cfg.get("level-type") << "   <- note the \\: escape\n";
    std::cout << "not-a-key    = " << cfg.get("not-a-key", "<missing>") << "\n\n";

    // 3. Change some settings through the object...
    cfg.set("motd", "CS Class Server - now with 100% more C++");
    cfg.set("max-players", 30);
    cfg.set("difficulty", "hard");
    cfg.set("pvp", true);                // this key doesn't exist yet -> gets appended

    // 4. ...and write the result to a NEW file (never clobber the original in a demo).
    if (cfg.save("server.properties.modified"))
        std::cout << "Wrote server.properties.modified\n";

    // 5. Prove the round trip works: load the file we just wrote.
    ServerConfig check("server.properties.modified");
    std::cout << "Re-loaded: motd = \"" << check.get("motd") << "\", max-players = "
              << check.getInt("max-players") << ", pvp = " << check.get("pvp") << "\n";

    return 0;
}
