#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib> 
#include <ctime>  
#include <vector>
#include <sstream>
#include "Player.h"
#include "Board.h"

using namespace std;

// store question and answer for riddles
struct Riddle {
    string question;
    string answer;
};

// store random event info with description, path type, advisor, and discovery points change
struct RandomEvent {
    string description;      
    int pathType;            
    int advisor;             
    int discoveryPoints;    
};

// hold all game data loaded from files
struct GameData {
    vector<Player> availableCharacters;  
    vector<Riddle> riddles;              
    vector<RandomEvent> randomEvents;   
};

// open file, skip header, read each line, parse pipe-delimited values, create player objects, add to vector
bool loadCharacters(string filename, GameData& gameData) {
    ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    string line;
    getline(file, line); 
    
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

// open file, skip header, read each line, find pipe separator, split into question and answer, add to vector
bool loadRiddles(string filename, GameData& gameData) {
    ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    string line;
    getline(file, line); 
    
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

// open file, skip header and comment lines, read each line, parse pipe-delimited values, convert to ints, add to vector
bool loadRandomEvents(string filename, GameData& gameData) {
    ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    string line;
    getline(file, line);
    getline(file, line);
    
    while (getline(file, line)) {
        if (line.empty() || line[0] == '/') continue;
        
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

// if strands different length or empty return 0, else count matches at each position, return matches divided by total
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

// if either empty return -1, if input shorter compare from start return 0, if input longer slide target along input, find best match position, return index
int bestStrandMatch(string input_strand, string target_strand) {
    if (input_strand.length() == 0 || target_strand.length() == 0) {
        return -1;
    }
    
    double bestScore = 0.0;
    int bestIndex = 0;
    
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
        return 0;
    }
    
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
            bestIndex = i;
        }
    }
    
    return bestIndex;
}

// find best alignment, determine shorter and longer strand, compare character by character, detect substitutions insertions deletions, print each mutation, handle remaining chars
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
            if (inputPos + 1 < (int)shorter.length() && targetPos + 1 < (int)longer.length() &&
                shorter[inputPos + 1] == longer[targetPos + 1]) {
                cout << "Substitution at position " << inputPos << ": " 
                     << shorter[inputPos] << " -> " << longer[targetPos] << endl;
                inputPos++;
                targetPos++;
            }
            else if (inputPos < (int)shorter.length() && targetPos + 1 < (int)longer.length() &&
                     shorter[inputPos] == longer[targetPos + 1]) {
                cout << "Insertion at position " << targetPos << ": " 
                     << longer[targetPos] << " inserted" << endl;
                targetPos++;
            }
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

// loop through strand, if char is T replace with U, else keep same, print both dna and rna
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

// if no riddles return true, pick random riddle, ask question, get answer, compare lowercase versions, award or deduct points
bool askRiddle(Player& player, GameData& gameData) {
    if (gameData.riddles.size() == 0) {
        return true;
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

// filter events by tile color and path type, pick random valid event, check if advisor protects, apply discovery points change
void triggerRandomEvent(Player& player, char tileColor, GameData& gameData) {
    if (gameData.randomEvents.size() == 0) {
        return;
    }
    
    vector<RandomEvent> validEvents;
    
    for (int i = 0; i < (int)gameData.randomEvents.size(); i++) {
        RandomEvent event = gameData.randomEvents[i];
        bool isValid = false;
        
        if (tileColor == 'B') {
            isValid = (event.pathType == 0);
        } else if (tileColor == 'P') {
            isValid = (event.pathType == 1);
        } else {
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

// get two dna strands from user, check equal length, calculate similarity, award points based on score
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

// get two dna strands from user, find best match position, calculate similarity at that position, award points based on score
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

// get two dna strands from user, call identify mutations, award points
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

// get dna strand from user, transcribe to rna, award points
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

// get tile color at player position, switch on color, call appropriate handler or do nothing, trigger random event
void handleTileEvent(Board& board, Player& player, int playerIndex, GameData& gameData) {
    int pos = player.getPosition();
    char tileColor = board.getTileColor(playerIndex, pos);
    
    cout << "\n=== TILE EVENT ===" << endl;
    
    switch (tileColor) {
        case 'G':
            cout << "You landed on a regular tile. Nothing happens." << endl;
            break;
            
        case 'B':
            cout << "You landed on a Blue tile (Training Fellowship)!" << endl;
            handleBlueTileTask(player);
            triggerRandomEvent(player, 'B', gameData);
            break;
            
        case 'P':
            cout << "You landed on a Pink tile (Direct Lab Assignment)!" << endl;
            handlePinkTileTask(player);
            triggerRandomEvent(player, 'P', gameData);
            break;
            
        case 'R':
            cout << "You landed on a Red tile (Challenge)!" << endl;
            if (handleRedTileTask(player)) {
                cout << "Challenge completed successfully!" << endl;
            }
            triggerRandomEvent(player, 'R', gameData);
            break;
            
        case 'T':
            cout << "You landed on a Brown tile (Special Event)!" << endl;
            handleBrownTileTask(player);
            triggerRandomEvent(player, 'T', gameData);
            break;
            
        case 'U': {
            cout << "You landed on a Purple tile (Bonus)!" << endl;
            int bonus = 300 + (rand() % 201);
            cout << "You gain " << bonus << " Discovery Points!" << endl;
            player.updateDiscoverPoints(bonus);
            player.enforceMinimumStats();
            triggerRandomEvent(player, 'U', gameData);
            break;
        }
            
        case 'O':
            cout << "Congratulations! You reached the finish line!" << endl;
            break;
            
        default:
            break;
    }
}

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

// show menu, get choice, validate choice, mark character as chosen, return selected player
Player selectCharacter(int playerNum, GameData& gameData, vector<bool>& chosen) {
    cout << "\n=== Player " << playerNum << " Character Selection ===" << endl;
    displayCharacterMenu(gameData, chosen);
    
    int choice;
    cout << "Enter the number of your chosen character: ";
    cin >> choice;
    cin.ignore();
    
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

// show path options, get choice, apply stat changes based on choice, set path type
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
        player.setPathType(0);
        player.updateDiscoverPoints(-5000);
        player.updateAccuracy(500);
        player.updateEfficiency(500);
        player.updateInsight(1000);
        player.enforceMinimumStats();
        cout << "You chose Training Fellowship!" << endl;
    } else {
        player.setPathType(1);
        player.updateDiscoverPoints(5000);
        player.updateAccuracy(200);
        player.updateEfficiency(200);
        player.updateInsight(200);
        player.enforceMinimumStats();
        cout << "You chose Direct Lab Assignment!" << endl;
    }
}

// if not training fellowship return, show advisor options, get choice, set advisor
void selectAdvisor(Player& player) {
    if (player.getPathType() != 0) {
        return;
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

void displayMainMenu(Player& player) {
    cout << "\n=== Main Menu ===" << endl;
    cout << "1. Check Player Progress" << endl;
    cout << "2. Review Character" << endl;
    cout << "3. Check Position" << endl;
    cout << "4. Review Advisor" << endl;
    cout << "5. Move Forward" << endl;
    cout << "Enter your choice (1-5): ";
}

// get menu choice, handle each option with submenus where needed, return choice code
int handleMenuChoice(Player& player, Board& board, int playerIndex) {
    string choice;
    getline(cin, choice);
    
    if (choice == "1") {
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
        cout << "\n=== Character Information ===" << endl;
        cout << "Character Name: " << player.getCharacterName() << endl;
        cout << "Experience: " << player.getExperience() << endl;
        return 1;
    } else if (choice == "3") {
        cout << "\n=== Current Position ===" << endl;
        cout << "Position: " << player.getPosition() << " / 51" << endl;
        cout << "\n=== Board State ===" << endl;
        board.displayBoard();
        return 1;
    } else if (choice == "4") {
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
        return 2;
    } else {
        cout << "Invalid choice. Please enter 1-5." << endl;
        return 0;
    }
}

// open file, write player stats for both players, close file
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

// start with base discovery points, add 1000 for every 100 points in accuracy efficiency and insight
int calculateFinalDiscoverPoints(Player player) {
    int finalDP = player.getDiscoverPoints();
    
    finalDP = finalDP + (player.getAccuracy() / 100) * 1000;
    finalDP = finalDP + (player.getEfficiency() / 100) * 1000;
    finalDP = finalDP + (player.getInsight() / 100) * 1000;
    
    return finalDP;
}

// seed random, load game data, initialize board, let players select characters and paths, game loop: alternate turns, show menu, roll dice, move, display board, handle tile events, check win condition, calculate final scores, write stats
int main() {
    srand(time(nullptr));
    
    GameData gameData;
    
    cout << "Loading game data..." << endl;
    if (!loadCharacters("characters.txt", gameData)) {
        cout << "Warning: Could not load characters.txt. Using default characters." << endl;
        gameData.availableCharacters.push_back(Player("Dr.Leo", 5, 500, 500, 1000, 20000));
        gameData.availableCharacters.push_back(Player("Dr.Helix", 8, 900, 600, 600, 20000));
    }
    if (!loadRiddles("riddles.txt", gameData)) {
        cout << "Warning: Could not load riddles.txt." << endl;
    }
    if (!loadRandomEvents("random_events.txt", gameData)) {
        cout << "Warning: Could not load random_events.txt." << endl;
    }
    
    Board gameBoard;
    
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
    
    gameBoard.setPlayerPosition(0, 0);
    gameBoard.setPlayerPosition(1, 0);
    
    cout << "\n=== Game Starting! ===" << endl;
    
    bool game_over = false;
    int turn = 1;
    bool player1Finished = false;
    bool player2Finished = false;
    
    while (!game_over) {
        Player& currentPlayer = (turn % 2 != 0) ? player1 : player2;
        int currentPlayerIndex = (turn % 2 != 0) ? 0 : 1;
        
        if ((turn % 2 != 0 && player1Finished) || (turn % 2 == 0 && player2Finished)) {
            turn++;
            continue;
        }
        
        cout << "\n========================================" << endl;
        cout << "--- Player " << (turn % 2 != 0 ? 1 : 2) << "'s Turn (" 
             << currentPlayer.getCharacterName() << ") ---" << endl;
        cout << "========================================" << endl;
        
        int menuChoice = 0;
        while (menuChoice != 2) {
            displayMainMenu(currentPlayer);
            menuChoice = handleMenuChoice(currentPlayer, gameBoard, currentPlayerIndex);
            if (menuChoice == 0) {
                continue;
            }
        }
        
        cout << "\nRolling the dice..." << endl;
        int steps = rand() % 6 + 1;
        cout << "You rolled: " << steps << endl;
        
        int oldPosition = currentPlayer.getPosition();
        currentPlayer.updatePosition(steps);
        int newPosition = currentPlayer.getPosition();
        
        cout << "Moving from position " << oldPosition << " to position " << newPosition << endl;
        
        gameBoard.setPlayerPosition(0, player1.getPosition());
        gameBoard.setPlayerPosition(1, player2.getPosition());
        
        cout << "\n=== Current Board State ===" << endl;
        gameBoard.displayBoard();
        
        if (newPosition != oldPosition && newPosition < 51) {
            handleTileEvent(gameBoard, currentPlayer, currentPlayerIndex, gameData);
        }
        
        if (currentPlayer.getPosition() >= 51) {
            if (turn % 2 != 0) {
                player1Finished = true;
                cout << "\nPlayer 1 reached the finish line!" << endl;
            } else {
                player2Finished = true;
                cout << "\nPlayer 2 reached the finish line!" << endl;
            }
        }
        
        if (player1Finished && player2Finished) {
            game_over = true;
            
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
            
            writeGameStats(player1, player2, "game_stats.txt");
        }
        
        turn++;
    }
    
    return 0;
}
