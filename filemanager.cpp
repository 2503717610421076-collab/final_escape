/*
============================================================
  FINAL ESCAPE — FILE MANAGER MODULE
  filemanager.cpp
  -------------------------------------------------------
  MEMBER    : Member 2
  PURPOSE   : Implements all file read/write operations.

  YOUR RESPONSIBILITIES:
    1. readLines()    — load question/answer .txt files
    2. loadUsers()    — read user.txt on login
    3. saveUsers()    — write user.txt after every score change
    4. loadPlayers()  — read players.txt for full progress
    5. savePlayers()  — write players.txt on save/exit
    6. writeGameJson()— generate game.json for the browser
    7. trim() / jsonEscape() — utility helpers

  HOW TO COMPILE (together with other modules):
    g++ -std=c++17 -c filemanager.cpp -o filemanager.o
============================================================
*/

#include "filemanager.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;

// ============================================================
//  FILE PATH DEFINITIONS
//  (declared extern in filemanager.h — defined once here)
// ============================================================

const string FILE_PUZZLE_Q     = "puzzle.txt";
const string FILE_ACTIVITY_Q   = "activity.txt";
const string FILE_PUZZLE_ANS   = "anspuzzle.txt";
const string FILE_ACTIVITY_ANS = "ansactivity.txt";
const string FILE_USERS        = "user.txt";      // NAME|SCORE
const string FILE_PLAYERS      = "players.txt";   // NAME|SCORE|ROOM|PUZZLE|HEALTH
const string FILE_GAME_JSON    = "game.json";

// ============================================================
//  UTILITY: trim whitespace from both ends of a string
// ============================================================

string trim(const string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

// ============================================================
//  UTILITY: escape special characters for JSON string output
// ============================================================

string jsonEscape(const string& s) {
    string out;
    for (char c : s) {
        if      (c == '"')  out += "\\\"";
        else if (c == '\\') out += "\\\\";
        else if (c == '\n') out += "\\n";
        else if (c == '\r') out += "\\r";
        else if (c == '\t') out += "\\t";
        else                out += c;
    }
    return out;
}

// ============================================================
//  READ LINES FROM A TXT FILE
//  Used to load puzzle.txt, activity.txt, anspuzzle.txt,
//  ansactivity.txt — one entry per non-empty line.
// ============================================================

vector<string> readLines(const string& filename) {
    vector<string> lines;
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "[FILEMANAGER ERROR] Cannot open: " << filename << endl;
        return lines;
    }

    string line;
    while (getline(file, line)) {
        string t = trim(line);
        if (!t.empty()) lines.push_back(t);
    }

    file.close();
    return lines;
}

// ============================================================
//  USER.TXT — LOGIN STORE
//  Format: NAME|SCORE   (one player per line)
//
//  loadUsers() is called at server startup AND on every
//  LOAD_GAME command to check if a player already exists.
// ============================================================

map<string, UserEntry> loadUsers() {
    map<string, UserEntry> users;
    ifstream file(FILE_USERS);

    if (!file.is_open()) {
        // Normal on first run — file doesn't exist yet
        return users;
    }

    string line;
    while (getline(file, line)) {
        line = trim(line);
        if (line.empty()) continue;

        // Split on the first '|' separator
        size_t sep = line.find('|');
        if (sep == string::npos) continue;  // malformed line, skip

        UserEntry u;
        u.name       = trim(line.substr(0, sep));
        string scoreStr = trim(line.substr(sep + 1));

        try   { u.score = stoi(scoreStr); }
        catch (...) { u.score = 0; }

        // Map key is lowercase name for case-insensitive lookup
        string key = u.name;
        transform(key.begin(), key.end(), key.begin(), ::tolower);
        users[key] = u;
    }

    file.close();
    return users;
}

// ============================================================
//  saveUsers() — overwrites user.txt with current map data.
//  Called after every login (new user) and every score change.
// ============================================================

void saveUsers(const map<string, UserEntry>& users) {
    ofstream file(FILE_USERS);

    if (!file.is_open()) {
        cerr << "[FILEMANAGER ERROR] Cannot write: " << FILE_USERS << endl;
        return;
    }

    for (auto& pair : users) {
        const UserEntry& u = pair.second;
        file << u.name << "|" << u.score << "\n";
    }

    file.close();
    cout << "[FILEMANAGER] user.txt saved (" << users.size() << " users)\n";
}

// ============================================================
//  PLAYERS.TXT — FULL PROGRESS STORE
//  Format: NAME|SCORE|ROOM|PUZZLE|HEALTH   (one player per line)
//
//  loadPlayers() is called at startup to restore all sessions.
// ============================================================

map<string, PlayerRecord> loadPlayers() {
    map<string, PlayerRecord> players;
    ifstream file(FILE_PLAYERS);

    if (!file.is_open()) {
        // Normal on first run
        return players;
    }

    string line;
    while (getline(file, line)) {
        line = trim(line);
        if (line.empty()) continue;

        // Parse pipe-delimited fields
        vector<string> fields;
        stringstream ss(line);
        string field;
        while (getline(ss, field, '|')) fields.push_back(trim(field));

        if (fields.size() < 5) continue;  // malformed line, skip

        PlayerRecord rec;
        rec.name = fields[0];
        try {
            rec.score         = stoi(fields[1]);
            rec.currentRoom   = stoi(fields[2]);
            rec.currentPuzzle = stoi(fields[3]);
            rec.health        = stoi(fields[4]);
        } catch (...) { continue; }

        string key = rec.name;
        transform(key.begin(), key.end(), key.begin(), ::tolower);
        players[key] = rec;
    }

    file.close();
    return players;
}

// ============================================================
//  savePlayers() — overwrites players.txt with current map.
//  Called after SAVE_PROGRESS and EXIT_GAME commands.
// ============================================================

void savePlayers(const map<string, PlayerRecord>& players) {
    ofstream file(FILE_PLAYERS);

    if (!file.is_open()) {
        cerr << "[FILEMANAGER ERROR] Cannot write: " << FILE_PLAYERS << endl;
        return;
    }

    for (auto& pair : players) {
        const PlayerRecord& rec = pair.second;
        file << rec.name          << "|"
             << rec.score         << "|"
             << rec.currentRoom   << "|"
             << rec.currentPuzzle << "|"
             << rec.health        << "\n";
    }

    file.close();
    cout << "[FILEMANAGER] players.txt saved (" << players.size() << " records)\n";
}

// ============================================================
//  WRITE GAME.JSON — browser data bundle
//  Merges all 4 question/answer arrays into a JSON array
//  that the HTML front-end fetches on startup.
//
//  Called once at server startup (main.cpp) and again on
//  GET_QUESTIONS command (gamelogic.cpp).
// ============================================================

void writeGameJson(
    const vector<string>& puzzleQ,
    const vector<string>& activityQ,
    const vector<string>& puzzleAns,
    const vector<string>& activityAns
) {
    ofstream out(FILE_GAME_JSON);

    if (!out.is_open()) {
        cerr << "[FILEMANAGER ERROR] Cannot write: " << FILE_GAME_JSON << endl;
        return;
    }

    // Use the shortest array length so indices are always valid
    int total = (int)min({ puzzleQ.size(), activityQ.size(),
                           puzzleAns.size(), activityAns.size() });

    out << "{\n";
    out << "  \"puzzles\": [\n";
    for (int i = 0; i < total; i++) {
        out << "    {\n";
        out << "      \"id\": "         << i                           << ",\n";
        out << "      \"question\": \"" << jsonEscape(puzzleQ[i])      << "\",\n";
        out << "      \"answer\": \""   << jsonEscape(puzzleAns[i])    << "\",\n";
        out << "      \"activity\": \"" << jsonEscape(activityQ[i])    << "\",\n";
        out << "      \"actAns\": \""   << jsonEscape(activityAns[i])  << "\"\n";
        out << "    }";
        if (i < total - 1) out << ",";
        out << "\n";
    }
    out << "  ]\n";
    out << "}\n";

    out.close();
    cout << "[FILEMANAGER] game.json written (" << total << " puzzles)\n";
}
