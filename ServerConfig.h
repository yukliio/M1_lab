// ServerConfig.h
// A class that wraps a Minecraft server.properties file.
//
// server.properties looks like this:
//     #Minecraft server properties          <- comment line (starts with #)
//     #Fri Oct 16 14:08:24 UTC 2026         <- also a comment, even though it has a date in it
//     max-players=20                        <- a setting: key=value
//     motd=A Minecraft Server               <- values can contain spaces
//     level-seed=                           <- values can be empty
//     query.port=19000                      <- keys can contain dots
//     level-type=minecraft\:normal          <- Java escapes ':' in values as '\:'
//
// The class's job: load the file, let you read/change settings, save it back
// WITHOUT destroying the comments or the order of the lines.

#pragma once            // "only include this header once per .cpp file"

#include <string>
#include <vector>

class ServerConfig {
public:
    // ---- constructors ----
    ServerConfig();                                   // empty config
    explicit ServerConfig(const std::string& path);   // loads the file right away

    // ---- file I/O ----
    bool load(const std::string& path);   // returns false if the file can't be opened
    bool save(const std::string& path) const;
    bool save() const;                    // saves back to the file it was loaded from

    // ---- reading settings ----
    bool        has(const std::string& key) const;
    std::string get(const std::string& key, const std::string& fallback = "") const;
    int         getInt(const std::string& key, int fallback = 0) const;
    bool        getBool(const std::string& key, bool fallback = false) const;

    // ---- changing settings ----
    void set(const std::string& key, const std::string& value);  // adds the key if missing
    void set(const std::string& key, const char* value);         // see "The bool trap" in the handout
    void set(const std::string& key, int value);
    void set(const std::string& key, bool value);

    // ---- info ----
    int  settingCount() const;
    void print() const;   // dumps every setting to std::cout

private:
    // One line of the file. Either a setting (key=value) or something else
    // (a comment or a blank line) that we keep verbatim so save() can
    // reproduce the file exactly.
    struct Line {
        bool        isSetting = false;
        std::string key;     // only used when isSetting
        std::string value;   // only used when isSetting
        std::string raw;     // the original text, used for comments / blanks
    };

    std::vector<Line> lines_;   // the whole file, in order
    std::string       path_;    // where it came from, so save() works

    // helper: find the index of a setting line, or -1
    int findKey(const std::string& key) const;
};
