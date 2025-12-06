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

// Structure to hold game data (replaces global variables)
struct GameData {
    vector<Player> availableCharacters;
    vector<Riddle> riddles;
    vector<RandomEvent> randomEvents;
};

// Function to load characters from file (no const reference)
bool loadCharacters(string filename, GameData& gameData) {
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
        gameData.availableCharacters.push_back(p);
    }
    
    file.close();
    return true;
}

// Function to load riddles from file (no const reference)
bool loadRiddles(string filename, GameData& gameData) {
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
            gameData.riddles.push_back(r);
        }
    }
    
    file.close();
    return true;
}

// Function to load random events from file (no const reference)
bool loadRandomEvents(string filename, GameData& gameData) {
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
        gameData.randomEvents.push_back(e);
    }
    
    file.close();
    return true;
}

// DNA Task 1: Similarity (Equal-Length) - Blue tiles
double strandSimilarity(string strand1, string strand2) {
    if (strand1.length() != strand2.length() || strand1.length() == 0) {
        return 0.0;
    }
    
    int matches = 0;
    int total = strand1.length();
    
    for (int i = 0; i < total; i++) {
        if (strand1[i] == strand2[i]) {
            matches++;
        }
    }
    
    return (double)matches / (double)total;
}

// DNA Task 2: Similarity (Unequal-Length) - Pink tiles
int bestStrandMatch(string input_strand, string target_strand) {
    if (input_strand.length() == 0 || target_strand.length() == 0) {
        return -1;
    }
    
    double bestScore = 0.0;
    int bestIndex = 0;
    
    // If input is shorter or equal, slide it along target (best match always at start of input)
    if (input_strand.length() <= target_strand.length()) {
        int matches = 0;
        int compareLen = input_strand.length();
        if (target_strand.length() < compareLen) {
            compareLen = target_strand.length();
        }
        for (int j = 0; j < compareLen; j++) {
            if (input_strand[j] == target_strand[j]) {
                matches++;
            }
        }
        return 0; // Best match starts at beginning of input
    }
    
    // If input is longer, slide target along input
    for (int i = 0; i <= (int)(input_strand.length() - target_strand.length()); i++) {
        int matches = 0;
        for (int j = 0; j < (int)target_strand.length(); j++) {
            if (input_strand[i + j] == target_strand[j]) {
                matches++;
            }
        }
        double score = (double)matches / (double)target_strand.length();
        if (score > bestScore) {
            bestScore = score;
            bestIndex = i; // Position in input_strand where best match starts
        }
    }
    
    return bestIndex;
}

// DNA Task 3: Mutation Identification - Red tiles
void identifyMutations(string input_strand, string target_strand) {
    int bestIndex = bestStrandMatch(input_strand, target_strand);
    
    string shorter = input_strand;
    string longer = target_strand;
    bool inputIsShorter = true;
    
    if (input_strand.length() > target_strand.length()) {
        shorter = target_strand;
        longer = input_strand;
        inputIsShorter = false;
    }
    
    int inputPos = 0;
    int targetPos = bestIndex;
    
    while (inputPos < (int)shorter.length() && targetPos < (int)longer.length()) {
        if (shorter[inputPos] == longer[targetPos]) {
            inputPos++;
            targetPos++;
        } else {
            // Check for substitution
            if (inputPos + 1 < (int)shorter.length() && targetPos + 1 < (int)longer.length() &&
                shorter[inputPos + 1] == longer[targetPos + 1]) {
                cout << "Substitution at position " << inputPos << ": " 
                     << shorter[inputPos] << " -> " << longer[targetPos] << endl;
                inputPos++;
                targetPos++;
            }
            // Check for insertion
            else if (inputPos < (int)shorter.length() && targetPos + 1 < (int)longer.length() &&
                     shorter[inputPos] == longer[targetPos + 1]) {
                cout << "Insertion at position " << targetPos << ": " 
                     << longer[targetPos] << " inserted" << endl;
                targetPos++;
            }
            // Check for deletion
            else if (inputPos + 1 < (int)shorter.length() && targetPos < (int)longer.length() &&
                     shorter[inputPos + 1] == longer[targetPos]) {
                cout << "Deletion at position " << inputPos << ": " 
                     << shorter[inputPos] << " deleted" << endl;
                inputPos++;
            } else {
                cout << "Substitution at position " << inputPos << ": " 
                     << shorter[inputPos] << " -> " << longer[targetPos] << endl;
                inputPos++;
                targetPos++;
            }
        }
    }
    
    // Handle remaining characters
    while (targetPos < (int)longer.length()) {
        cout << "Insertion at position " << targetPos << ": " 
             << longer[targetPos] << " inserted" << endl;
        targetPos++;
    }
    while (inputPos < (int)shorter.length()) {
        cout << "Deletion at position " << inputPos << ": " 
             << shorter[inputPos] << " deleted" << endl;
        inputPos++;
    }
}

// DNA Task 4: Transcribe DNA to RNA - Brown tiles
void transcribeDNAtoRNA(string strand) {
    string rna = "";
    for (int i = 0; i < (int)strand.length(); i++) {
        if (strand[i] == 'T') {
            rna = rna + 'U';
        } else {
            rna = rna + strand[i];
        }
    }
    cout << "DNA: " << strand << endl;
    cout << "RNA: " << rna << endl;
}

// Function to convert string to lowercase (no range-based for loop)
string toLowercase(string str) {
    string result = "";
    for (int i = 0; i < (int)str.length(); i++) {
        char c = str[i];
        if (c >= 'A' && c <= 'Z') {
            c = c - 'A' + 'a';
        }
        result = result + c;
    }
    return result;
}

// Function to ask a riddle
bool askRiddle(Player& player, GameData& gameData) {
    if (gameData.riddles.size() == 0) {
        return true; // No riddles available, assume correct
    }
    
    int riddleIndex = rand() % gameData.riddles.size();
    Riddle r = gameData.riddles[riddleIndex];
    
    cout << "\n=== RIDDLE ===" << endl;
    cout << r.question << endl;
    cout << "Your answer: ";
    
    string answer;
    getline(cin, answer);
    
    string lowerAnswer = toLowercase(answer);
    string lowerCorrect = toLowercase(r.answer);
    
    if (lowerAnswer == lowerCorrect) {
        cout << "Correct! You gain 100 Discovery Points!" << endl;
        player.updateDiscoverPoints(100);
        player.enforceMinimumStats();
        return true;
    } else {
        cout << "Incorrect! The answer was: " << r.answer << endl;
        cout << "You lose 50 Discovery Points." << endl;
        player.updateDiscoverPoints(-50);
        player.enforceMinimumStats();
        return false;
    }
}

// Function to trigger a random event
void triggerRandomEvent(Player& player, char tileColor, GameData& gameData) {
    if (gameData.randomEvents.size() == 0) {
        return;
    }
    
    // Filter events based on tile color (no range-based for loop)
    vector<RandomEvent> validEvents;
    
    for (int i = 0; i < (int)gameData.randomEvents.size(); i++) {
        RandomEvent event = gameData.randomEvents[i];
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
    
    if (validEvents.size() == 0) {
        return;
    }
    
    int eventIndex = rand() % validEvents.size();
    RandomEvent e = validEvents[eventIndex];
    
    cout << "\n=== RANDOM EVENT ===" << endl;
    cout << e.description << endl;
    
    // Check for advisor protection
    bool protectedByAdvisor = false;
    if (e.advisor > 0 && e.discoveryPoints < 0 && player.getAdvisor() == e.advisor) {
        protectedByAdvisor = true;
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
        player.enforceMinimumStats();
    }
}

// Function to handle DNA task on Blue tile
bool handleBlueTileTask(Player& player) {
    cout << "\n=== DNA Task 1: Similarity (Equal-Length) ===" << endl;
    cout << "Compare two DNA strands of equal length." << endl;
    
    string strand1, strand2;
    cout << "Enter first DNA strand (A, C, G, T only): ";
    getline(cin, strand1);
    cout << "Enter second DNA strand (same length): ";
    getline(cin, strand2);
    
    if (strand1.length() != strand2.length()) {
        cout << "Error: Strands must be equal length!" << endl;
        return false;
    }
    
    double similarity = strandSimilarity(strand1, strand2);
    cout << "Similarity score: " << similarity << endl;
    
    if (similarity >= 0.7) {
        cout << "Excellent match! You gain 200 Discovery Points!" << endl;
        player.updateDiscoverPoints(200);
        player.enforceMinimumStats();
        return true;
    } else if (similarity >= 0.5) {
        cout << "Good match! You gain 100 Discovery Points!" << endl;
        player.updateDiscoverPoints(100);
        player.enforceMinimumStats();
        return true;
    } else {
        cout << "Poor match. You lose 50 Discovery Points." << endl;
        player.updateDiscoverPoints(-50);
        player.enforceMinimumStats();
        return false;
    }
}

// Function to handle DNA task on Pink tile
bool handlePinkTileTask(Player& player) {
    cout << "\n=== DNA Task 2: Similarity (Unequal-Length) ===" << endl;
    cout << "Find the best alignment between two DNA strands." << endl;
    
    string input_strand, target_strand;
    cout << "Enter input DNA strand: ";
    getline(cin, input_strand);
    cout << "Enter target DNA strand: ";
    getline(cin, target_strand);
    
    int bestIndex = bestStrandMatch(input_strand, target_strand);
    
    if (bestIndex < 0) {
        cout << "Error: Invalid strands!" << endl;
        return false;
    }
    
    cout << "Best match found at index: " << bestIndex << endl;
    
    // Calculate similarity at best position
    string shorter = input_strand;
    string longer = target_strand;
    if (input_strand.length() > target_strand.length()) {
        shorter = target_strand;
        longer = input_strand;
    }
    
    int matches = 0;
    for (int i = 0; i < (int)shorter.length() && (bestIndex + i) < (int)longer.length(); i++) {
        if (shorter[i] == longer[bestIndex + i]) {
            matches++;
        }
    }
    double similarity = (double)matches / (double)shorter.length();
    cout << "Similarity at best position: " << similarity << endl;
    
    if (similarity >= 0.7) {
        cout << "Excellent alignment! You gain 200 Discovery Points!" << endl;
        player.updateDiscoverPoints(200);
        player.enforceMinimumStats();
        return true;
    } else if (similarity >= 0.5) {
        cout << "Good alignment! You gain 100 Discovery Points!" << endl;
        player.updateDiscoverPoints(100);
        player.enforceMinimumStats();
        return true;
    } else {
        cout << "Poor alignment. You lose 50 Discovery Points." << endl;
        player.updateDiscoverPoints(-50);
        player.enforceMinimumStats();
        return false;
    }
}

// Function to handle DNA task on Red tile
bool handleRedTileTask(Player& player) {
    cout << "\n=== DNA Task 3: Mutation Identification ===" << endl;
    cout << "Identify mutations between two DNA sequences." << endl;
    
    string input_strand, target_strand;
    cout << "Enter input DNA strand: ";
    getline(cin, input_strand);
    cout << "Enter target DNA strand: ";
    getline(cin, target_strand);
    
    cout << "\nMutations identified:" << endl;
    identifyMutations(input_strand, target_strand);
    
    cout << "\nChallenge completed! You gain 200 Discovery Points!" << endl;
    player.updateDiscoverPoints(200);
    player.enforceMinimumStats();
    return true;
}

// Function to handle DNA task on Brown tile
void handleBrownTileTask(Player& player) {
    cout << "\n=== DNA Task 4: Transcribe DNA to RNA ===" << endl;
    cout << "Convert a DNA sequence to RNA." << endl;
    
    string strand;
    cout << "Enter DNA strand: ";
    getline(cin, strand);
    
    transcribeDNAtoRNA(strand);
    
    cout << "\nTranscription completed! You gain 150 Discovery Points!" << endl;
    player.updateDiscoverPoints(150);
    player.enforceMinimumStats();
}

// Function to handle tile events
void handleTileEvent(Board& board, Player& player, int playerIndex, GameData& gameData) {
    int pos = player.getPosition();
    char tileColor = board.getTileColor(playerIndex, pos);
    
    cout << "\n=== TILE EVENT ===" << endl;
    
    switch (tileColor) {
        case 'G': // Green - Regular tile, nothing special
            cout << "You landed on a regular tile. Nothing happens." << endl;
            break;
            
        case 'B': // Blue - Training Fellowship (DNA Task 1)
            cout << "You landed on a Blue tile (Training Fellowship)!" << endl;
            handleBlueTileTask(player);
            triggerRandomEvent(player, 'B', gameData);
            break;
            
        case 'P': // Pink - Direct Lab Assignment (DNA Task 2)
            cout << "You landed on a Pink tile (Direct Lab Assignment)!" << endl;
            handlePinkTileTask(player);
            triggerRandomEvent(player, 'P', gameData);
            break;
            
        case 'R': // Red - Challenge tile (DNA Task 3)
            cout << "You landed on a Red tile (Challenge)!" << endl;
            if (handleRedTileTask(player)) {
                cout << "Challenge completed successfully!" << endl;
            }
            triggerRandomEvent(player, 'R', gameData);
            break;
            
        case 'T': // Brown - Special event (DNA Task 4)
            cout << "You landed on a Brown tile (Special Event)!" << endl;
            handleBrownTileTask(player);
            triggerRandomEvent(player, 'T', gameData);
            break;
            
        case 'U': { // Purple - Bonus tile
            cout << "You landed on a Purple tile (Bonus)!" << endl;
            int bonus = 300 + (rand() % 201); // 300-500 bonus
            cout << "You gain " << bonus << " Discovery Points!" << endl;
            player.updateDiscoverPoints(bonus);
            player.enforceMinimumStats();
            triggerRandomEvent(player, 'U', gameData);
            break;
        }
            
        case 'O': // Orange - Finish line
            cout << "Congratulations! You reached the finish line!" << endl;
            break;
            
        default:
            break;
    }
}

// Display character selection menu
void displayCharacterMenu(GameData& gameData, vector<bool>& chosen) {
    cout << "\n=== Available Characters ===" << endl;
    for (int i = 0; i < (int)gameData.availableCharacters.size(); i++) {
        if (!chosen[i]) {
            Player p = gameData.availableCharacters[i];
            cout << (i + 1) << ". " << p.getCharacterName() 
                 << " - Exp: " << p.getExperience()
                 << ", Acc: " << p.getAccuracy()
                 << ", Eff: " << p.getEfficiency()
                 << ", Ins: " << p.getInsight()
                 << ", DP: " << p.getDiscoverPoints() << endl;
        }
    }
}

// Character selection function
Player selectCharacter(int playerNum, GameData& gameData, vector<bool>& chosen) {
    cout << "\n=== Player " << playerNum << " Character Selection ===" << endl;
    displayCharacterMenu(gameData, chosen);
    
    int choice;
    cout << "Enter the number of your chosen character: ";
    cin >> choice;
    cin.ignore(); // Clear newline
    
    while (choice < 1 || choice > (int)gameData.availableCharacters.size() || chosen[choice - 1]) {
        cout << "Invalid choice. Please select an available character: ";
        cin >> choice;
        cin.ignore();
    }
    
    chosen[choice - 1] = true;
    Player selected = gameData.availableCharacters[choice - 1];
    cout << "You selected: " << selected.getCharacterName() << endl;
    return selected;
}

// Path type selection function
void selectPathType(Player& player) {
    cout << "\n=== Path Type Selection ===" << endl;
    cout << "Choose your path:" << endl;
    cout << "1. Training Fellowship" << endl;
    cout << "   - Cost: -5,000 Discovery Points" << endl;
    cout << "   - Bonus: +500 Accuracy, +500 Efficiency, +1,000 Insight" << endl;
    cout << "   - Includes advisor selection" << endl;
    cout << "2. Direct Lab Assignment" << endl;
    cout << "   - Bonus: +5,000 Discovery Points" << endl;
    cout << "   - Bonus: +200 Accuracy, +200 Efficiency, +200 Insight" << endl;
    cout << "   - No advisor" << endl;
    cout << "Enter your choice (1 or 2): ";
    
    int choice;
    cin >> choice;
    cin.ignore();
    
    while (choice != 1 && choice != 2) {
        cout << "Invalid choice. Please enter 1 or 2: ";
        cin >> choice;
        cin.ignore();
    }
    
    if (choice == 1) {
        // Training Fellowship
        player.setPathType(0);
        player.updateDiscoverPoints(-5000);
        player.updateAccuracy(500);
        player.updateEfficiency(500);
        player.updateInsight(1000);
        player.enforceMinimumStats();
        cout << "You chose Training Fellowship!" << endl;
    } else {
        // Direct Lab Assignment
        player.setPathType(1);
        player.updateDiscoverPoints(5000);
        player.updateAccuracy(200);
        player.updateEfficiency(200);
        player.updateInsight(200);
        player.enforceMinimumStats();
        cout << "You chose Direct Lab Assignment!" << endl;
    }
}

// Advisor selection function
void selectAdvisor(Player& player) {
    if (player.getPathType() != 0) {
        return; // Only Training Fellowship gets advisors
    }
    
    cout << "\n=== Advisor Selection ===" << endl;
    cout << "Choose your advisor:" << endl;
    cout << "1. Dr. Aliquot - Master of the 'wet lab', assists in avoiding contamination" << endl;
    cout << "2. Dr. Assembler - Expert who helps improve efficiency and streamlines pipelines" << endl;
    cout << "3. Dr. Pop-Gen - Genetics specialist with insight for identifying rare genetic variants" << endl;
    cout << "4. Dr. Bio-Script - Genius behind data analysis, helps debug code" << endl;
    cout << "5. Dr. Loci - Your biggest supporter assisting you in learning the equipment" << endl;
    cout << "Enter your choice (1-5): ";
    
    int choice;
    cin >> choice;
    cin.ignore();
    
    while (choice < 1 || choice > 5) {
        cout << "Invalid choice. Please enter 1-5: ";
        cin >> choice;
        cin.ignore();
    }
    
    player.setAdvisor(choice);
    cout << "You selected advisor " << choice << "!" << endl;
}

// Display main menu with 5+ options
void displayMainMenu(Player& player) {
    cout << "\n=== Main Menu ===" << endl;
    cout << "1. Check Player Progress" << endl;
    cout << "2. Review Character" << endl;
    cout << "3. Check Position" << endl;
    cout << "4. Review Advisor" << endl;
    cout << "5. Move Forward" << endl;
    cout << "Enter your choice (1-5): ";
}

// Handle main menu choice with secondary layers
int handleMenuChoice(Player& player, Board& board, int playerIndex) {
    string choice;
    getline(cin, choice);
    
    if (choice == "1") {
        // Check Player Progress - secondary layer
        cout << "\n=== Player Progress ===" << endl;
        cout << "1. Review Discover Points" << endl;
        cout << "2. Review Trait Stats" << endl;
        cout << "Enter your choice (1 or 2): ";
        string subChoice;
        getline(cin, subChoice);
        
        if (subChoice == "1") {
            cout << "Discovery Points: " << player.getDiscoverPoints() << endl;
        } else if (subChoice == "2") {
            cout << "Accuracy: " << player.getAccuracy() << endl;
            cout << "Efficiency: " << player.getEfficiency() << endl;
            cout << "Insight: " << player.getInsight() << endl;
            cout << "Experience: " << player.getExperience() << endl;
        } else {
            cout << "Invalid choice." << endl;
        }
        return 1;
    } else if (choice == "2") {
        // Review Character
        cout << "\n=== Character Information ===" << endl;
        cout << "Character Name: " << player.getCharacterName() << endl;
        cout << "Experience: " << player.getExperience() << endl;
        return 1;
    } else if (choice == "3") {
        // Check Position
        cout << "\n=== Current Position ===" << endl;
        cout << "Position: " << player.getPosition() << " / 51" << endl;
        cout << "\n=== Board State ===" << endl;
        board.displayBoard();
        return 1;
    } else if (choice == "4") {
        // Review Advisor - secondary layer
        if (player.getAdvisor() == 0) {
            cout << "\nYou do not have an advisor (Direct Lab Assignment path)." << endl;
        } else {
            cout << "\n=== Advisor Information ===" << endl;
            cout << "Advisor Number: " << player.getAdvisor() << endl;
            string advisorNames[] = {"", "Dr. Aliquot", "Dr. Assembler", "Dr. Pop-Gen", "Dr. Bio-Script", "Dr. Loci"};
            string advisorAbilities[] = {"", 
                "Master of the 'wet lab', assists in avoiding contamination",
                "Expert who helps improve efficiency and streamlines pipelines",
                "Genetics specialist with insight for identifying rare genetic variants",
                "Genius behind data analysis, helps debug code",
                "Your biggest supporter assisting you in learning the equipment"};
            
            cout << "Advisor Name: " << advisorNames[player.getAdvisor()] << endl;
            cout << "1. Display advisor abilities" << endl;
            cout << "2. Use abilities for challenge" << endl;
            cout << "Enter your choice (1 or 2): ";
            string subChoice;
            getline(cin, subChoice);
            
            if (subChoice == "1") {
                cout << "Ability: " << advisorAbilities[player.getAdvisor()] << endl;
            } else if (subChoice == "2") {
                cout << "Your advisor is ready to protect you from negative events!" << endl;
            } else {
                cout << "Invalid choice." << endl;
            }
        }
        return 1;
    } else if (choice == "5") {
        return 2; // Move Forward
    } else {
        cout << "Invalid choice. Please enter 1-5." << endl;
        return 0;
    }
}

// Write game stats to file
void writeGameStats(Player player1, Player player2, string filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cout << "Warning: Could not write game stats to file." << endl;
        return;
    }
    
    file << "=== Journey Through Genome - Game Statistics ===" << endl;
    file << endl;
    file << "Player 1:" << endl;
    file << "  Character: " << player1.getCharacterName() << endl;
    file << "  Experience: " << player1.getExperience() << endl;
    file << "  Accuracy: " << player1.getAccuracy() << endl;
    file << "  Efficiency: " << player1.getEfficiency() << endl;
    file << "  Insight: " << player1.getInsight() << endl;
    file << "  Discovery Points: " << player1.getDiscoverPoints() << endl;
    file << "  Final Position: " << player1.getPosition() << endl;
    file << endl;
    file << "Player 2:" << endl;
    file << "  Character: " << player2.getCharacterName() << endl;
    file << "  Experience: " << player2.getExperience() << endl;
    file << "  Accuracy: " << player2.getAccuracy() << endl;
    file << "  Efficiency: " << player2.getEfficiency() << endl;
    file << "  Insight: " << player2.getInsight() << endl;
    file << "  Discovery Points: " << player2.getDiscoverPoints() << endl;
    file << "  Final Position: " << player2.getPosition() << endl;
    
    file.close();
    cout << "Game statistics written to " << filename << endl;
}

// Calculate final Discover Points with trait bonuses
int calculateFinalDiscoverPoints(Player player) {
    int finalDP = player.getDiscoverPoints();
    
    // Add 1000 DP for every 100 points in Accuracy, Efficiency, or Insight
    finalDP = finalDP + (player.getAccuracy() / 100) * 1000;
    finalDP = finalDP + (player.getEfficiency() / 100) * 1000;
    finalDP = finalDP + (player.getInsight() / 100) * 1000;
    
    return finalDP;
}

int main() {
    // Seed the random number generator
    srand(time(nullptr));
    
    // Initialize game data structure (replaces globals)
    GameData gameData;
    
    // Load game data
    cout << "Loading game data..." << endl;
    if (!loadCharacters("characters.txt", gameData)) {
        cout << "Warning: Could not load characters.txt. Using default characters." << endl;
        // Add default characters
        gameData.availableCharacters.push_back(Player("Dr.Leo", 5, 500, 500, 1000, 20000));
        gameData.availableCharacters.push_back(Player("Dr.Helix", 8, 900, 600, 600, 20000));
    }
    if (!loadRiddles("riddles.txt", gameData)) {
        cout << "Warning: Could not load riddles.txt." << endl;
    }
    if (!loadRandomEvents("random_events.txt", gameData)) {
        cout << "Warning: Could not load random_events.txt." << endl;
    }
    
    // Initialize game board
    Board gameBoard;
    
    // Character selection
    cout << "\n=== Journey Through Genome ===" << endl;
    vector<bool> chosen(gameData.availableCharacters.size(), false);
    
    Player player1 = selectCharacter(1, gameData, chosen);
    selectPathType(player1);
    if (player1.getPathType() == 0) {
        selectAdvisor(player1);
    }
    
    Player player2 = selectCharacter(2, gameData, chosen);
    selectPathType(player2);
    if (player2.getPathType() == 0) {
        selectAdvisor(player2);
    }
    
    // Initialize board positions
    gameBoard.setPlayerPosition(0, 0);
    gameBoard.setPlayerPosition(1, 0);
    
    cout << "\n=== Game Starting! ===" << endl;
    
    // Main Game Loop
    bool game_over = false;
    int turn = 1;
    bool player1Finished = false;
    bool player2Finished = false;
    
    while (!game_over) {
        // Determine which player's turn it is
        Player& currentPlayer = (turn % 2 != 0) ? player1 : player2;
        int currentPlayerIndex = (turn % 2 != 0) ? 0 : 1;
        
        // Skip turn if player already finished
        if ((turn % 2 != 0 && player1Finished) || (turn % 2 == 0 && player2Finished)) {
            turn++;
            continue;
        }
        
        cout << "\n========================================" << endl;
        cout << "--- Player " << (turn % 2 != 0 ? 1 : 2) << "'s Turn (" 
             << currentPlayer.getCharacterName() << ") ---" << endl;
        cout << "========================================" << endl;
        
        // 1. Display Main Menu and handle choice
        int menuChoice = 0;
        while (menuChoice != 2) {
            displayMainMenu(currentPlayer);
            menuChoice = handleMenuChoice(currentPlayer, gameBoard, currentPlayerIndex);
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
            handleTileEvent(gameBoard, currentPlayer, currentPlayerIndex, gameData);
        }
        
        // 5. Check if player reached finish
        if (currentPlayer.getPosition() >= 51) {
            if (turn % 2 != 0) {
                player1Finished = true;
                cout << "\nPlayer 1 reached the finish line!" << endl;
            } else {
                player2Finished = true;
                cout << "\nPlayer 2 reached the finish line!" << endl;
            }
        }
        
        // 6. Check End Condition (ALL players must reach finish)
        if (player1Finished && player2Finished) {
            game_over = true;
            
            // Calculate final Discover Points
            int finalDP1 = calculateFinalDiscoverPoints(player1);
            int finalDP2 = calculateFinalDiscoverPoints(player2);
            
            cout << "\n========================================" << endl;
            cout << "GAME OVER!" << endl;
            cout << "========================================" << endl;
            cout << "\nFinal Results:" << endl;
            cout << "Player 1 (" << player1.getCharacterName() << "):" << endl;
            cout << "  Base Discovery Points: " << player1.getDiscoverPoints() << endl;
            cout << "  Final Discovery Points (with trait bonuses): " << finalDP1 << endl;
            cout << "\nPlayer 2 (" << player2.getCharacterName() << "):" << endl;
            cout << "  Base Discovery Points: " << player2.getDiscoverPoints() << endl;
            cout << "  Final Discovery Points (with trait bonuses): " << finalDP2 << endl;
            cout << "\n";
            
            if (finalDP1 > finalDP2) {
                cout << "Player 1 (" << player1.getCharacterName() 
                     << ") wins with " << finalDP1 << " Discovery Points!" << endl;
            } else if (finalDP2 > finalDP1) {
                cout << "Player 2 (" << player2.getCharacterName() 
                     << ") wins with " << finalDP2 << " Discovery Points!" << endl;
            } else {
                cout << "It's a tie! Both players have " << finalDP1 << " Discovery Points!" << endl;
            }
            cout << "========================================" << endl;
            
            // Write game stats to file
            writeGameStats(player1, player2, "game_stats.txt");
        }
        
        turn++;
    }
    
    return 0;
}
