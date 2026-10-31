// ServerLog.cpp -- STARTER. Fill in every function marked TODO.
#include "ServerLog.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <string>
#include <vector>

// ---------------------------------------------------------------- reading

bool ServerLog::processFile(const std::string& path, const std::string& date) {
    // TODO (Task 4):
    //   1. Open the file with std::ifstream. If !is_open(), print an error to
    //      std::cerr and return false.
    //   2. ++filesRead_; lastSeconds_ = 0;
    //   3. Loop:  while (std::getline(in, line))
    //        ++linesRead_
    //        strip a trailing '\r' if present
    //        LogEntry e(line);  if (!e.isValid()) { ++malformed_; continue; }
    //        lastSeconds_ = e.seconds();
    //        handle(e, date);
    //   4. After the loop: call endOfDay(lastSeconds_) on EVERY player.
    //   5. return true
    std::ifstream file(path);
    if (!file.is_open()){
        std::cerr << "Error: file could not be opened :/"; 
        return false;
    }
    ++filesRead_; lastSeconds_ = 0;
    std::string line;
    while(std::getline(file, line)){
        ++linesRead_; 
        // handle /r
        if (!line.empty() && line.back() == '\r'){
            line.pop_back();
        }
        LogEntry e(line);
        if(!e.isValid()){ ++malformed_; continue;}
        lastSeconds_ = e.seconds();
        handle(e, date);
    }
    for(size_t i = 0; i < players_.size(); ++i){
        players_[i].endOfDay(lastSeconds_);
    }
    return true;
}

void ServerLog::handle(const LogEntry& e, const std::string& date) {
    // TODO (Task 4): decide what kind of line this is and update the right Player.
    //
    //   - if level is WARN or ERROR: ++warnings_  (then keep going; it may still matter)
    //   - "UUID of player NAME is ...": push NAME onto knownNames_, return
    //   - e.isPlayerChat(): player(e.chatSender()).recordChat(), return
    //   - e.isLogin():  parse   NAME[/0.0.0.0:0] logged in with entity id N at ([minecraft:DIM]X, Y, Z)
    //         name      = text before the first '['
    //         dimension = text between "[minecraft:" and the next ']'
    //         coords    = text between that ']' and the next ')'  ->  "X, Y, Z"
    //         replace the commas with spaces and read three doubles with std::istringstream
    //         player(name).recordLogin(...);  logins_.push_back({...});  return
    //   - e.isSystemChat(): handleSystemChat(e, e.systemChatText()), return
    //   - anything else: ignore
    if(e.level() == "WARN" || e.level() == "ERROR"){
        ++warnings_;
    }


    if(e.message().compare(0, 4, "UUID") == 0){
        size_t end = e.message().find(" is "); 
        if (end != std::string::npos){
            std::string name = e.message().substr(15, end - 15);
            knownNames_.push_back(name);
            return;
        }
    }

    if(e.isPlayerChat()){
        player(e.chatSender()).recordChat(); 
        return;
    }
    if(e.isLogin()){
        size_t open = e.message().find("["); 
        std::string name = e.message().substr(0, open); 
        size_t open2 = e.message().find("[minecraft:"); 
        size_t close2 = e.message().find("]", open2);
        std::string dimension = e.message().substr(open2 + 11, close2 - (open2 + 11));
        size_t close3 = e.message().find(")");
        std::string coords = e.message().substr(close2 + 1, close3 - (close2 + 1));
        for(size_t i = 0; i < coords.size();++i){
            if (coords[i] == ','){
                coords[i] = ' '; 
            }
        }
        std::istringstream coordi(coords); 
        double x, y, z;
        coordi >> x >> y >> z; 
        player(name).recordLogin(dimension, x, y, z);
        logins_.push_back({name, date, e.timeText(), dimension, x, y, z});
        return; 
    }
    if (e.isSystemChat()){
        handleSystemChat(e, e.systemChatText()); 
        return;
    }
}

void ServerLog::handleSystemChat(const LogEntry& e, const std::string& text) {
    // TODO (Task 4): 'text' is what comes after "System chat: ". Cases:
    //   "NAME joined the game"                 -> player(NAME).recordJoin(e.seconds())
    //   "NAME left the game"                   -> player(NAME).recordLeave(e.seconds())
    //   "NAME has made the advancement [..]"   -> recordAdvancement()
    //   "NAME has completed the challenge [..]"-> recordAdvancement()
    //   "NAME has reached the goal [..]"       -> recordAdvancement()
    //   otherwise, if the FIRST WORD is a known player (isKnownPlayer), it's a
    //   death message: "NAME was slain by Zombie" -> recordDeath("was slain by Zombie")
    //   otherwise ignore ("Added X to the whitelist", "That player does not exist", ...)

    std::size_t end = text.find(" joined the game");

    if (end != std::string::npos) {
        std::string name = text.substr(0, end);
        player(name).recordJoin(e.seconds());
        return;
    }

    end = text.find(" left the game");

    if (end != std::string::npos) {
        std::string name = text.substr(0, end);
        player(name).recordLeave(e.seconds());
        return;
    }

    end = text.find(" has made the advancement");
    if (end != std::string::npos) {
        std::string name = text.substr(0, end);
        player(name).recordAdvancement();
        return;
    }

    
    end = text.find(" has completed the challenge");
    if (end != std::string::npos) {
        std::string name = text.substr(0, end);
        player(name).recordAdvancement();
        return;
    }

    end = text.find(" has reached the goal");
    if (end != std::string::npos) {
        std::string name = text.substr(0, end);
        player(name).recordAdvancement();
        return;
    }

    size_t space = text.find(' '); 
    if(space != std::string::npos) {
        std::string name = text.substr(0, space);
        if(isKnownPlayer(name)){
            std::string cause = text.substr(space + 1); 
            player(name).recordDeath(cause);
        }
    }
}

Player& ServerLog::player(const std::string& name) {
    // TODO (Task 4): find-or-create. Loop over players_; if one has this name,
    // return a reference to it. Otherwise push_back(Player(name)) and return
    // players_.back().
    // Careful: a reference or pointer into a vector is only valid until the next
    // push_back. Use the returned reference right away; don't keep it around.

    for (size_t i = 0; i < players_.size(); ++i){
        if (players_[i].name() == name){
            return players_[i];
        }
    }
    players_.push_back(Player(name)); 
    return players_.back();
}

const Player* ServerLog::findPlayer(const std::string& name) const {
    // TODO (Task 4): return a pointer to the player with this name, or nullptr
    for (size_t i = 0; i < players_.size(); ++i){
        if (players_[i].name() == name){
            return &players_[i];
        }
    }
    return nullptr;
}

bool ServerLog::isKnownPlayer(const std::string& name) const {
    // TODO (Task 4): true if name is in knownNames_ (std::find, <algorithm>)
    // or findPlayer(name) is not nullptr
    
    auto it = std::find(knownNames_.begin(), knownNames_.end(), name); 
    if (it != knownNames_.end() || findPlayer(name) != nullptr){
        return true;
    }
    return false;
}

// ---------------------------------------------------------------- results

std::vector<CauseCount> ServerLog::deathCauseTotals() const {
    // TODO (Task 5): merge every player's deathCauses() into one vector<CauseCount>
    // (find-or-append again), then std::sort it by count, largest first.
    std::vector<CauseCount> totals;
    for (std::size_t i = 0; i < players_.size(); ++i){
        for (std::size_t j = 0; j < players_[i].deathCauses().size(); ++j){
            CauseCount  current = players_[i].deathCauses()[j];
            bool found = false; 
            for(std::size_t k = 0; k < totals.size(); ++k){
                if (totals[k].cause == current.cause) {
                    totals[k].count += current.count; 
                    found = true;
                    break;
                }
            }
            if(!found){
                totals.push_back(current);
            }
        }
    }
    std::sort(totals.begin(), totals.end(), [](const CauseCount& a, const CauseCount& b) {
        return a.count > b.count;
    }); 
    return totals;
}

std::vector<const Player*> ServerLog::playersByTimeOnline() const {
    // TODO (Task 5): collect a pointer to every Player (&p in a range-for over
    // players_), then std::sort them by secondsOnline(), largest first.
    // Hint: a lambda comparator.
    std::vector<const Player*> players_ps; 
    for (std::size_t i = 0; i < players_.size(); ++i) {
        players_ps.push_back(&players_[i]);
    }

    std::sort(players_ps.begin(), players_ps.end(), [](const Player* a, const Player* b) {
        return a->secondsOnline() > b->secondsOnline();
    }); 


    return players_ps;
}
std::vector<const Player*> ServerLog::playersByLongestSession() const {
    std::vector<const Player*> sortedPlayers;

    for (std::size_t i = 0; i < players_.size(); ++i) {
        sortedPlayers.push_back(&players_[i]);
    }

    std::sort(sortedPlayers.begin(), sortedPlayers.end(),
        [](const Player* a, const Player* b) {
            return a->longestSessionSeconds() > b->longestSessionSeconds();
        });

    return sortedPlayers;
}

// ---------------------------------------------------------------- output

bool ServerLog::writeReport(const std::string& path) const {
    // TODO (Task 5): open an std::ofstream and write the report. Required sections:
    //   header (files, lines, malformed, warnings, player count)
    //   leaderboard table (use std::setw / std::left / std::right so columns line up)
    //   causes of death, sorted by count
    //   superlatives: chattiest player, most deaths
    //   players still online when the last log ended
    //   last known position of every player (std::fixed, std::setprecision(1))

    std::ofstream report(path); 
    if (!report.is_open()){
        std::cerr << "Cannot open report file"; 
        return false;
    }
    report << "SUMMARY\n";
    report << "Files read: " << filesRead_ << '\n';
    report << "Lines read: " << linesRead_ << '\n';
    report << "Malformed lines: " << malformed_ << '\n';
    report << "Warnings: " << warnings_ << '\n';
    report << "Players: " << players_.size() << "\n\n";

    // LEADERBOARD
    std::vector<const Player*> sortedPlayers = playersByTimeOnline();

    report << "\nLEADERBOARD\n";
    report << std::left << std::setw(20) << "Player"
        << std::right << std::setw(15) << "Time online"
        << '\n';

    for (std::size_t i = 0; i < sortedPlayers.size(); ++i) {
        report << std::left << std::setw(20) << sortedPlayers[i]->name()
            << std::right << std::setw(15) << sortedPlayers[i]->timeOnlineText()
            << '\n';
    }

    // CAUSES OF DEATH 
    report << "\nCAUSES OF DEATH\n";

    std::vector<CauseCount> causes = deathCauseTotals();

    for (std::size_t i = 0; i < causes.size(); ++i) {
        report << causes[i].cause << ": "
            << causes[i].count << "\n";
    }

    // SUPERLATIVES
    const Player* chattiest = nullptr;
    const Player* mostDeaths = nullptr;

    for (std::size_t i = 0; i < players_.size(); ++i) {
        const Player* current = &players_[i];

        if (chattiest == nullptr || current->chats() > chattiest->chats()) {
            chattiest = current;
        }

        if (mostDeaths == nullptr || current->deaths() > mostDeaths->deaths()) {
            mostDeaths = current;
        }
    }

    report << "\nSUPERLATIVES\n";

    if (chattiest != nullptr) {
        report << "Chattiest player: " << chattiest->name()
            << " (" << chattiest->chats() << " chats)\n";
    }

    if (mostDeaths != nullptr) {
        report << "Most deaths: " << mostDeaths->name()
            << " (" << mostDeaths->deaths() << " deaths)\n";
    }

    // PLAYERS STILL ONLINE
    report << "\nPLAYERS STILL ONLINE\n";
    for (std::size_t i = 0; i < players_.size(); ++i){
        if(players_[i].isOnline()){
            report << players_[i].name() << '\n';
        }
    }



    // LAST POSITIONS   
    report << "\nLAST KNOWN POSITIONS\n";
    report << std::fixed << std::setprecision(1); 

    for (std::size_t i = 0; i < players_.size(); ++i){
        report << players_[i].name() << ": ("
       << players_[i].lastX() << ", "
       << players_[i].lastY() << ", "
       << players_[i].lastZ() << ")\n";
    }

    // LONGEST SESSIONS 
    report << "\nLONGEST SESSIONS\n";

    std::vector<const Player*> longestSessions = playersByLongestSession();

    for (std::size_t i = 0; i < longestSessions.size(); ++i) {
        report << longestSessions[i]->name() << ": "
            << longestSessions[i]->longestSessionSeconds()
            << " seconds\n";
    }   

    return true;
}

bool ServerLog::writeLoginsCsv(const std::string& path) const {
    // TODO (Task 6): write "player,date,time,dimension,x,y,z" then one line per LoginRecord.
    // Use std::fixed << std::setprecision(2) so the numbers are tidy.

    std::ofstream logins_csv(path); 
    if (!logins_csv.is_open()){
        std::cerr << "Cannot open report file"; 
        return false;
    }
    logins_csv << "player,date,time,dimension,x,y,z\n";
    logins_csv << std::fixed << std::setprecision(2);
    for(std::size_t i = 0; i < logins_.size(); ++i){
        logins_csv << logins_[i].player << "," << logins_[i].date << "," 
        << logins_[i].time << "," << logins_[i].dimension << "," 
        << logins_[i].x << "," << logins_[i].y << "," << logins_[i].z << "\n";
    }
    return true;
}

