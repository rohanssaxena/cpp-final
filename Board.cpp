// Use header file
#include "Board.h"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>

// Define macros for colors
#define ORANGE "\033[48;2;230;115;0m"
#define GREY "\033[48;2;128;128;128m"
#define GREEN "\033[48;2;34;139;34m"
#define BLUE "\033[48;2;10;10;230m"
#define PINK "\033[48;2;255;105;180m"
#define BROWN "\033[48;2;139;69;19m"
#define RED "\033[48;2;230;10;10m"
#define PURPLE "\033[48;2;128;0;128m"
#define RESET "\033[0m"

using namespace std;

// CONSTRUCTOR

Board::Board() {
    _player_count = _MAX_PLAYERS;

    for (int i = 0; i < _player_count; i++) {
        _player_position[i] = 0;
    }

    initializeBoard();
}

// PRIVATE MEMBER FUNCTIONS

// initializeTiles: randomly generates tiles for one player's lane
//   FOR each tile position (0 to 51):
//     IF position is 51 (last tile):
//       set to orange (finish line)
//     ELSE IF position is 0 (first tile):
//       set to grey (start)
//     ELSE IF we need more green tiles AND random chance succeeds:
//       set to green (regular tile)
//       increment green count
//     ELSE:
//       randomly choose: blue, pink, brown, red, or purple
//     store tile in board array

void Board::initializeTiles(int player_index) {
    Tile tile;
    int green_count = 0;  
    int total_tiles = _BOARD_SIZE;

    for (int i = 0; i < total_tiles; i++) {
        if (i == total_tiles - 1) {
            tile.color = 'O';
        } 
        else if (i == 0) {
            tile.color = 'Y';
        } 
        else if (green_count < 30 && (rand() % (total_tiles - i) < 30 - green_count)) {
            tile.color = 'G';
            green_count++;
        }
        else {
            int color_choice = rand() % 5;  
            switch (color_choice) {
                case 0:
                    tile.color = 'B'; // Blue - Training Fellowship (DNA Task 1)
                    break;
                case 1:
                    tile.color = 'P'; // Pink - Direct Lab Assignment (DNA Task 2)
                    break;
                case 2:
                    tile.color = 'T'; // Brown - Special Event (DNA Task 4)
                    break;
                case 3:
                    tile.color = 'R'; // Red - Challenge (DNA Task 3)
                    break;
                case 4:
                    tile.color = 'U'; // Purple - Bonus tile
                    break;
            }
        }

        _tiles[player_index][i] = tile;
    }
}

bool Board::isPlayerOnTile(int player_index, int pos) {
    if (_player_position[player_index] == pos) {
        return true;
    }
    return false;
}

void Board::displayTile(int player_index, int pos) {
    string color = "";  
    bool player = isPlayerOnTile(player_index, pos);  

    switch(_tiles[player_index][pos].color) {
        case 'O': color = ORANGE; break;  // Finish line
        case 'Y': color = GREY; break;    // Start
        case 'G': color = GREEN; break;   // Regular tile
        case 'B': color = BLUE; break;    // Training Fellowship
        case 'P': color = PINK; break;    // Direct Lab Assignment
        case 'T': color = BROWN; break;   // Special Event
        case 'R': color = RED; break;     // Challenge
        case 'U': color = PURPLE; break;  // Bonus
    }

    if (player == true) {
        cout << color << "|" << (player_index + 1) << "|" << RESET;
    }
    else {
        cout << color << "| |" << RESET;
    }
}

// PUBLIC MEMBER FUNCTIONS

void Board::initializeBoard() {
    for (int i = 0; i < 2; i++) {
        initializeTiles(i);
    }
}

void Board::displayTrack(int player_index) {
    for (int i = 0; i < _BOARD_SIZE; i++) {
        displayTile(player_index, i);
    }
    cout << endl;
}

void Board::displayBoard() {
    for (int i = 0; i < 2; i++) {
        displayTrack(i); 
        if (i == 0) {
            cout << endl; 
        }
    }
}

bool Board::movePlayer(int player_index) {
    // Move player position by one
    _player_position[player_index]++;

    // Player reached last tile (position 51 is the finish)
    if (_player_position[player_index] == _BOARD_SIZE - 1) {
        return true;
    }

    return false;
}

void Board::setPlayerPosition(int player_index, int position) {
    if (player_index >= 0 && player_index < _player_count) {
        if (position < 0) {
            _player_position[player_index] = 0;  
        } else if (position >= _BOARD_SIZE) {
            _player_position[player_index] = _BOARD_SIZE - 1;  
        } else {
            _player_position[player_index] = position; 
        }
    }
}

int Board::getPlayerPosition(int player_index) const {
    if (player_index >= 0 && player_index < _player_count) {
        return _player_position[player_index];
    }
    return -1; 
}

char Board::getTileColor(int player_index, int position) const {
    if (player_index >= 0 && player_index < _player_count && 
        position >= 0 && position < _BOARD_SIZE) {
        return _tiles[player_index][position].color;
    }
    return ' '; 
}

void Board::triggerTileEvent(int player_index, Player& player) {
    int pos = _player_position[player_index];
    char tileColor = getTileColor(player_index, pos);
    
    // This method will be called from main.cpp where the actual event logic is implemented
    // The tile color is passed to determine which event to trigger
}