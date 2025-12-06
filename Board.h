// Recognized as "include guards"
// Prevent the board header file from being used more than once
#ifndef BOARD_H
#define BOARD_H

// Tile struct definition (moved from Tile.h)
// Represents a single tile on the game board with a color property
// Color codes: 'G'=Green, 'B'=Blue, 'P'=Pink, 'R'=Red, 'T'=Brown, 'U'=Purple, 'O'=Orange, 'Y'=Grey
struct Tile {
    char color;
};

// Forward declaration - tells compiler Player class exists (defined in Player.h)
// Used to avoid circular dependencies
class Player;

// Board class: Manages the game board with two lanes (one per player)
// Each lane has 52 tiles with different colors/types that trigger different events
class Board {
    private:
        // Static in this context: Belongs to the class, not each object
        // Board has 52 tiles (positions 0-51, where 51 is the finish line)
        static const int _BOARD_SIZE = 52;
        // Maximum number of players (this is a 2-player game)
        static const int _MAX_PLAYERS = 2;

        // Composition! Board contains an array of Tile objects
        // _tiles[player_index][tile_position] - 2 lanes, each with 52 tiles
        Tile _tiles[2][_BOARD_SIZE];

        // Number of players (always 2 in this game)
        int _player_count;
        // Current position of each player on their respective lane
        int _player_position[_MAX_PLAYERS];

        // Private helper functions:
        // Initialize tiles for a specific player's lane with random colors
        void initializeTiles(int player_index);
        // Check if a player is currently on a specific tile position
        bool isPlayerOnTile(int player_index, int pos);
        // Display a single tile with appropriate color and player marker
        void displayTile(int player_index, int pos);

    public:
        // Default Constructor - creates board and initializes both lanes
        Board();

        // Initialize both player lanes with random tile distributions
        void initializeBoard();
        // Display a single player's track (all 52 tiles in their lane)
        void displayTrack(int player_index);
        // Display both players' tracks (the entire board)
        void displayBoard();
        // Move a player forward by 1 tile, returns true if they reached finish
        bool movePlayer(int player_index);
        // Set a player's position (with bounds checking)
        void setPlayerPosition(int player_index, int position);
        // Recall we can use const for getter functions
        // Get a player's current position on the board
        int getPlayerPosition(int player_index) const;
        // Get the color of a tile at a specific position for a specific player
        char getTileColor(int player_index, int position) const;
        // Trigger the event associated with the tile a player is on (placeholder - logic in main.cpp)
        void triggerTileEvent(int player_index, Player& player);
};

#endif