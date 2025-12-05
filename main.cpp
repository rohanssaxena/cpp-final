#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib> // For srand and rand (spinner)
#include <ctime>   // For time (seeding the random number generator)
#include <vector>
#include <sstream>
#include "Player.h"
#include "Board.h"

using namespace std;

// Structure for storing riddles
struct Riddle {
    string question;
    string answer;
};

// Structure for storing random events
struct RandomEvent {
    string description;
    int pathType;      // 0 = Training Fellowship, 1 = Direct Lab Assignment
    int advisor;       // 0 = none, 1-5 = different advisors
    int discoveryPoints;
};

// Global vectors to store game data
vector<Player> availableCharacters;
vector<Riddle> riddles;
vector<RandomEvent> randomEvents;

// Function to load characters from file
bool loadCharacters(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    string line;
    getline(file, line); // Skip header line
    
    while (getline(file, line)) {
        if (line.empty()) continue;
        
        stringstream ss(line);
        string name, exp, acc, eff, ins, dp;
        
        getline(ss, name, '|');
        getline(ss, exp, '|');
        getline(ss, acc, '|');
        getline(ss, eff, '|');
        getline(ss, ins, '|');
        getline(ss, dp, '|');
        
        Player p(name, stoi(exp), stoi(acc), stoi(eff), stoi(ins), stoi(dp));
        availableCharacters.push_back(p);
    }
    
    file.close();
    return true;
}

// Function to load riddles from file
bool loadRiddles(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    string line;
    getline(file, line); // Skip header line
    
    while (getline(file, line)) {
        if (line.empty()) continue;
        
        size_t pipePos = line.find('|');
        if (pipePos != string::npos) {
            Riddle r;
            r.question = line.substr(0, pipePos);
            r.answer = line.substr(pipePos + 1);
            riddles.push_back(r);
        }
    }
    
    file.close();
    return true;
}

// Function to load random events from file
bool loadRandomEvents(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    string line;
    getline(file, line); // Skip header line
    getline(file, line); // Skip comment line
    
    while (getline(file, line)) {
        if (line.empty() || line[0] == '/') continue; // Skip comments
        
        stringstream ss(line);
        string desc, pathType, advisor, dp;
        
        getline(ss, desc, '|');
        getline(ss, pathType, '|');
        getline(ss, advisor, '|');
        getline(ss, dp, '|');
        
        RandomEvent e;
        e.description = desc;
        e.pathType = stoi(pathType);
        e.advisor = stoi(advisor);
        e.discoveryPoints = stoi(dp);
        randomEvents.push_back(e);
    }
    
    file.close();
    return true;
}

// Function to ask a riddle
bool askRiddle(Player& player) {
    if (riddles.empty()) {
        return true; // No riddles available, assume correct
    }
    
    int riddleIndex = rand() % riddles.size();
    Riddle r = riddles[riddleIndex];
    
    cout << "\n=== RIDDLE ===" << endl;
    cout << r.question << endl;
    cout << "Your answer: ";
    
    string answer;
    getline(cin, answer);
    
    // Convert to lowercase for comparison
    string lowerAnswer = answer;
    for (char& c : lowerAnswer) {
        c = tolower(c);
    }
    
    string lowerCorrect = r.answer;
    for (char& c : lowerCorrect) {
        c = tolower(c);
    }
    
    if (lowerAnswer == lowerCorrect) {
        cout << "Correct! You gain 100 Discovery Points!" << endl;
        player.updateDiscoverPoints(100);
        return true;
    } else {
        cout << "Incorrect! The answer was: " << r.answer << endl;
        cout << "You lose 50 Discovery Points." << endl;
        player.updateDiscoverPoints(-50);
        return false;
    }
}

// Function to trigger a random event
void triggerRandomEvent(Player& player, char tileColor) {
    if (randomEvents.empty()) {
        return;
    }
    
    // Filter events based on tile color
    // Blue = PathType 0 (Training Fellowship), Pink = PathType 1 (Direct Lab Assignment)
    // Red, Brown, Purple can be either
    vector<RandomEvent> validEvents;
    
    for (const auto& event : randomEvents) {
        bool isValid = false;
        
        if (tileColor == 'B') { // Blue = Training Fellowship
            isValid = (event.pathType == 0);
        } else if (tileColor == 'P') { // Pink = Direct Lab Assignment
            isValid = (event.pathType == 1);
        } else { // Red, Brown, Purple can be either
            isValid = true;
        }
        
        if (isValid) {
            validEvents.push_back(event);
        }
    }
    
    if (validEvents.empty()) {
        return;
    }
    
    int eventIndex = rand() % validEvents.size();
    RandomEvent e = validEvents[eventIndex];
    
    cout << "\n=== RANDOM EVENT ===" << endl;
    cout << e.description << endl;
    
    // Check for advisor protection (if advisor matches player's stats)
    bool protectedByAdvisor = false;
    if (e.advisor > 0 && e.discoveryPoints < 0) {
        // Advisor protection logic - simplified: if player has high corresponding stat
        if (e.advisor == 1 && player.getAccuracy() > 700) protectedByAdvisor = true;
        if (e.advisor == 2 && player.getEfficiency() > 700) protectedByAdvisor = true;
        if (e.advisor == 3 && player.getInsight() > 700) protectedByAdvisor = true;
        if (e.advisor == 4 && player.getEfficiency() > 700) protectedByAdvisor = true;
        if (e.advisor == 5 && player.getInsight() > 700) protectedByAdvisor = true;
    }
    
    if (protectedByAdvisor && e.discoveryPoints < 0) {
        cout << "Your advisor protects you! No Discovery Points lost." << endl;
    } else {
        player.updateDiscoverPoints(e.discoveryPoints);
        if (e.discoveryPoints > 0) {
            cout << "You gain " << e.discoveryPoints << " Discovery Points!" << endl;
        } else {
            cout << "You lose " << -e.discoveryPoints << " Discovery Points." << endl;
        }
    }
}

// Function to handle tile events
void handleTileEvent(Board& board, Player& player, int playerIndex) {
    int pos = player.getPosition();
    char tileColor = board.getTileColor(playerIndex, pos);
    
    cout << "\n=== TILE EVENT ===" << endl;
    
    switch (tileColor) {
        case 'G': // Green - Regular tile, nothing special
            cout << "You landed on a regular tile. Nothing happens." << endl;
            break;
            
        case 'B': // Blue - Training Fellowship (Riddle)
            cout << "You landed on a Blue tile (Training Fellowship)!" << endl;
            if (askRiddle(player)) {
                cout << "You successfully completed the training!" << endl;
            }
            triggerRandomEvent(player, 'B');
            break;
            
        case 'P': // Pink - Direct Lab Assignment (Riddle)
            cout << "You landed on a Pink tile (Direct Lab Assignment)!" << endl;
            if (askRiddle(player)) {
                cout << "You successfully completed the assignment!" << endl;
            }
            triggerRandomEvent(player, 'P');
            break;
            
        case 'R': // Red - Challenge tile
            cout << "You landed on a Red tile (Challenge)!" << endl;
            if (askRiddle(player)) {
                cout << "Challenge completed! You gain 200 Discovery Points!" << endl;
                player.updateDiscoverPoints(200);
            } else {
                cout << "Challenge failed! You lose 100 Discovery Points." << endl;
                player.updateDiscoverPoints(-100);
            }
            triggerRandomEvent(player, 'R');
            break;
            
        case 'T': // Brown - Special event
            cout << "You landed on a Brown tile (Special Event)!" << endl;
            triggerRandomEvent(player, 'T');
            break;
            
        case 'U': { // Purple - Bonus tile
            cout << "You landed on a Purple tile (Bonus)!" << endl;
            int bonus = 300 + (rand() % 201); // 300-500 bonus
            cout << "You gain " << bonus << " Discovery Points!" << endl;
            player.updateDiscoverPoints(bonus);
            triggerRandomEvent(player, 'U');
            break;
        }
            
        case 'O': // Orange - Finish line
            cout << "Congratulations! You reached the finish line!" << endl;
            break;
            
        default:
            break;
    }
}

// Helper function to display main menu
void displayMainMenu(Player& player) {
    cout << "\n=== Main Menu ===" << endl;
    cout << "1. Check Progress" << endl;
    cout << "2. Move Forward" << endl;
    cout << "Enter your choice (1 or 2): ";
}

// Function to handle menu choice
int handleMenuChoice(Player& player) {
    string choice;
    getline(cin, choice);
    
    if (choice == "1") {
        cout << "\n=== Player Progress ===" << endl;
        cout << "Character: " << player.getCharacterName() << endl;
        cout << "Experience: " << player.getExperience() << endl;
        cout << "Accuracy: " << player.getAccuracy() << endl;
        cout << "Efficiency: " << player.getEfficiency() << endl;
        cout << "Insight: " << player.getInsight() << endl;
        cout << "Discovery Points: " << player.getDiscoverPoints() << endl;
        cout << "Position: " << player.getPosition() << " / 51" << endl;
        return 1;
    } else if (choice == "2") {
        return 2;
    } else {
        cout << "Invalid choice. Please enter 1 or 2." << endl;
        return 0;
    }
}

int main() {
    // Seed the random number generator
    srand(time(nullptr));
    
    // Load game data
    cout << "Loading game data..." << endl;
    if (!loadCharacters("characters.txt")) {
        cout << "Warning: Could not load characters.txt. Using default characters." << endl;
    }
    if (!loadRiddles("riddles.txt")) {
        cout << "Warning: Could not load riddles.txt." << endl;
    }
    if (!loadRandomEvents("random_events.txt")) {
        cout << "Warning: Could not load random_events.txt." << endl;
    }
    
    // Initialize game board
    Board gameBoard;
    
    // Initialize players
    Player player1, player2;
    
    if (availableCharacters.size() >= 2) {
        // Use first two characters from file
        player1 = availableCharacters[0];
        player2 = availableCharacters[1];
        cout << "Loaded characters: " << player1.getCharacterName() 
             << " and " << player2.getCharacterName() << endl;
    } else {
        // Fallback to default characters
        player1 = Player("Dr.Leo", 5, 500, 500, 1000, 20000);
        player2 = Player("Dr.Helix", 8, 900, 600, 600, 20000);
        cout << "Using default characters." << endl;
    }
    
    // Initialize board positions
    gameBoard.setPlayerPosition(0, 0);
    gameBoard.setPlayerPosition(1, 0);
    
    cout << "\n=== Journey Through Genome ===" << endl;
    cout << "Game Starting!" << endl;
    
    // Main Game Loop
    bool game_over = false;
    int turn = 1;
    
    while (!game_over) {
        // Determine which player's turn it is
        Player& currentPlayer = (turn % 2 != 0) ? player1 : player2;
        int currentPlayerIndex = (turn % 2 != 0) ? 0 : 1;
        
        cout << "\n========================================" << endl;
        cout << "--- Player " << (turn % 2 != 0 ? 1 : 2) << "'s Turn (" 
             << currentPlayer.getCharacterName() << ") ---" << endl;
        cout << "========================================" << endl;
        
        // 1. Display Main Menu and handle choice
        int menuChoice = 0;
        while (menuChoice != 2) {
            displayMainMenu(currentPlayer);
            menuChoice = handleMenuChoice(currentPlayer);
            if (menuChoice == 0) {
                continue; // Invalid choice, try again
            }
        }
        
        // 2. Player Movement (Spinner Logic)
        cout << "\nRolling the dice..." << endl;
        int steps = rand() % 6 + 1; // Generates 1 to 6
        cout << "You rolled: " << steps << endl;
        
        int oldPosition = currentPlayer.getPosition();
        currentPlayer.updatePosition(steps);
        int newPosition = currentPlayer.getPosition();
        
        cout << "Moving from position " << oldPosition << " to position " << newPosition << endl;
        
        // Sync player positions with board
        gameBoard.setPlayerPosition(0, player1.getPosition());
        gameBoard.setPlayerPosition(1, player2.getPosition());
        
        // 3. Update and Display Board
        cout << "\n=== Current Board State ===" << endl;
        gameBoard.displayBoard();
        
        // 4. Trigger Tile Event (only if player moved to a new tile)
        if (newPosition != oldPosition && newPosition < 51) {
            handleTileEvent(gameBoard, currentPlayer, currentPlayerIndex);
        }
        
        // 5. Check End Condition (first player to reach tile 51 wins)
        if (player1.getPosition() >= 51) {
            game_over = true;
            cout << "\n========================================" << endl;
            cout << "GAME OVER!" << endl;
            cout << "Player 1 (" << player1.getCharacterName() 
                 << ") wins with " << player1.getDiscoverPoints() << " Discovery Points!" << endl;
            cout << "Player 2 (" << player2.getCharacterName() 
                 << ") finished with " << player2.getDiscoverPoints() << " Discovery Points." << endl;
            cout << "========================================" << endl;
        } else if (player2.getPosition() >= 51) {
            game_over = true;
            cout << "\n========================================" << endl;
            cout << "GAME OVER!" << endl;
            cout << "Player 2 (" << player2.getCharacterName() 
                 << ") wins with " << player2.getDiscoverPoints() << " Discovery Points!" << endl;
            cout << "Player 1 (" << player1.getCharacterName() 
                 << ") finished with " << player1.getDiscoverPoints() << " Discovery Points." << endl;
            cout << "========================================" << endl;
        }
        
        turn++;
    }
    
    return 0;
}
