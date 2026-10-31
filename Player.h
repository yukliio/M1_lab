// Player.h -- everything we know about one player, built up from log events.
#pragma once
#include <string>
#include <vector>

// How many times one thing happened: "was slain by Zombie" -> 3.
// A vector of these is our counting table (find the cause; if missing, append it).
struct CauseCount {
    std::string cause;
    int count = 0;
};

class Player {
    std::string name_;
    int joins_ = 0, chats_ = 0, deaths_ = 0, advancements_ = 0;
    int secondsOnline_ = 0;

    // session tracking
    bool online_ = false;
    int  sessionStart_ = 0;      // seconds since midnight when this session began

    std::vector<CauseCount> deathCauses_;   // one entry per distinct cause

    bool hasPos_ = false;
    double x_ = 0, y_ = 0, z_ = 0;
    std::string dimension_;

    int activeSessionSeconds_ = 0;
    int longestSessionSeconds_ = 0;

public:
    Player();                                 // needed so std::vector<Player> can make empty slots
    explicit Player(const std::string& name);

    const std::string& name() const { return name_; }

    // ---- events (called by ServerLog as it reads the log) ----
    void recordJoin(int seconds);
    void recordLeave(int seconds);
    void recordChat();
    void recordDeath(const std::string& cause);      // e.g. "slain by Zombie"
    void recordAdvancement();
    void recordLogin(const std::string& dimension, double x, double y, double z);
    void endOfDay(int lastSeconds);  // the log file ended; close any open session

    // ---- stats ----
    int  joins()        const { return joins_; }
    int  chats()        const { return chats_; }
    int  deaths()       const { return deaths_; }
    int  advancements() const { return advancements_; }
    int  secondsOnline() const { return secondsOnline_; }
    bool isOnline()     const { return online_; }
    const std::vector<CauseCount>& deathCauses() const { return deathCauses_; }
    int  deathsBy(const std::string& cause) const;   // 0 if never


    bool hasPosition() const { return hasPos_; }
    double lastX() const { return x_; }
    double lastY() const { return y_; }
    double lastZ() const { return z_; }
    const std::string& lastDimension() const { return dimension_; }
    int longestSessionSeconds() const {return longestSessionSeconds_;}

    // "3h 04m" style text for the report
    std::string timeOnlineText() const;


};
