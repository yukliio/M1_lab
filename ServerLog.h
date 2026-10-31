// ServerLog.h -- reads log files and builds up a list of Players.
//
// This class is COMPOSED of other classes: it owns a vector of Player objects and
// creates a LogEntry for every line it reads. Composition ("has-a") is the most
// common way classes work together.
#pragma once
#include <string>
#include <vector>
#include "LogEntry.h"
#include "Player.h"

// One row for logins.csv: where a player was standing when they logged in.
struct LoginRecord {
    std::string player;
    std::string date;       // taken from the file name, e.g. "2026-10-18"
    std::string time;       // "HH:MM:SS"
    std::string dimension;  // "overworld", "the_nether", "the_end"
    double x, y, z;
};

class ServerLog {
public:
    // Read one log file. Lines are processed in order. Returns false if the
    // file can't be opened. 'date' is just a label carried into LoginRecords.
    bool processFile(const std::string& path, const std::string& date);

    // ---- results ----
    const std::vector<Player>&      players() const { return players_; }   // in first-seen order
    const std::vector<LoginRecord>& logins()  const { return logins_; }
    const Player* findPlayer(const std::string& name) const;   // nullptr if unknown

    int filesRead()     const { return filesRead_; }
    int linesRead()     const { return linesRead_; }
    int malformedLines() const { return malformed_; }
    int warnings()      const { return warnings_; }

    // Totals across all players, most common first
    std::vector<CauseCount> deathCauseTotals() const;
    // Pointers to every player, sorted by time online, most first.
    // (Call this only after all files are processed: adding a player can move the vector.)
    std::vector<const Player*> playersByTimeOnline() const;
    std::vector<const Player*> playersByLongestSession() const;

    // Write the summary report. Returns false if the file can't be created.
    bool writeReport(const std::string& path) const;
    bool writeLoginsCsv(const std::string& path) const;


private:
    std::vector<Player>       players_;     // every player we have seen
    std::vector<LoginRecord>  logins_;
    std::vector<std::string>  knownNames_;  // from "UUID of player X" lines

    int filesRead_ = 0, linesRead_ = 0, malformed_ = 0, warnings_ = 0;
    int lastSeconds_ = 0;    // last timestamp seen in the current file

    void handle(const LogEntry& e, const std::string& date);
    void handleSystemChat(const LogEntry& e, const std::string& text);
    Player& player(const std::string& name);   // find-or-create
    bool isKnownPlayer(const std::string& name) const;
};
