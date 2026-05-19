/*
============================================================
  FINAL ESCAPE — SERVER ENTRY POINT
  main.cpp
  -------------------------------------------------------
  MEMBER    : Member 4
  PURPOSE   : Server startup, file loading, and the
              stdin command loop that ties all modules together.

  YOUR RESPONSIBILITIES:
    1. Print startup banner
    2. Call filemanager to load all 4 question/answer files
    3. Call writeGameJson() to build game.json for the browser
    4. Load user.txt and players.txt into memory maps
    5. Run the command loop — read lines from stdin, pass
       each to handleCommand() in gamelogic.cpp
    6. Handle clean EXIT shutdown

  HOW TO COMPILE ALL MODULES TOGETHER:
    g++ -std=c++17 main.cpp filemanager.cpp gamelogic.cpp -o gameserver

  HOW TO RUN:
    ./gameserver

  COMMAND REFERENCE (for testing):
    LOAD_GAME <name>
    SAVE_PROGRESS <name> <score> <room> <puzzle> <health>
    GET_QUESTIONS
    EXIT_GAME <name> <score> <room> <puzzle> <health>
    LEADERBOARD
    EXIT
============================================================
*/

#include "structs.h"
#include "filemanager.h"
#include "gamelogic.h"

#include <iostream>
#include <string>
#include <map>
#include <vector>

using namespace std;

int main() {

    // =========================================================
    //  STARTUP BANNER
    // =========================================================

    cout << "============================================\n";
    cout << "  FINAL ESCAPE — C++ GAME SERVER  v2.0\n";
    cout << "  Modular Edition (3-member split)\n";
    cout << "============================================\n\n";

    // =========================================================
    //  STEP 1 — Load question / answer files from disk
    //           (calls filemanager.cpp → readLines)
    // =========================================================

    cout << "[MAIN] Loading puzzle.txt ...\n";
    vector<string> puzzleQ = readLines(FILE_PUZZLE_Q);
    cout << "  -> " << puzzleQ.size() << " puzzle questions loaded\n";

    cout << "[MAIN] Loading activity.txt ...\n";
    vector<string> activityQ = readLines(FILE_ACTIVITY_Q);
    cout << "  -> " << activityQ.size() << " activity questions loaded\n";

    cout << "[MAIN] Loading anspuzzle.txt ...\n";
    vector<string> puzzleAns = readLines(FILE_PUZZLE_ANS);
    cout << "  -> " << puzzleAns.size() << " puzzle answers loaded\n";

    cout << "[MAIN] Loading ansactivity.txt ...\n";
    vector<string> activityAns = readLines(FILE_ACTIVITY_ANS);
    cout << "  -> " << activityAns.size() << " activity answers loaded\n";

    // =========================================================
    //  STEP 2 — Generate game.json for the browser
    //           (calls filemanager.cpp → writeGameJson)
    // =========================================================

    cout << "\n[MAIN] Building game.json ...\n";
    writeGameJson(puzzleQ, activityQ, puzzleAns, activityAns);

    // =========================================================
    //  STEP 3 — Load user.txt (login store)
    //           (calls filemanager.cpp → loadUsers)
    // =========================================================

    cout << "\n[MAIN] Loading user.txt (login store) ...\n";
    map<string, UserEntry> users = loadUsers();
    cout << "  -> " << users.size() << " user(s) found\n";

    // =========================================================
    //  STEP 4 — Load players.txt (full progress store)
    //           (calls filemanager.cpp → loadPlayers)
    // =========================================================

    cout << "\n[MAIN] Loading players.txt (progress store) ...\n";
    map<string, PlayerRecord> players = loadPlayers();
    cout << "  -> " << players.size() << " progress record(s) found\n";

    // =========================================================
    //  STEP 5 — Command loop
    //           Reads one command per line from stdin.
    //           Each line is passed to handleCommand()
    //           in gamelogic.cpp for processing.
    // =========================================================

    cout << "\n[MAIN] Server ready. Waiting for commands...\n";
    cout << "-----------------------------------------------\n";
    cout << "  LOAD_GAME <name>\n";
    cout << "  SAVE_PROGRESS <name> <score> <room> <puzzle> <health>\n";
    cout << "  GET_QUESTIONS\n";
    cout << "  EXIT_GAME <name> <score> <room> <puzzle> <health>\n";
    cout << "  LEADERBOARD\n";
    cout << "  EXIT\n";
    cout << "-----------------------------------------------\n\n";

    string inputLine;

    while (true) {
        cout << "> ";
        cout.flush();

        // getline returns false on EOF (pipe closed or Ctrl+D)
        if (!getline(cin, inputLine)) {
            cout << "\n[MAIN] Input stream closed. Shutting down.\n";
            break;
        }

        inputLine = trim(inputLine);
        if (inputLine.empty()) continue;

        // Clean EXIT — no save needed here (browser calls EXIT_GAME first)
        if (inputLine == "EXIT") {
            cout << "[MAIN] EXIT received. Server shutting down.\n";
            break;
        }

        // Dispatch to gamelogic.cpp
        handleCommand(inputLine, players, users,
                      puzzleQ, activityQ,
                      puzzleAns, activityAns);
    }

    cout << "[MAIN] Goodbye.\n";
    return 0;
}
