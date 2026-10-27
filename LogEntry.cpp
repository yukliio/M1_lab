// LogEntry.cpp -- STARTER. Fill in every function marked TODO.
#include "LogEntry.h"

LogEntry::LogEntry() {}

LogEntry::LogEntry(const std::string& rawLine) : raw_(rawLine) {
    // TODO (Task 1): parse the line.
    //
    //   [14:03:22] [Server thread/INFO]: System chat: Steve joined the game
    //   0        9 ^open      ^slash  ^close
    //
    // Steps:
    //   1. If the line is shorter than 12 chars, or raw_[0] != '[' or raw_[9] != ']',
    //      it has no timestamp -> leave valid_ = false and return.
    //   2. time_ = the 8 characters between the first [ ]. seconds_ = parseTime(time_).
    //      If seconds_ < 0, return.
    //   3. Find the second '[' (search from index 10), then the '/' after it,
    //      then the "]: " after that. If any is std::string::npos, return.
    //   4. thread_  = text between '[' and '/'
    //      level_   = text between '/' and ']'
    //      message_ = everything after "]: "
    //   5. valid_ = true
}

int LogEntry::parseTime(const std::string& t) {
    // TODO (Task 1): "HH:MM:SS" -> seconds since midnight, or -1 if malformed.
    // Check: size is 8, t[2] and t[5] are ':', the other six are digits,
    // hours <= 23, minutes/seconds <= 59.   Hint: std::stoi(t.substr(0, 2))
    (void)t;
    return -1;
}

// ---- classifiers -------------------------------------------------------------

bool LogEntry::isSystemChat() const {
    // TODO (Task 2): true if valid and message_ starts with "System chat: "
    return false;
}

std::string LogEntry::systemChatText() const {
    // TODO (Task 2): the text after "System chat: " (or "" if not a system chat)
    return "";
}

bool LogEntry::isPlayerChat() const {
    // TODO (Task 2): true if thread_ starts with "Async Chat Thread" and message_ starts with '<'
    return false;
}

std::string LogEntry::chatSender() const {
    // TODO (Task 2): for "<Steve> hello", return "Steve"
    return "";
}

std::string LogEntry::chatText() const {
    // TODO (Task 2): for "<Steve> hello there", return "hello there"
    return "";
}

bool LogEntry::isLogin() const {
    // TODO (Task 2): true if message_ contains "] logged in with entity id "
    return false;
}

bool LogEntry::isCommand() const {
    // TODO (Task 2): true if message_ contains " issued server command: "
    return false;
}
