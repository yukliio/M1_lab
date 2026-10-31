// LogEntry.cpp -- STARTER. Fill in every function marked TODO.
#include "LogEntry.h"
#include <iostream>
#include <string>

LogEntry::LogEntry() {}

LogEntry::LogEntry(const std::string& rawLine) : raw_(rawLine) {
    // check timestamp formatting
    if((raw_.size() < 12) || (raw_[0] != '[') || (raw_[9] != ']')){
        valid_ = false;
        return;
    }

    // extract timestamp
    time_ = raw_.substr(1, 8);
    seconds_ = parseTime(time_);
    if (seconds_ < 0){
        return;
    } 

    // define indecies 

    std::size_t open = raw_.find('[', 10);
    if (open == std::string::npos){
        return;
    }

    std::size_t slash = raw_.find('/', open); 
    if(slash == std::string::npos){
        return;
    }

    std::size_t close = raw_.find("]: ", slash); 
    if(close == std::string::npos){
        return;
    }

    // assign thread, level, and message
    thread_ = raw_.substr(open + 1, slash - open - 1); 
    level_ = raw_.substr(slash + 1, close - slash - 1); 
    message_ = raw_.substr(close + 3);
    valid_ = true;

    // std::cout << "Time: " << time_ << std::endl;
    // std::cout << "Seconds: " << seconds_ << std::endl;
    // std::cout << "Thread: " << thread_ << std::endl;
    // std::cout << "Level: " << level_ << std::endl;
    // std::cout << "Message: " << message_ << std::endl;
    // std::cout << "Valid: " << valid_ << std::endl;
}

int LogEntry::parseTime(const std::string& t) {
    // TODO (Task 1): "HH:MM:SS" -> seconds since midnight, or -1 if malformed.
    // Check: size is 8, t[2] and t[5] are ':', the other six are digits,
    // hours <= 23, minutes/seconds <= 59.   Hint: std::stoi(t.substr(0, 2))
    if (( t.size() == 8) && (t[2] == ':') && (t[5] == ':')) {
        for (std::size_t i = 0; i < t.size(); ++i){
            if (i != 2 && i != 5){
                if(t[i] > '9' || t[i] < '0'){
                    return -1;
                }
            }
        }

        int hours = std::stoi(t.substr(0, 2));
        int minutes = std::stoi(t.substr(3, 2));
        int seconds = std::stoi(t.substr(6, 2));

        if (hours > 23 || minutes > 59 || seconds > 59){
            return -1;
        }

        return (hours * 60 + minutes) * 60 + seconds;
    }
    return -1;
}

// ---- classifiers -------------------------------------------------------------

bool LogEntry::isSystemChat() const {
    // TODO (Task 2): true if valid and message_ starts with "System chat: "
    if (valid_ && message_.substr(0, 13) == "System chat: ") {
        return true;
    }
    return false;
}

std::string LogEntry::systemChatText() const {
    // TODO (Task 2): the text after "System chat: " (or "" if not a system chat)
    if (isSystemChat()){
        return message_.substr(13);
    }
    return "";
}

bool LogEntry::isPlayerChat() const {
    // TODO (Task 2): true if thread_ starts with "Async Chat Thread" and message_ starts with '<'
    if(thread_.compare(0, 17, "Async Chat Thread") == 0 && message_.compare(0, 1, "<") == 0){
        return true;
    }
    return false;
}

std::string LogEntry::chatSender() const {
    // TODO (Task 2): for "<Steve> hello", return "Steve"
    if (isPlayerChat()){
        size_t close = message_.find(">");

        if(close != std::string::npos){
            return message_.substr(1, close - 1);
        }
    }
    return "";
}

std::string LogEntry::chatText() const {
    // TODO (Task 2): for "<Steve> hello there", return "hello there"
    if (isPlayerChat()) {
        size_t close = message_.find(">");
        if(close != std::string::npos){
            return message_.substr(close + 2);
        }
    }
    return "";
}

bool LogEntry::isLogin() const {
    // TODO (Task 2): true if message_ contains "] logged in with entity id "
    if (message_.find("] logged in with entity id ") != std::string::npos){
        return true;
    }
    return false;
}

bool LogEntry::isCommand() const {
    // TODO (Task 2): true if message_ contains " issued server command: "
    if (message_.find(" issued server command: ") != std::string::npos){
        return true;
    }
    return false;
}
