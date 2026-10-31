// Player.cpp -- STARTER. Fill in every function marked TODO.
#include "Player.h"
#include <sstream>
#include <iomanip>
#include <vector>

Player::Player() {}
Player::Player(const std::string& name) : name_(name) {}

void Player::recordJoin(int seconds) {
    // TODO (Task 3): count the join, mark online, remember when the session started
    joins_++; 
    online_ = true; 
    sessionStart_ = seconds;
}

void Player::recordLeave(int seconds) {
    // TODO (Task 3): if online, add (seconds - sessionStart_) to secondsOnline_.
    // If NOT online, this is a "left" with no "joined" in this file: the player
    // was on the server when the day started (log rolled over at midnight).
    // Credit them from 00:00:00, i.e. add 'seconds'.
    // Either way, mark them offline.

    if(online_){
        secondsOnline_ += seconds - sessionStart_;
    } else {
        secondsOnline_ += seconds;
    }
    online_ = false;
}

void Player::recordChat()        { /* TODO (Task 3) */  chats_++;}
void Player::recordAdvancement() { /* TODO (Task 3) */ advancements_++;}

void Player::recordDeath(const std::string& cause) {
    // TODO (Task 3): count the death AND count this cause in deathCauses_.
    // deathCauses_ is a vector<CauseCount>. Loop over it looking for an entry
    // whose .cause == cause; if found, ++count and return. Otherwise push_back
    // a new CauseCount{cause, 1}. (Same shape as ServerConfig::findKey in the demo.)
    deaths_++; 
    for(size_t i = 0; i < deathCauses_.size(); ++i){
        if (deathCauses_[i].cause == cause){
            deathCauses_[i].count++;
            return;
        }
    }
    deathCauses_.push_back(CauseCount{cause, 1});
}

int Player::deathsBy(const std::string& cause) const {
    // TODO (Task 3): return the count for this cause, or 0 if it never happened
    for(size_t i = 0; i < deathCauses_.size(); ++i){
        if (deathCauses_[i].cause == cause){
            return deathCauses_[i].count;
        }
    }
    return 0;
}

void Player::recordLogin(const std::string& dimension, double x, double y, double z) {
    // TODO (Task 3): remember the position and dimension; set hasPos_ = true
    x_ = x; y_ = y; z_ = z; dimension_ = dimension; 
    hasPos_ = true; 
}

void Player::endOfDay(int lastSeconds) {
    // TODO (Task 4): the file ended. If this player is still online, credit
    // them up to lastSeconds, then set sessionStart_ = 0 so that tomorrow's
    // "left the game" counts from midnight. Leave online_ as it is.
    if (online_){
        secondsOnline_ += lastSeconds - sessionStart_;
        sessionStart_ = 0; 
    }
}

std::string Player::timeOnlineText() const {
    // TODO (Task 5): return e.g. "3h 04m". Hint: std::ostringstream + std::setw(2) + std::setfill('0')
    int hours = secondsOnline_ / 3600; 
    int minutes = (secondsOnline_ % 3600) / 60;

    std::ostringstream out; 
    out << hours << "h " << std::setfill('0') << std::setw(2) << minutes << "m";
    return out.str();
}
