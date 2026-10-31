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
    load(path);
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
    std::ifstream mcin(path); 
    if (!mcin.is_open()){
        std::cerr << "Error: could not open file '" << path << "'for reading." << std::endl;
        return false;
    }
    lines_.clear();
    path_ = path; 
    std::string lineText; 
    while(std::getline(mcin, lineText)){
        Line line; 
        line.raw = lineText; 

        // Strip trialing '\r (Windows line endings)
        if (!lineText.empty() && lineText.back() == '\r'){
            lineText.pop_back();
        }

        // handle comment lines and blank lines
        if (lineText.empty() || lineText[0] == '#' ){
            lines_.push_back(line); 
            continue;
        }

        // handle setting lines 
        std::size_t eqSign = lineText.find('=');
        if (eqSign == std::string::npos) {
            lines_.push_back(line); 
            continue;
        }

        line.isSetting = true; 
        line.key = lineText.substr(0, eqSign); 
        line.value = lineText.substr(eqSign + 1); 
        lines_.push_back(line);
    }
    
    mcin.close();
    return true;
}

// ---------------------------------------------------------------- file output

bool ServerConfig::save(const std::string& path) const {
    // TODO: open an std::ofstream; if it fails return false.
    // For each line: settings as key=value, everything else as raw. '\n' after each.
    std::ofstream out(path); // truncate existing file 
    if(!out.is_open()){
        std::cerr << "ServerConfig could not write to " << path << std::endl; 
        return false;
    }
    for (const Line& line: lines_) {
        if(line.isSetting) {
            out << line.key << "=" << line.value << std::endl;
        }  else {
            out <<  line.raw << std::endl;
        }
    }
    return true;
}

bool ServerConfig::save() const {
    if (path_.empty()) {
        std::cerr << "no file path to save to" << std::endl;
        return false;
    }
    return save(path_);
}


// ---------------------------------------------------------------- lookups

int ServerConfig::findKey(const std::string& key) const {
    // TODO: index of the setting line with this key, or -1
    for (std::size_t i = 0; i < lines_.size(); i++) {
        if (lines_[i].isSetting && lines_[i].key == key) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

bool ServerConfig::has(const std::string& key) const {
    // TODO
    return findKey(key) != -1;
}

std::string ServerConfig::get(const std::string& key, const std::string& fallback) const {
    // TODO
    if(findKey(key) != -1) {
        return lines_.at(findKey(key)).value;
    }
    return fallback;
}

int ServerConfig::getInt(const std::string& key, int fallback) const {
    // TODO: std::stoi inside try/catch; empty value or missing key -> fallback
    if(findKey(key) == -1 || lines_.at(findKey(key)).value.empty()){
        return fallback; 
    }
    try{
        return std::stoi(lines_.at(findKey(key)).value); 
    } catch (...) {
        return fallback;
    }
}

bool ServerConfig::getBool(const std::string& key, bool fallback) const {
    // TODO: "true" -> true, "false" -> false, anything else -> fallback
    std::string val = get(key, ""); 
    if (val.empty()) {
        return fallback;
    }
    if (val == "true") {
        return true;
    }
    if (val == "false") {
        return false;
    }
    return fallback;
}

// ---------------------------------------------------------------- setters

void ServerConfig::set(const std::string& key, const std::string& value) {
    // TODO: change in place if the key exists (order and comments survive); else append
    if (findKey(key) != -1) {
        lines_.at(findKey(key)).value = value; 
        return;
    }
    Line line; 
    line.isSetting = true; 
    line.key = key;
    line.value = value;
    lines_.push_back(line);

}

void ServerConfig::set(const std::string& key, const char* value) {
    // Without this overload, set("motd", "hello") would call set(key, bool)!
    // TODO: forward to the std::string version
    set(key, std::string(value));
}

void ServerConfig::set(const std::string& key, int value) {
    // TODO: std::to_string
    set(key, std::to_string(value));
}

void ServerConfig::set(const std::string& key, bool value) {
    // TODO: "true" / "false"
    set(key, std::string(value ? "true" : "false"));
}

// ---------------------------------------------------------------- info

int ServerConfig::settingCount() const {
    int count = 0; 
    for (const Line& line : lines_) {
        if(line.isSetting){
            count++;
        }
    }
    return count;
}

void ServerConfig::print() const {
    // TODO: key = value for every setting, std::left << std::setw(40) << key

    for (const Line& line : lines_) {
        if(line.isSetting){
            std::cout << std::left << std::setw(40) << line.key << "=" << line.value << std::endl;
        }
    }

}