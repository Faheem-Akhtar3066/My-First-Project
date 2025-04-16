#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <chrono>
#include <thread>
#include <ctime>
#include <cstdlib>
#include <algorithm>

namespace WordGame {

    const int MAX_WORDS = 100; // Maximum number of words
    const int MAX_WORD_LENGTH = 50; // Maximum length of each word
    const int MAX_CHANCES = 5; // Maximum chances per game
    const int GRID_SIZES[] = { 10, 15, 20 }; // Grid sizes for levels 1, 2, 3
    const int NUM_DIRECTIONS = 8; // Number of search directions

    //-------------------------------------------------------
    // Game class manages the main game functionalities.
    // Includes a default constructor and public member
    // functions for displaying menus, instructions, scores,
    // and starting the game.
    //-------------------------------------------------------
    class Game {
    private:
        int chances;

    public:
        Game(); // Default constructor to initialize game settings
        void displayMenu(); // Displays the main menu
        void displayInstructions(); // Shows game instructions
        void HighestScore(); // Displays the highest scores
        void AboutUs(); // Provides information about developers
        void startGame(); // Starts the game logic
    };

    //-------------------------------------------------------
    // Abstract class GameMode manages the game grid and
    // provides functions for grid printing, populating,
    // word count, and word length based on difficulty levels.
    //-------------------------------------------------------
    class GameMode {
    protected:
        char** grid; // Pointer to a 2D character grid
        int gridSize; // Size of the grid
        std::string wordList[MAX_WORDS]; // List of words in the game
        int wordCount; // Total words used in the grid
        int level; // Current level

    public:
        GameMode() : grid(nullptr), gridSize(0), wordCount(0), level(1) {} // Constructor initializes variables
        virtual ~GameMode() { clearGrid(); } // Destructor clears grid memory
        virtual void Grid(const std::string& level, const std::string& filename) = 0; // Sets up the grid based on level
        virtual int getWordLength(int level) const = 0; // Gets word length for a level
        virtual int getWordCount(int level) const = 0; // Gets word count for a level

        char** getGrid() const { return grid; }
        int getGridSize() const { return gridSize; } // Returns grid size
        void clearGrid(); // Clears dynamically allocated grid memory
        void printGrid(); // Prints the grid to the console
        void allocateGrid(int size); // Allocates memory for the grid
        void populateGrid(const std::string words[], int totalWords, int targetWordCount, int targetWordLength); // Fills grid with words
    };

    //-------------------------------------------------------
    // EasyMode class inherits from GameMode and overrides
    // functions to set up the grid, word count, and word
    // length specifically for easy mode.
    //-------------------------------------------------------
    class EasyMode : public GameMode {
    public:
        void Grid(const std::string& level, const std::string& filename) override; // Sets up grid for easy mode
        int getWordLength(int level) const override; // Returns word length
        int getWordCount(int level) const override; // Returns word count
    };

    //-------------------------------------------------------
    // HardMode class inherits from GameMode and overrides
    // functions to set up the grid, word count, and word
    // length specifically for hard mode.
    //-------------------------------------------------------
    class HardMode : public GameMode {
    public:
        void Grid(const std::string& level, const std::string& filename) override; // Sets up grid for hard mode
        int getWordLength(int level) const override; // Returns word length
        int getWordCount(int level) const override; // Returns word count
    };

    //-------------------------------------------------------
    // WordValidator class validates words by checking
    // their presence in the dictionary or grid.
    //-------------------------------------------------------
    class WordValidator {
    public:
        bool isValidWord(const std::string& word, const std::string dictionary[], int dictionarySize); // Checks if word exists in dictionary
        bool checkWordInGrid(std::string word, char** grid, int gridSize); // Searches word in grid
        void loadWords(const std::string& filename, std::string words[], int& wordSize); // Loads words from file
    };

    //-------------------------------------------------------
    // ScoreTracker class tracks and manages scores for the
    // game.
    //-------------------------------------------------------
    class ScoreTracker {
    public:
        ScoreTracker() : score(0) { std::fill(topScores, topScores + 5, 0); } // Initializes with zero score
        void updateScore(bool correctGuess); // Updates score based on guesses
        void loadScores(const std::string& filename); // Loads scores from file
        void saveScores(const std::string& filename); // Saves scores to file
        int getScore() const; // Gets current score
        int getTopScore(int index) const; // Gets top score from list
        void compareAndUpdateScores(const std::string& filename); // Updates high scores

    private:
        int score; // Current score
        int topScores[5]; // Top 5 scores
    };

} // namespace WordGame