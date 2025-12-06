#ifndef PLAYER_H
#define PLAYER_H

#include <string>

using namespace std;

class Player {
private:
    string characterName;
    int experience;
    int accuracy;
    int efficiency;
    int insight;
    int discoverPoints;
    int position; // Current tile index on the board
    int pathType; // 0 = Training Fellowship, 1 = Direct Lab Assignment
    int advisor;  // 0 = none, 1-5 = different advisors

public:
    // Required constructor (Must not use 'this->')
    Player(string name, int exp, int acc, int eff, int ins, int dp) {
        characterName = name;
        experience = exp;
        accuracy = acc;
        efficiency = eff;
        insight = ins;
        discoverPoints = dp;
        position = 0; // Starts at tile 0
        pathType = -1; // Not set
        advisor = 0;   // No advisor
    }
    
    // Default Constructor (Optional but good practice)
    Player() {
        characterName = "Default";
        experience = 0;
        accuracy = 0;
        efficiency = 0;
        insight = 0;
        discoverPoints = 0;
        position = 0;
        pathType = -1; // Not set
        advisor = 0;   // No advisor
    }

    // --- Required Getters (Must return by value) ---
    string getCharacterName() {
        return characterName;
    }
    int getExperience() {
        return experience;
    }
    int getAccuracy() {
        return accuracy;
    }
    int getEfficiency() {
        return efficiency;
    }
    int getInsight() {
        return insight;
    }
    int getDiscoverPoints() {
        return discoverPoints;
    }
    int getPosition() {
        return position;
    }
    int getPathType() {
        return pathType;
    }
    int getAdvisor() {
        return advisor;
    }

    // --- Required Setters (Must not use pass-by-reference) ---
    void setCharacterName(string name) {
        characterName = name;
    }
    void updateExperience(int change) {
        experience = experience + change;
    }
    void updateAccuracy(int change) {
        accuracy = accuracy + change;
    }
    void updateEfficiency(int change) {
        efficiency = efficiency + change;
    }
    void updateInsight(int change) {
        insight = insight + change;
    }
    void updateDiscoverPoints(int change) {
        discoverPoints = discoverPoints + change;
    }
    void updatePosition(int steps) {
        position = position + steps;
        if (position > 51) { // Cap at the final tile
            position = 51; 
        }
    }
    void setPathType(int path) {
        pathType = path;
    }
    void setAdvisor(int adv) {
        advisor = adv;
    }
    void enforceMinimumStats() {
        if (accuracy < 100) accuracy = 100;
        if (efficiency < 100) efficiency = 100;
        if (insight < 100) insight = 100;
    }
};

#endif