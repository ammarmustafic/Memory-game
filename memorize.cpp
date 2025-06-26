#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <chrono>
#include <thread>
#include <random>

using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::vector;

struct Player {
    string name;
    int score;

    bool operator<(const Player& other) const {
        return score < other.score;
    }
};

class MemoryGame {
public:
    MemoryGame() : m_rng(std::chrono::steady_clock::now().time_since_epoch().count()) {
        loadLeaderboard();
    }

    void run() {
        displayWelcomeScreen();
        getPlayerName();
        gameLoop();
        displayGoodbyeScreen();
    }

private:
    vector<Player> m_players;
    string m_currentPlayerName;
    int m_score = 0;
    
    std::mt19937 m_rng;
    void displayWelcomeScreen() const {
        clearScreen();
        cout << "========================================" << endl;
        cout << "||        ADVANCED MEMORY GAME        ||" << endl;
        cout << "========================================" << endl;
        cout << endl;
    }

    void getPlayerName() {
        cout << "Enter your operator handle (name): ";
        string name;
        while (std::getline(cin, name)) {
            if (!name.empty() && !playerExists(name)) {
                m_currentPlayerName = name;
                break;
            }
            if (name.empty()) {
                cout << "Handle cannot be empty. Please try again: ";
            } else {
                cout << "Handle already taken. Choose another: ";
            }
        }
        cout << "Welcome, operator " << m_currentPlayerName << ". Initializing sequence..." << endl;
        sleep(2000);
    }

    void gameLoop() {
        int level = 1;
        int timeLimit = 4000;
        while (true) {
            clearScreen();
            cout << "Level: " << level << " | Score: " << m_score << endl;
            cout << "---------------------------------" << endl;

            string challenge = generateChallenge(level);
            
            cout << "Memorize sequence: " << challenge << endl;
            countdown(timeLimit);
            clearScreen();

            cout << "Enter the sequence: ";
            string guess;
            std::getline(cin, guess);

            if (guess == challenge) {
                cout << "Correct! Sequence matched." << endl;
                m_score += level * 10;
                level++;
                timeLimit = std::max(1000, timeLimit - 250);
                sleep(1500);
            } else {
                cout << "Incorrect. Sequence terminated." << endl;
                cout << "The correct sequence was: " << challenge << endl;
                cout << "Your final score: " << m_score << endl;
                sleep(3000);
                break;
            }
        }
        updateLeaderboard();
    }

    string generateChallenge(int length) {
        std::uniform_int_distribution<int> dist(0, 1);
        string challenge;

        if (dist(m_rng) == 0) {
            std::uniform_int_distribution<int> num_dist(0, 9);
            for (int i = 0; i < length; ++i) {
                challenge += std::to_string(num_dist(m_rng));
            }
        } else { 
            string letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
            std::shuffle(letters.begin(), letters.end(), m_rng);
            challenge = letters.substr(0, length);
        }
        return challenge;
    }

    void countdown(int milliseconds) const {
        int steps = 10;
        int step_duration = milliseconds / steps;
        for (int i = steps; i > 0; --i) {
            cout << "\rTime remaining: [" << string(i, '#') << string(steps - i, ' ') << "]" << std::flush;
            sleep(step_duration);
        }
        cout << endl;
    }


    void loadLeaderboard() {
        std::ifstream file("leaderboard.dat");
        if (!file) return;

        Player p;
        while (file >> p.name >> p.score) {
            m_players.push_back(p);
        }
    }

    void updateLeaderboard() {
        m_players.push_back({m_currentPlayerName, m_score});
        std::sort(m_players.rbegin(), m_players.rend()); 

        if (m_players.size() > 10) {
            m_players.resize(10);
        }

        std::ofstream file("leaderboard.dat");
        for (const auto& player : m_players) {
            file << player.name << " " << player.score << endl;
        }
    }

    void printLeaderboard() const {
        cout << "\n------- LEADERBOARD (TOP 10) -------" << endl;
        cout << "Rank\tName\t\tScore" << endl;
        cout << "------------------------------------" << endl;
        int rank = 1;
        for (const auto& player : m_players) {
            cout << rank++ << "\t" << player.name << "\t\t" << player.score << endl;
        }
        cout << "------------------------------------" << endl;
    }
    
    bool playerExists(const string& name) const {
        for(const auto& p : m_players) {
            if (p.name == name) return true;
        }
        return false;
    }

    void displayGoodbyeScreen() const {
        clearScreen();
        printLeaderboard();
        cout << "\nSession for operator " << m_currentPlayerName << " terminated." << endl;
    }

    

    void clearScreen() const {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
    }
    
    void sleep(int milliseconds) const {
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    }
};


int main() {
    MemoryGame game;
    game.run();
    return 0;
}
