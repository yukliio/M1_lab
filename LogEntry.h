// LogEntry.h
//
// A normal line looks like:
//   [14:03:22] [Server thread/INFO]: System chat: Steve joined the game
//   ^time      ^thread       ^level  ^message
//
// Some lines are NOT normal (a stack trace, or someone's typo in the console),
// so a LogEntry can be invalid. Always check isValid() before using the parts.
#pragma once
#include <string>

class LogEntry {
    //private by default... this is a class
    bool        valid_ = false;
    std::string raw_;
    std::string time_;
    int         seconds_ = -1;
    std::string thread_;
    std::string level_;
    std::string message_;
public:
    LogEntry();                              // an invalid, empty entry
    explicit LogEntry(const std::string& rawLine);   // parses the line

    bool isValid() const { return valid_; }

    // the pieces
    const std::string& timeText() const { return time_; }     // "14:03:22"
    int                seconds()  const { return seconds_; }  // 14*3600 + 3*60 + 22
    const std::string& thread()   const { return thread_; }   // "Server thread"
    const std::string& level()    const { return level_; }    // "INFO", "WARN", "ERROR"
    const std::string& message()  const { return message_; }  // everything after ": "
    const std::string& raw()      const { return raw_; }

    // convenience classifiers (all based on message_/thread_)
    bool isSystemChat() const;     // message starts with "System chat: "
    std::string systemChatText() const;   // the part after "System chat: "
    bool isPlayerChat() const;     // "<name> text" on an Async Chat Thread
    std::string chatSender() const;
    std::string chatText() const;
    bool isLogin() const;          // "Name[/ip] logged in with entity id ... at (...)"
    bool isCommand() const;        // "Name issued server command: /..."

    // Convert "HH:MM:SS" to seconds since midnight. Returns -1 if malformed.
    static int parseTime(const std::string& hhmmss);
};
