#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <windows.h>
#include <cmath>
#include <algorithm>
#include <fstream>

using namespace std;


const int MAX_PLAYERS = 100;
const int MAX_DIGITS = 5;
const int MAX_TIME = 150;

struct Player {
    string name;
    int score;
};


void printLeaderboard(Player players[], int numPlayers) {
    cout << "------- Leaderboard -------" << endl;
    cout << "Rank\tName\t\tScore" << endl;
    sort(players, players+numPlayers, [](Player a, Player b){ return a.score > b.score; });
    for (int i = 0; i < numPlayers; i++) {
        cout << i+1 << "\t" << players[i].name << "\t\t" << players[i].score << endl;
    }
}


void loadPlayers(Player players[], int& numPlayers) {
    ifstream file("leaderboard.txt");
    if (file.is_open()) {
        string name;
        int score;
        while (file >> name >> score) {
            players[numPlayers].name = name;
            players[numPlayers].score = score;
            numPlayers++;
        }
        file.close();
    }
}

bool checkPlayerExists(Player players[], int numPlayers, string name) {
    for (int i = 0; i < numPlayers; i++) {
        if (players[i].name == name) {
            return true;
        }
    }
    return false;
}

void savePlayers(Player players[], int numPlayers) {
    ofstream file("leaderboard.txt");
    if (file.is_open()) {
        for (int i = 0; i < numPlayers; i++) {
            file << players[i].name << " " << players[i].score << endl;
        }
        file.close();
    }
}

int main() {
    srand(time(NULL));
    int level = 1;
    int score = 0;
    Player players[MAX_PLAYERS];
    int numPlayers = 0;
    string name;
    
    while (name.empty()) {
        cout << "Welcome to the memory game! What's your name?" << endl;
        getline(cin,name);
        system("CLS");
    }
    
    
    cout << "Hi " << name << "! Get ready." << endl;
    Sleep(800);
    system("CLS");
    
    loadPlayers(players, numPlayers); 
    
    while (checkPlayerExists(players, numPlayers, name)) {
        cout << "Username already exists in our database. Please enter a different name: ";
        getline(cin, name);
        Sleep(800);
    }
    
    int timeLimit = MAX_TIME;
    int numDigits = 1;
    while (true) {
    	
        bool isNumberGame = rand() % 2 == 0;
        // generate random number or letter string
        string randomStr;
        if (isNumberGame) {
            int num = rand() % (int)(pow(10, numDigits));
            randomStr = to_string(num);
        } else {
            string letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
            int letterCount = min(numDigits, 26);
            random_shuffle(letters.begin(), letters.end());
            randomStr = letters.substr(0, letterCount);
        }
        
        cout << "Memorize this " << (isNumberGame ? "number" : "letter") << ": " << randomStr << endl;
        auto start = chrono::steady_clock::now();
        Sleep(timeLimit);
        auto end = chrono::steady_clock::now();
        system("cls");
        
        // if time's up
        auto elapsed = chrono::duration_cast<chrono::milliseconds>(end - start);
        if (elapsed.count() >= timeLimit) {
            string guess;
            cout << "What was the " << (isNumberGame ? "number" : "letter") << "? ";
            getline(cin, guess);
            if (guess == randomStr) {
                score++;
                level++;
                cout << "Correct! Your score is now " << score << endl;
                numDigits = min(numDigits+1, MAX_DIGITS);
                timeLimit = max(timeLimit-10, 50);
            } else {
                cout << "Sorry, that's incorrect. Your final score is " << score << endl;
                players[numPlayers].name = name;
                players[numPlayers].score = score;
                numPlayers++;
                savePlayers(players, numPlayers);
                break;
            }
        }
    }

    printLeaderboard(players, numPlayers);
    return 0;
}
