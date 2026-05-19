#pragma once
/*
============================================================
  FINAL ESCAPE — SHARED DATA STRUCTURES
  structs.h
  -------------------------------------------------------
  SHARED    : All Members (do not modify without team sync)
  PURPOSE   : Single source of truth for all structs used
              across filemanager, gamelogic, and main.

  RULES:
    - If you need to add a field, discuss with the team
      first — changing these structs affects all 3 modules.
============================================================
*/

#ifndef STRUCTS_H
#define STRUCTS_H

#include <string>

// ---- Full game progress record (stored in players.txt) ----
struct PlayerRecord {
    std::string name;           // player's display name
    int         score;          // current total score
    int         currentRoom;    // 0-based room index
    int         currentPuzzle;  // 0-based puzzle index within room
    int         health;         // remaining health (0–120)
};

// ---- Login/score record (stored in user.txt) ----
struct UserEntry {
    std::string name;           // player's display name
    int         score;          // score (kept in sync with PlayerRecord)
};

#endif // STRUCTS_H
