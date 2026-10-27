# Lab Part 1: Server Log Analyzer

## What you're building

A program that reads the real log files from our class Minecraft server and answers questions the server itself never does: who has played the most, who dies the most (and how), who talks the most, and where everyone's base is.

You will write three classes that work together:

| Class | Represents | Owns |
| --- | --- | --- |
| `LogEntry` | one line of the log, split into time / thread / level / message | strings and an `int` |
| `Player` | one player's running statistics | counters, a session timer, a `vector` of death-cause counts |
| `ServerLog` | the whole analysis: reads files, creates `LogEntry`s, updates `Player`s | a `vector<Player>` and a `vector<LoginRecord>` |

Your finished program reads seven log files (six full days plus today's partial log) and writes two files: `report.txt`, a formatted leaderboard, and `logins.csv`, the coordinates of every login. Part 2 of this lab turns `logins.csv` into a heatmap drawn over a map of the world, so get it right.

**The data.** The `data/logs/` folder holds `2026-10-16-1.log` through `2026-10-21-1.log` and `latest.log`. These are the genuine files from the server with one change: player IP addresses have been replaced by `0.0.0.0:0`. The usernames are real and so is everything they said and did. 3,089 lines, 23 players, 64 deaths.

**What you need before starting.** You should be comfortable with `std::string`, `std::vector`, `if`/`for`/`while`, and functions. Classes are explained in the next section; the only containers used are `std::string` and `std::vector`. You do not need to have used `fstream` before.

**Files you get.** This repository contains the three headers (`LogEntry.h`, `Player.h`, `ServerLog.h`), a finished `main.cpp`, and three `.cpp` files full of `TODO`s. The headers are the design: they are complete and you should not need to change them. Your job is the `.cpp` files. Build with CLion (open the folder; it has a `CMakeLists.txt`), with `make`, or from a terminal:

```
g++ -std=c++20 -Wall -Wextra -Werror -pedantic -O2 -o loganalyzer main.cpp LogEntry.cpp Player.cpp ServerLog.cpp
./loganalyzer data/logs/2026-10-16-1.log data/logs/2026-10-17-1.log data/logs/2026-10-18-1.log data/logs/2026-10-19-1.log data/logs/2026-10-20-1.log data/logs/2026-10-21-1.log data/logs/latest.log
```

Those are the course flags: C++20, all warnings on, and `-Werror`, which turns every warning into a compile error. Unused variables, signed/unsigned comparisons and missing returns will stop your build; that is intended. The starter compiles cleanly under them on day one. It just prints `FAILED` for every file until you implement `processFile`.

## Concepts you need

Read this section once before you start, then come back to it as you hit each task. Everything here is used somewhere in the lab.

### Classes: why, and the parts of one

A class bundles data with the functions that are allowed to touch that data. Without one, a "player" is six loose variables that every function in your program can accidentally corrupt. With one, it is a single thing with a known set of operations.

```cpp
// Player.h -- the DECLARATION: what a Player is and what it can do
class Player {
public:                                   // anyone can call these
    Player();                             // default constructor
    explicit Player(const std::string& name);
    void recordJoin(int seconds);         // a "mutator": changes the object
    int  joins() const { return joins_; } // a "getter": const promises it changes nothing
private:                                  // only Player's own functions can touch these
    std::string name_;
    int joins_ = 0;                       // default member initializer
    bool online_ = false;
};
```

```cpp
// Player.cpp -- the DEFINITION: the code behind each function
#include "Player.h"
Player::Player(const std::string& name) : name_(name) {}   // member initializer list
void Player::recordJoin(int seconds) {
    ++joins_;
    online_ = true;
}
```

- **`public:` / `private:`** - Data goes in `private`. If `joins_` were public, any code anywhere could write `p.joins_ = -5;` and nothing would stop it. By keeping it private and offering `recordJoin()`, the class guarantees the count only ever goes up by one, and only when a join happens. That guarantee is called an *invariant*, and protecting invariants is the whole point of `private`.
- **The trailing underscore** (`joins_`) is just a naming convention for private members so you can tell them apart from local variables and parameters. Some codebases use `m_joins` instead.
- **Constructors** run when an object is created. `Player p;` calls the default constructor. `Player p("Steve");` calls the one that takes a string. You need a default constructor whenever a container may create objects for you (`std::vector<Player> v(5);` makes five empty players), which is why `Player()` exists even though it is empty.
- **`explicit`** on a one-argument constructor stops C++ from silently converting a `std::string` into a `Player` when you did not ask for it. Use it on every single-argument constructor unless you specifically want that conversion.
- **The member initializer list** (`: name_(name)`) initializes members before the constructor body runs. For a `std::string` this avoids constructing an empty string and then assigning over it. For `const` or reference members it is the only way.
- **`const` member functions** (`int joins() const`) promise not to modify the object. The compiler enforces it: try to write `++joins_` inside one and you get an error. Mark every getter `const`. It matters because a `const Player&` (which is how you should pass objects around, see below) can only call `const` functions.
- **`ServerLog::processFile`** - the `ClassName::` prefix in the `.cpp` file tells the compiler which class's function you are defining. Forget it and you have written a free function with the same name that cannot see the private members.
- **`.h` and `.cpp`** - The header is the contract; anything that wants to use a `Player` includes `Player.h`. The `.cpp` is the implementation and is compiled once. `#pragma once` at the top of a header stops it being included twice in the same file (which would redefine the class and fail to compile). Include your own headers with quotes (`#include "Player.h"`) and the standard library with angle brackets (`#include <string>`).
- **`struct` vs `class`** - identical except that `struct` members are public by default. Use `struct` for plain bags of data with no invariants to protect (`LoginRecord` in this lab) and `class` when the data needs guarding.
- **Composition** - `ServerLog` *has a* `std::vector<Player>`. A class that owns other objects is composed of them. It is the normal way classes cooperate; you will not need inheritance in this lab.

### Passing objects around

Copying a `Player` copies its strings and its vectors. Copying a `ServerLog` copies every player. Pass objects by **const reference** unless you have a reason not to:

```cpp
void printPlayer(const Player& p);     // no copy, cannot modify p -- the default choice
void updatePlayer(Player& p);          // no copy, CAN modify p -- when you mean to change it
void takePlayer(Player p);             // copies -- almost never what you want for a class
```

The same applies to `const std::string&`. Every function signature in the starter headers follows this rule; look at them.

### Reading a text file: `std::ifstream`

```cpp
#include <fstream>
#include <string>

std::ifstream in("data/logs/latest.log");   // open for reading
if (!in.is_open()) {                          // ALWAYS check
    std::cerr << "cannot open file\n";
    return false;
}
std::string line;
while (std::getline(in, line)) {              // reads one line, strips the '\n'
    // ... do something with line ...
}
// 'in' closes itself when it goes out of scope
```

- **`while (std::getline(in, line))`** is *the* way to read a file line by line. `getline` returns the stream, and a stream converts to `false` once a read has failed. The loop runs exactly once per line, including a last line with no newline.
- **Never write `while (!in.eof())`.** `eof()` only becomes true *after* a read has tried to go past the end. So that loop runs one extra time with a stale or empty `line`. This is the most common file-reading bug in C++ and it is on the test.
- **`getline` vs `>>`** - `in >> word` reads one whitespace-separated token and skips leading whitespace. `getline(in, line)` reads everything up to the newline. A log message has spaces in it, so you want `getline`. Use `>>` for things that are naturally space-separated numbers.
- **Relative paths** are relative to the *working directory* of the running program, not to where the source file lives. CLion runs your program from `cmake-build-debug/` by default. If `is_open()` fails and you are sure the file exists, this is why. Fix it by passing an absolute path, by setting the working directory in the run configuration (Run > Edit Configurations > Working directory), or by `cd`-ing to the right place in a terminal.
- **Windows line endings.** Files edited on Windows end lines with `\r\n`. `getline` strips the `\n` but leaves the `\r`. If a string that should equal `"left the game"` mysteriously does not, that is why. Strip it: `if (!line.empty() && line.back() == '\r') line.pop_back();`

### Writing a text file: `std::ofstream`

```cpp
std::ofstream out("report.txt");               // creates the file, or TRUNCATES it if it exists
if (!out.is_open()) return false;
out << "Players seen: " << players.size() << "\n";   // exactly like std::cout
```

- Opening with `std::ofstream` erases an existing file. To add to the end instead: `std::ofstream out("log.txt", std::ios::app);`
- Output is buffered. It reaches the disk when the stream is closed (end of scope) or flushed (`out.flush()` or `std::endl`). Prefer `'\n'` over `std::endl` inside loops; `endl` forces a flush every time and is slow.
- If you need to write to a file *and* see it immediately in another program while yours is still running, that is what `flush()` is for.

### Lining up columns: `<iomanip>`

```cpp
#include <iomanip>
out << std::left  << std::setw(18) << name          // pad name to 18 chars, text on the left
    << std::right << std::setw(7)  << joins << "\n"; // pad number to 7 chars, number on the right
out << std::fixed << std::setprecision(1) << 12.3456; // prints 12.3
out << std::setw(2) << std::setfill('0') << 7;        // prints 07  (good for times)
```

`setw` applies to the *next* thing printed only. `left`, `right`, `fixed`, `setprecision` and `setfill` stay in effect until changed. Remember to reset `setfill(' ')` after using `'0'` or your next table will be full of zeros.

### Taking strings apart: `find`, `substr`, and friends

Every line of the log must be cut into pieces. These five functions do all of it:

| Call | Returns | Example on `s = "[14:03:22] [Server thread/INFO]: hi"` |
| --- | --- | --- |
| `s.find("]: ")` | index of first match, or `std::string::npos` | `31` |
| `s.find('[', 10)` | first `[` at or after index 10 | `11` |
| `s.rfind("abc")` | index of *last* match | |
| `s.substr(1, 8)` | 8 characters starting at index 1 | `"14:03:22"` |
| `s.substr(34)` | everything from index 34 to the end | `"hi"` |
| `s.compare(0, 5, "[14:0") == 0` | does `s` start with that text | `true` |
| `std::stoi("22")`, `std::stod("-3.7")` | text to `int` / `double`; **throws** `std::invalid_argument` on junk | `22` |

`npos` is a special "not found" value. Always check for it before using a result in `substr`, or you will get an exception (`out_of_range`) on the first malformed line. The log has a malformed line in it on purpose.

### Parsing numbers out of text: `std::stringstream`

When a piece of text contains several numbers, a `stringstream` lets you use `>>` on it as if it were a file:

```cpp
#include <sstream>
std::string coords = "12.5 64.0 -3.2";
std::istringstream ss(coords);
double x, y, z;
if (ss >> x >> y >> z) { /* all three parsed */ }
```

The other direction, `std::ostringstream`, builds a string with `<<` and gives it back with `.str()`. Use it for `Player::timeOnlineText()`: `out << h << "h " << std::setw(2) << std::setfill('0') << m << "m"; return out.str();`

If the numbers are comma-separated (`"12.5, 64.0, -3.2"`), replace the commas with spaces first: `std::replace(coords.begin(), coords.end(), ',', ' ');` (needs `<algorithm>`). You will also see `std::getline(ss, field, ',')` in Part 2: `getline` with a third argument reads up to a delimiter instead of a newline, which splits a CSV line into fields.

### A `vector` as a lookup table: find-or-append

You need to look players up by name and count deaths by cause. Both are "find the entry with this key; if there isn't one, add it", and a `std::vector` plus a loop does it. It is the same loop as `ServerConfig::findKey` in the demo:

```cpp
struct CauseCount { std::string cause; int count = 0; };
std::vector<CauseCount> deathCauses_;

void Player::recordDeath(const std::string& cause) {
    ++deaths_;
    for (CauseCount& c : deathCauses_)            // find...
        if (c.cause == cause) { ++c.count; return; }
    deathCauses_.push_back({cause, 1});           // ...or append
}

Player& ServerLog::player(const std::string& name) {   // find-or-create
    for (Player& p : players_)
        if (p.name() == name) return p;
    players_.push_back(Player(name));
    return players_.back();
}
```

- **`CauseCount& c` in the loop, not `CauseCount c`.** Without the `&` the loop works on a *copy* of each entry and `++c.count` changes nothing. This is the number one bug in this lab. The same goes for `Player& p`: copying a `Player` to check its name is slow and, worse, returning a reference to the copy is a dangling reference.
- **A reference or pointer into a `vector` is valid only until the next `push_back`.** When the vector grows it may move all its elements to new memory. So use what `player()` returns immediately (`player(name).recordJoin(t);`) and never store it. `playersByTimeOnline()` returns pointers into `players_`; that is safe because you only call it after every file has been read and nothing is being added any more.
- The list stays in first-seen order. If you want alphabetical or by-time order, copy pointers into another vector and `std::sort` that (next section).
- A linear search over 23 players, 3,000 times, is nothing. If you had a million players you would want `std::map<std::string, Player>` (a sorted dictionary with `players["Steve"]` syntax, found in `<map>`) or `std::unordered_map`. You will meet them later in the course; the find-or-append loop is what they do for you behind the scenes.

### Sorting with a lambda

```cpp
#include <algorithm>
std::vector<const Player*> v;                     // pointers: we don't want to copy the Players
for (const Player& p : players_) v.push_back(&p);
std::sort(v.begin(), v.end(), [](const Player* a, const Player* b) {
    return a->secondsOnline() > b->secondsOnline();   // "a comes before b" when a has more time
});
```

A lambda is a function written inline. The `[]` is its capture list (empty here), then parameters, then a body returning `true` when the first argument should come first. `>` gives descending order, `<` ascending.

### Time arithmetic

The log timestamps are `HH:MM:SS` with no date. Convert them to seconds since midnight (`h*3600 + m*60 + s`) so that subtracting two of them gives a duration. A session that is still open when the file ends, and a `left the game` with no matching `joined the game` because the player was on at midnight, are both real situations in this data. The tasks tell you how to handle them.

## Anatomy of the log

Every normal line has the same skeleton. The server is Paper 26.3, so the formats below are exactly what is in the files.

```
[14:03:22] [Server thread/INFO]: System chat: Steve joined the game
 ^^^^^^^^   ^^^^^^^^^^^^^ ^^^^   ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
  time       thread       level  message
```

The timestamp is always characters 1 to 8. The thread and level sit inside the second pair of brackets, separated by `/`. The message is everything after `]: `. These are the line types your program must recognise:

| What happened | Thread | Message looks like | Your program should |
| --- | --- | --- | --- |
| Player authenticated | `User Authenticator #12` | `UUID of player Steve is 0b7e9bea-...` | remember `Steve` is a real player name |
| Joined | `Server thread` | `System chat: Steve joined the game` | `recordJoin` |
| Logged in with position | `Server thread` | `Steve[/0.0.0.0:0] logged in with entity id 42 at ([minecraft:overworld]12.5, 64.0, -3.2)` | `recordLogin` + add a `LoginRecord` |
| Left | `Server thread` | `System chat: Steve left the game` | `recordLeave` |
| Lost connection | `Server thread` | `Steve lost connection: Disconnected` | ignore (always paired with a "left") |
| Chat | `Async Chat Thread - #407` | `<Steve> yo gurt` | `recordChat` |
| Death | `Server thread` | `System chat: Steve was slain by Zombie` | `recordDeath("was slain by Zombie")` |
| Advancement | `Server thread` | `System chat: Steve has made the advancement [Stone Age]` | `recordAdvancement` |
| Challenge / goal | `Server thread` | `System chat: Steve has completed the challenge [Return to Sender]` | `recordAdvancement` |
| Command | `Server thread` | `Steve issued server command: /msg Alex 675 -124` | ignore (or count, as an extension) |
| Whitelist change | `Server thread` | `System chat: Added Steve to the whitelist` | ignore |
| Warning | `Server thread` (level `WARN`) | `Steve moved too quickly! 538.97,-41.0,178.11` | count it, then ignore |
| Rejected login | `Server thread` | `Disconnecting ryanturtle7926 (/0.0.0.0:0): You are not whitelisted on this server!` | ignore |

**Death messages have no fixed shape.** The 64 deaths in this data use 19 different sentences: `was slain by Zombie`, `drowned`, `fell from a high place`, `tried to swim in lava`, `was blown up by Creeper`, `was shot by Skeleton`, `was killed by Witch using magic`, `was doomed to fall by Magma Cube`, `fell off a ladder`, `burned to death`, and so on. You cannot list them all. The reliable rule: a `System chat:` message whose **first word is a known player's name** and which is not a join, leave, or advancement is a death. That is why you collect names from the `UUID of player` lines first.

**Things in the data that will break naive code.** These are not mistakes in the files; this is what real logs look like, and handling them is part of the lab.

- Line 564 of `2026-10-18-1.log` is `whiteliast add carolssol<--[HERE]` with no timestamp at all (someone mistyped a console command). Your `LogEntry` must mark it invalid instead of crashing.
- `System chat: That player does not exist` and `System chat: Unknown or incomplete command...` start with a word that is not a player. The known-player check handles them.
- Players are online when a day's file ends and the next day's file starts. `2026-10-18-1.log` ends at `23:59:18`; a player who left at `00:26:05` the next day has a `left the game` with no `joined the game` in that file.
- `8kizi8 was slain by vowelphobic` is a death *and* the killer is a player. Your death-cause list will contain `was slain by vowelphobic`. That is fine, and it makes the report more interesting.
- Three logins are in `[minecraft:the_nether]`. Nether coordinates are on a different map (1 nether block = 8 overworld blocks), so Part 2 will skip them. Record the dimension so it can.
- Chat text can contain anything, including `<`, `>`, `[` and `]`. Only trust the *first* `>` to find the end of the sender's name.
- Names are case-sensitive and contain `_` and digits: `8kizi8`, `t3rt1e`, `Kamish_Saladin`.

## The tasks

Do them in order. Each one has a checkpoint so you know it works before moving on. Open the matching `.cpp` file; every `TODO` names its task.

### Task 1 - `LogEntry` constructor and `parseTime` (`LogEntry.cpp`)

Implement `LogEntry::LogEntry(const std::string&)` so that it fills `time_`, `seconds_`, `thread_`, `level_` and `message_` and sets `valid_ = true`, or leaves `valid_` false if the line does not fit the pattern. Implement `parseTime("14:03:22")` to return `50602`, and `-1` for anything that is not exactly `HH:MM:SS`.

Write a tiny test in a scratch `main` (or use the one below) before you touch the other files:

```cpp
LogEntry e("[14:03:22] [Server thread/INFO]: System chat: Steve joined the game");
assert(e.isValid() && e.seconds() == 50602 && e.thread() == "Server thread"
       && e.level() == "INFO" && e.message() == "System chat: Steve joined the game");
LogEntry bad("whiteliast add carolssol<--[HERE]");
assert(!bad.isValid());
```

**Checkpoint:** both asserts pass. Try three more real lines from the log, including a `WARN`.

### Task 2 - the classifiers (`LogEntry.cpp`)

Implement `isSystemChat`, `systemChatText`, `isPlayerChat`, `chatSender`, `chatText`, `isLogin`, `isCommand`. Each is one to four lines using `find`, `compare` or `substr`. Make every one return something sensible on an invalid entry (`false` or `""`); never let them throw.

**Checkpoint:** `LogEntry("[01:44:52] [Async Chat Thread - #407/INFO]: <TheOkayGuy> yo gurt")` gives `isPlayerChat() == true`, `chatSender() == "TheOkayGuy"`, `chatText() == "yo gurt"`.

### Task 3 - `Player` event recorders (`Player.cpp`)

Implement `recordJoin`, `recordLeave`, `recordChat`, `recordDeath`, `deathsBy`, `recordAdvancement`, `recordLogin`. The session logic is in `recordLeave`:

- online and a leave arrives: add `seconds - sessionStart_` to the total.
- *not* online and a leave arrives: the player was on at midnight; add `seconds` (time since 00:00:00).
- either way they are now offline.

**Checkpoint:** a `Player` that joins at 100, leaves at 160, joins at 200, leaves at 230 reports `secondsOnline() == 90`. A fresh `Player` that just gets `recordLeave(3600)` reports `3600`.

### Task 4 - `ServerLog::processFile` and the dispatcher (`ServerLog.cpp`, `Player::endOfDay`)

This is the heart of the lab. `processFile` is the read loop from the concepts section: open, check, `getline`, strip `\r`, build a `LogEntry`, skip invalid ones (count them), pass the rest to `handle`. After the loop, call `endOfDay(lastSeconds_)` on every player so open sessions get credited up to the last timestamp in the file.

`handle` decides what a line is, in this order: UUID line, player chat, login, system chat, else ignore. `handleSystemChat` then sorts joins, leaves, advancements and deaths. Follow the `TODO` comments; they spell out every case.

The login line needs the most string surgery. Work on a copy in a scratch program until this prints the right numbers:

```
TheOkayGuy[/0.0.0.0:0] logged in with entity id 795941 at ([minecraft:overworld]-18.28375767285751, 78.0, 13.460360019648773)
-> name=TheOkayGuy dim=overworld x=-18.28 y=78 z=13.46
```

Also fix the `player()` helper. The starter's version does `push_back` without searching first, so every event creates a brand-new Player and the count explodes. Loop over `players_` looking for the name; only append when it is not there. Then implement `findPlayer` (the `const`, pointer-returning twin used by the report) and `isKnownPlayer`.

**Checkpoint:** run on `latest.log` alone. You should see `20 lines, 3 players, 3 logins with coordinates, 0 malformed lines`. Then run on all seven files: `3089 lines, 23 players, 173 logins with coordinates, 1 malformed lines`. If your player count is higher than 23 you are creating players from something that is not a player (check the death rule); if it is lower, you are missing a join format.

### Task 5 - the report (`ServerLog.cpp`, `Player::timeOnlineText`)

Implement `deathCauseTotals`, `playersByTimeOnline` (vector of pointers + `std::sort` + lambda) and `writeReport`. Use `setw`/`left`/`right` so the leaderboard columns line up regardless of name length. `timeOnlineText` formats `3h 04m` with the zero-padded minutes.

**Checkpoint:** the first three rows of your leaderboard are MissSciFiNerd 33h 16m, Kamish_Saladin 15h 47m, vowelphobic 10h 44m. Top cause of death: `tried to swim in lava` (13). Chattiest: vowelphobic (460). Most deaths: 8kizi8 (24).

Now open `data/players/player_stats.csv`. Those are the server's *own* play-time numbers (in ticks; 20 ticks = 1 second), recorded by Minecraft itself. MissSciFiNerd shows 33.7 hours there versus your 33h 16m. Why would they differ at all? Why is the server's number always a little higher? (Hint: when does the server start counting versus when the `joined the game` line is printed?)

### Task 6 - `logins.csv` (`ServerLog.cpp`)

Implement `writeLoginsCsv`. One header line, then one line per `LoginRecord`, numbers with two decimals. This file is the input to Part 2.

**Checkpoint:** 174 lines including the header. The first data line is `vowelphobic,2026-10-16,14:16:34,overworld,-1.50,72.00,-5.50`.

## Pitfalls, extensions, and what to hand in

### If it goes wrong

| Symptom | Likely cause |
| --- | --- |
| `FAILED` on every file | wrong working directory; print the path you are opening and check it exists relative to where the program *runs* |
| Crash (`std::out_of_range`) on the first file | `substr` with an `npos` index: a `find` failed and you did not check |
| Crash (`std::invalid_argument`) | `stoi` on non-digits; validate before converting |
| `left the game` never matches | a trailing `\r`; strip it |
| Player count is 24+ | something non-player is being treated as a death or a join (`That`, `Unknown`, `Added`) |
| Hundreds of players, the same name many times | `player()` appends without searching first; or the search loop uses `Player p` (a copy) instead of `Player& p` |
| Time online is negative | a leave is being matched to the wrong join, or `endOfDay` is not resetting `sessionStart_` to 0 |
| Everything is 0 | you are counting but the loop runs zero times: is `isValid()` always false? Print the first line's parts |
| Numbers slightly off from the checkpoint | the midnight rule: check `recordLeave` when not online, and `endOfDay` |

### Extensions (pick any; the teacher's guide says how they are weighted)

1. **Sessions table.** Record every (join, leave) pair as a `Session` struct and write `sessions.csv`. Then report each player's *longest* single session.
2. **Busiest hour.** Count how many players were online during each hour of the day across all days. Print a 24-row histogram with `#` characters as bars.
3. **Who killed whom.** Deaths `was slain by X` where `X` is a player are PvP. Build a kills leaderboard and the full list of victim/killer pairs.
4. **Chat word count.** Total words per player, average message length, and the single most-used word on the server (ignore words shorter than 4 letters).
5. **Robustness.** Run your program on a file that does not exist, an empty file, and a file of random text. It must not crash and must say what happened.
6. **Operator overloading.** Add `std::ostream& operator<<(std::ostream&, const Player&)` so `std::cout << player` prints one leaderboard row.
7. **Advancement tracker.** Capture the advancement name in the brackets and list, per player, which ones they earned in order.
8. **Stats cross-check.** Parse `data/players/player_stats.csv` (a simple CSV) and print your computed time next to the server's for each player, with the difference.
9. **`std::map`.** Read about `std::map<std::string, Player>` in `<map>` and replace the find-or-append loops in `player()`, `recordDeath()` and `deathCauseTotals()` with it. The public interface must not change. What got shorter? What order does the map iterate in, and what did that do to your report?

### Hand in

- `LogEntry.cpp`, `Player.cpp`, `ServerLog.cpp` (headers unchanged unless you added extension methods; if so, hand them in too).
- `report.txt` and `logins.csv` produced from all seven log files.
- A short `NOTES.md`: which extensions you did, anything you changed in the design and why, and your answer to the Task 5 question about why the server's play-time numbers differ from yours.

### Grading

| Component | Weight |
| --- | --- |
| Tasks 1-2: `LogEntry` parses every line type, invalid lines handled | 20% |
| Tasks 3-4: `Player` and `ServerLog` produce the checkpoint numbers | 35% |
| Task 5-6: report is complete and aligned; `logins.csv` is correct | 20% |
| Code quality: `const` correctness, no copies of big objects, data stays private, files always checked after opening | 15% |
| `NOTES.md` and at least one extension | 10% |
