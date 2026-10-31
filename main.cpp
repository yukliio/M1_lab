// main.cpp -- Part 1: Server Log Analyzer
//
// Build:  g++ -std=c++20 -Wall -Wextra -Werror -pedantic -O2 -o loganalyzer main.cpp LogEntry.cpp Player.cpp ServerLog.cpp
// Run:    ./loganalyzer ../data/logs/2026-10-16-1.log ../data/logs/2026-10-17-1.log ... ../data/logs/latest.log
//   (give the files in date order -- the analyzer processes them in the order you list them)
//
// Writes:  report.txt   logins.csv
#include "ServerLog.h"
#include <iostream>
#include <cassert>

// "2026-10-18-1.log" -> "2026-10-18";  "latest.log" -> "latest"
static std::string dateFromFilename(const std::string& path) {
    std::size_t slash = path.find_last_of("/\\");
    std::string name = (slash == std::string::npos) ? path : path.substr(slash + 1);
    if (name.size() >= 10 && name[4] == '-' && name[7] == '-') return name.substr(0, 10);
    return "latest";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "usage: loganalyzer <log file> [more log files, in date order]\n";
        return 1;
    }

    ServerLog log;
    for (int i = 1; i < argc; ++i) {
        std::string path = argv[i];
        std::cout << "Reading " << path << " ... ";
        if (log.processFile(path, dateFromFilename(path)))
            std::cout << "ok\n";
        else
            std::cout << "FAILED\n";
    }

    std::cout << "\n" << log.linesRead() << " lines, " << log.players().size()
              << " players, " << log.logins().size() << " logins with coordinates, "
              << log.malformedLines() << " malformed lines\n";

    if (log.writeReport("report.txt"))   std::cout << "Wrote report.txt\n";
    if (log.writeLoginsCsv("logins.csv")) std::cout << "Wrote logins.csv\n";

    return 0;
}
