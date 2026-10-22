// ServerConfig.cpp -- STARTER. Implement every function. See README.md for the order.
#include "ServerConfig.h"

#include <fstream>    // std::ifstream, std::ofstream
#include <iostream>   // std::cout, std::cerr
#include <iomanip>    // std::setw, std::left

// ---------------------------------------------------------------- constructors

ServerConfig::ServerConfig() {
    // nothing to do: lines_ and path_ start empty
}

ServerConfig::ServerConfig(const std::string& path) {
    // TODO: load the file (ignore the result; the caller can check settingCount())
    (void)path;
}

// ---------------------------------------------------------------- file input

bool ServerConfig::load(const std::string& path) {
    // TODO:
    //   open an std::ifstream; if !is_open(): print an error to std::cerr, return false
    //   lines_.clear(); path_ = path;
    //   while (std::getline(in, text)):
    //       Line line; line.raw = text;
    //       strip a trailing '\r'
    //       empty or starts with '#'  -> push_back(line); continue;
    //       eq = text.find('=');  npos -> push_back(line); continue;   (junk line, kept verbatim)
    //       line.isSetting = true; line.key = text.substr(0, eq); line.value = text.substr(eq + 1);
    //       push_back(line)
    //   return true
    (void)path;
    return false;
}

// ---------------------------------------------------------------- file output

bool ServerConfig::save(const std::string& path) const {
    // TODO: open an std::ofstream; if it fails return false.
    // For each line: settings as key=value, everything else as raw. '\n' after each.
    (void)path;
    return false;
}

bool ServerConfig::save() const {
    // TODO: if path_ is empty, print an error and return false; else return save(path_)
    return false;
}

// ---------------------------------------------------------------- lookups

int ServerConfig::findKey(const std::string& key) const {
    // TODO: index of the setting line with this key, or -1
    (void)key;
    return -1;
}

bool ServerConfig::has(const std::string& key) const {
    // TODO
    (void)key;
    return false;
}

std::string ServerConfig::get(const std::string& key, const std::string& fallback) const {
    // TODO
    (void)key;
    return fallback;
}

int ServerConfig::getInt(const std::string& key, int fallback) const {
    // TODO: std::stoi inside try/catch; empty value or missing key -> fallback
    (void)key;
    return fallback;
}

bool ServerConfig::getBool(const std::string& key, bool fallback) const {
    // TODO: "true" -> true, "false" -> false, anything else -> fallback
    (void)key;
    return fallback;
}

// ---------------------------------------------------------------- setters

void ServerConfig::set(const std::string& key, const std::string& value) {
    // TODO: change in place if the key exists (order and comments survive); else append
    (void)key; (void)value;
}

void ServerConfig::set(const std::string& key, const char* value) {
    // Without this overload, set("motd", "hello") would call set(key, bool)!
    // TODO: forward to the std::string version
    (void)key; (void)value;
}

void ServerConfig::set(const std::string& key, int value) {
    // TODO: std::to_string
    (void)key; (void)value;
}

void ServerConfig::set(const std::string& key, bool value) {
    // TODO: "true" / "false"
    (void)key; (void)value;
}

// ---------------------------------------------------------------- info

int ServerConfig::settingCount() const {
    // TODO
    return 0;
}

void ServerConfig::print() const {
    // TODO: key = value for every setting, std::left << std::setw(40) << key
}
