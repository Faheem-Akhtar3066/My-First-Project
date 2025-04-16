#include "header.h"

namespace WordGame {

    //------------------------------------------------------------------------------
    // Game constructor
    //------------------------------------------------------------------------------
    Game::Game() : chances(MAX_CHANCES) {
        srand(static_cast<unsigned int>(time(0)));
    }

    //------------------------------------------------------------------------------
    // allocateGrid
    // Dynamically allocates memory for a 2D char grid of given size.
    // Throws runtime_error on failure.
    //------------------------------------------------------------------------------
    void GameMode::allocateGrid(int size) {
        try {
            gridSize = size;
            grid = new char* [gridSize];
            for (int i = 0; i < gridSize; ++i) {
                grid[i] = new char[gridSize];
            }
        }
        catch (const std::bad_alloc& e) {
            std::cerr << "Memory allocation failed: " << e.what() << std::endl;
            grid = nullptr;
            gridSize = 0;
            throw std::runtime_error("Failed to allocate grid memory");
        }
    }

    //------------------------------------------------------------------------------
    // populateGrid
    // Fills the grid with words from the provided list, respecting targetWordCount and targetWordLength.
    // Remaining cells are filled with random letters, ensuring no unintended words are formed.
    //------------------------------------------------------------------------------
    void GameMode::populateGrid(const std::string words[], int totalWords, int targetWordCount, int targetWordLength) {
        // Clear the grid
        for (int i = 0; i < gridSize; ++i) {
            for (int j = 0; j < gridSize; ++j) {
                grid[i][j] = ' ';
            }
        }

        int placedWords = 0;
        int maxAttempts = 100 * targetWordCount;
        std::string placedWordsList[MAX_WORDS];
        int placedWordsCount = 0;

        // Place words randomly in any direction
        while (placedWords < targetWordCount && maxAttempts > 0) {
            int wordIndex = rand() % totalWords;
            const std::string& word = words[wordIndex];
            if (word.length() != targetWordLength) {
                continue; // Skip words not matching target length
            }

            // Check if word is already placed
            bool alreadyPlaced = false;
            for (int i = 0; i < placedWordsCount; ++i) {
                if (placedWordsList[i] == word) {
                    alreadyPlaced = true;
                    break;
                }
            }
            if (alreadyPlaced) {
                continue;
            }

            int row = rand() % gridSize;
            int col = rand() % gridSize;
            int direction = rand() % NUM_DIRECTIONS;

            // Check if word can be placed
            bool canPlace = true;
            for (int k = 0; k < targetWordLength; ++k) {
                int newRow = row, newCol = col;
                switch (direction) {
                case 0: newCol += k; break; // Right
                case 1: newCol -= k; break; // Left
                case 2: newRow += k; break; // Down
                case 3: newRow -= k; break; // Up
                case 4: newRow += k; newCol += k; break; // Down-right
                case 5: newRow -= k; newCol += k; break; // Up-right
                case 6: newRow += k; newCol -= k; break; // Down-left
                case 7: newRow -= k; newCol -= k; break; // Up-left
                }

                if (newRow < 0 || newRow >= gridSize || newCol < 0 || newCol >= gridSize) {
                    canPlace = false;
                    break;
                }
                if (grid[newRow][newCol] != ' ' && grid[newRow][newCol] != word[k]) {
                    canPlace = false;
                    break;
                }
            }

            // Place the word
            if (canPlace) {
                for (int k = 0; k < targetWordLength; ++k) {
                    int newRow = row, newCol = col;
                    switch (direction) {
                    case 0: newCol += k; break;
                    case 1: newCol -= k; break;
                    case 2: newRow += k; break;
                    case 3: newRow -= k; break;
                    case 4: newRow += k; newCol += k; break;
                    case 5: newRow -= k; newCol += k; break;
                    case 6: newRow += k; newCol -= k; break;
                    case 7: newRow -= k; newCol -= k; break;
                    }
                    grid[newRow][newCol] = word[k];
                }
                placedWordsList[placedWordsCount++] = word;
                ++placedWords;
            }
            --maxAttempts;
        }

        // Fill remaining spaces with random letters
        for (int i = 0; i < gridSize; ++i) {
            for (int j = 0; j < gridSize; ++j) {
                if (grid[i][j] == ' ') {
                    grid[i][j] = 'a' + (rand() % 26); // Random lowercase letter
                }
            }
        }
    }

    //------------------------------------------------------------------------------
    // printGrid
    // Prints the current state of the grid to the console with borders.
    //------------------------------------------------------------------------------
    void GameMode::printGrid() {
        std::cout << "   ";
        for (int col = 0; col < gridSize; ++col) {
            std::cout << "  " << col << "  ";
        }
        std::cout << std::endl;

        for (int i = 0; i < gridSize; ++i) {
            std::cout << " ";
            for (int k = 0; k < gridSize; ++k) {
                std::cout << " - - ";
            }
            std::cout << std::endl;

            std::cout << i << " ";
            for (int j = 0; j < gridSize; ++j) {
                std::cout << " | " << grid[i][j] << " ";
            }
            std::cout << "|" << std::endl;
        }

        std::cout << "  ";
        for (int k = 0; k < gridSize; ++k) {
            std::cout << " - - ";
        }
        std::cout << std::endl;
    }

    //------------------------------------------------------------------------------
    // clearGrid
    // Frees dynamically allocated grid memory.
    //------------------------------------------------------------------------------
    void GameMode::clearGrid() {
        if (grid != nullptr) {
            for (int i = 0; i < gridSize; ++i) {
                delete[] grid[i];
            }
            delete[] grid;
            grid = nullptr;
            gridSize = 0;
        }
    }

    //------------------------------------------------------------------------------
    // Grid implementation for EasyMode
    // Allocates and populates the grid based on the selected level.
    //------------------------------------------------------------------------------
    void EasyMode::Grid(const std::string& level, const std::string& filename) {
        std::string easyWords[MAX_WORDS];
        int totalEasyWords = 0;
        WordValidator validator;
        try {
            validator.loadWords(filename, easyWords, totalEasyWords);
        }
        catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << std::endl;
            return;
        }

        int levelNum = std::stoi(level);
        allocateGrid(GRID_SIZES[levelNum - 1]);
        populateGrid(easyWords, totalEasyWords, getWordCount(levelNum), getWordLength(levelNum));
    }

    

    //------------------------------------------------------------------------------
    // Grid implementation for HardMode
    // Allocates and populates the grid based on the selected level.
    //------------------------------------------------------------------------------
    void HardMode::Grid(const std::string& level, const std::string& filename) {
        std::string hardWords[MAX_WORDS];
        int totalHardWords = 0;
        WordValidator validator;
        try {
            validator.loadWords(filename, hardWords, totalHardWords);
        }
        catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << std::endl;
            return;
        }

        int levelNum = std::stoi(level);
        allocateGrid(GRID_SIZES[levelNum - 1]);
        populateGrid(hardWords, totalHardWords, getWordCount(levelNum), getWordLength(levelNum));
    }

    //------------------------------------------------------------------------------
    // getWordLength and getWordCount for EasyMode
    //------------------------------------------------------------------------------
    int EasyMode::getWordLength(int level) const {
        switch (level) {
        case 1: return 2;
        case 2: return 3;
        case 3: return 4;
        default: return 3; // Fallback
        }
    }

    int EasyMode::getWordCount(int level) const {
        switch (level) {
        case 1: return 3;
        case 2: return 5;
        case 3: return 7;
        default: return 5; // Fallback
        }
    }

    //------------------------------------------------------------------------------
    // getWordLength and getWordCount for HardMode
    //------------------------------------------------------------------------------
    int HardMode::getWordLength(int level) const {
        switch (level) {
        case 1: return 5;
        case 2: return 6;
        case 3: return 7;
        default: return 5; // Fallback
        }
    }

    int HardMode::getWordCount(int level) const {
        switch (level) {
        case 1: return 3;
        case 2: return 5;
        case 3: return 7;
        default: return 5; // Fallback
        }
    }

    //------------------------------------------------------------------------------
    // isValidWord
    // Validates if the provided word exists in the dictionary.
    //------------------------------------------------------------------------------
    bool WordValidator::isValidWord(const std::string& word, const std::string dictionary[], int dictionarySize) {
        for (int i = 0; i < dictionarySize; ++i) {
            if (dictionary[i] == word) {
                return true;
            }
        }
        return false;
    }

    //------------------------------------------------------------------------------
    // loadWords
    // Loads words from a file into an array.
    //------------------------------------------------------------------------------
    void WordValidator::loadWords(const std::string& filename, std::string words[], int& wordSize) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file " + filename);
        }
        std::string word;
        wordSize = 0;
        while (file >> word && wordSize < MAX_WORDS) {
            words[wordSize++] = word;
        }
        file.close();
    }

    //------------------------------------------------------------------------------
    // checkWordInGrid
    // Checks if a word exists in the grid in any direction, removing it if found.
    //------------------------------------------------------------------------------
    bool WordValidator::checkWordInGrid(std::string word, char** grid, int gridSize) {
        int wordLen = static_cast<int>(word.length());
        for (int i = 0; i < gridSize; ++i) {
            for (int j = 0; j < gridSize; ++j) {
                for (int d = 0; d < NUM_DIRECTIONS; ++d) {
                    if ((d == 0 && j + wordLen > gridSize) || // Right
                        (d == 1 && j - wordLen + 1 < 0) || // Left
                        (d == 2 && i + wordLen > gridSize) || // Down
                        (d == 3 && i - wordLen + 1 < 0) || // Up
                        (d == 4 && (i + wordLen > gridSize || j + wordLen > gridSize)) || // Down-right
                        (d == 5 && (i - wordLen + 1 < 0 || j + wordLen > gridSize)) || // Up-right
                        (d == 6 && (i + wordLen > gridSize || j - wordLen + 1 < 0)) || // Down-left
                        (d == 7 && (i - wordLen + 1 < 0 || j - wordLen + 1 < 0))) { // Up-left
                        continue;
                    }

                    bool match = true;
                    int i2 = i, j2 = j;
                    for (int k = 0; k < wordLen; ++k) {
                        if (std::tolower(word[k]) != std::tolower(grid[i2][j2])) {
                            match = false;
                            break;
                        }
                        switch (d) {
                        case 0: j2++; break; // Right
                        case 1: j2--; break; // Left
                        case 2: i2++; break; // Down
                        case 3: i2--; break; // Up
                        case 4: i2++; j2++; break; // Down-right
                        case 5: i2--; j2++; break; // Up-right
                        case 6: i2++; j2--; break; // Down-left
                        case 7: i2--; j2--; break; // Up-left
                        }
                    }

                    if (match) {
                        i2 = i; j2 = j;
                        for (int k = 0; k < wordLen; ++k) {
                            grid[i2][j2] = '*'; // Mark matched word
                            switch (d) {
                            case 0: j2++; break;
                            case 1: j2--; break;
                            case 2: i2++; break;
                            case 3: i2--; break;
                            case 4: i2++; j2++; break;
                            case 5: i2--; j2++; break;
                            case 6: i2++; j2--; break;
                            case 7: i2--; j2--; break;
                            }
                        }
                        std::cout << "++++++++++++ Matched ++++++++++++++\n";
                        return true;
                    }
                }
            }
        }
        return false;
    }

    //------------------------------------------------------------------------------
    // displayInstructions
    // Displays game rules and instructions.
    //------------------------------------------------------------------------------
    void Game::displayInstructions() {
        std::cout << "\033[2J\033[1;1H"; // Clear screen (portable)
        std::cout << "========================================================================\n";
        std::cout << "||                           INSTRUCTIONS                             ||\n";
        std::cout << "========================================================================\n";
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "| You have two modes: 'Easy' and 'Hard'. Each mode has 3 levels.       |\n";
        std::cout << "| In level 1, the grid is 10x10, and you must guess 3 words.           |\n";
        std::cout << "| In level 2, the grid is 15x15, and you must guess 5 words.           |\n";
        std::cout << "| In level 3, the grid is 20x20, and you must guess 7 words.           |\n";
        std::cout << "| In Easy Mode:                                                        |\n";
        std::cout << "|   - Level 1: Max word length is 2 characters.                        |\n";
        std::cout << "|   - Level 2: Max word length is 3 characters.                        |\n";
        std::cout << "|   - Level 3: Max word length is 4 characters.                        |\n";
        std::cout << "| In Hard Mode:                                                        |\n";
        std::cout << "|   - Level 1: Max word length is 5 characters.                        |\n";
        std::cout << "|   - Level 2: Max word length is 6 characters.                        |\n";
        std::cout << "|   - Level 3: Max word length is 7 characters.                        |\n";
        std::cout << "| You have " << MAX_CHANCES << " chances to guess the correct word.         |\n";
        std::cout << "========================================================================\n";
        std::cout << "Press Enter to return to menu...";
        std::cin.get();
    }

    //------------------------------------------------------------------------------
    // AboutUs
    // Displays developer information.
    //------------------------------------------------------------------------------
    void Game::AboutUs() {
        std::cout << "\033[2J\033[1;1H";
        std::cout << "========================================================================\n";
        std::cout << "||                            ABOUT US                                ||\n";
        std::cout << "========================================================================\n";
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "|| -> This Game is made by Muhammad Abdullah and Hafiz Faheem.        ||\n";
        std::cout << "|| -> We are students at Fast NUCES CFD campus, studying CS.          ||\n";
        std::cout << "========================================================================\n";
        std::cout << "Press Enter to return to menu...";
        std::cin.get();
    }

    //------------------------------------------------------------------------------
    // displayMenu
    // Displays the main menu and handles user input.
    //------------------------------------------------------------------------------
    void Game::displayMenu() {
        std::string choice;
        do {
            std::cout << "\033[2J\033[1;1H";
            std::cout << "========================================================================\n";
            std::cout << "||                         Word Guessing Game                         ||\n";
            std::cout << "========================================================================\n";
            std::cout << "||                             MAIN MENU                              ||\n";
            std::cout << "========================================================================\n";
            std::cout << "|| -> Press (a) to Start Game                                         ||\n";
            std::cout << "|| -> Press (b) to Display Rules and Instructions                     ||\n";
            std::cout << "|| -> Press (c) for About Us                                          ||\n";
            std::cout << "|| -> Press (d) to Display Highest Scores                             ||\n";
            std::cout << "|| -> Press (e) to Exit                                               ||\n";
            std::cout << "========================================================================\n";
            std::cout << "Enter your Choice: ";
            std::getline(std::cin, choice);

            if (choice.empty() || choice.length() > 1 || std::tolower(choice[0]) < 'a' || std::tolower(choice[0]) > 'e') {
                std::cout << "Invalid input! Please enter a single character (a-e).\n";
                std::this_thread::sleep_for(std::chrono::seconds(1));
                continue;
            }

            choice = std::tolower(choice[0]);
            if (choice == "a") {
                startGame();
            }
            else if (choice == "b") {
                displayInstructions();
            }
            else if (choice == "c") {
                AboutUs();
            }
            else if (choice == "d") {
                HighestScore();
            }
            else if (choice == "e") {
                break;
            }
        } while (true);
    }

    //------------------------------------------------------------------------------
    // HighestScore
    // Displays the top five highest scores.
    //------------------------------------------------------------------------------
    void Game::HighestScore() {
        std::cout << "\033[2J\033[1;1H";
        std::cout << "========================================================================\n";
        std::cout << "||                           HIGHEST SCORE                            ||\n";
        std::cout << "========================================================================\n";

        ScoreTracker scoreTracker;
        scoreTracker.loadScores("scores.txt");

        for (int i = 0; i < 5; ++i) {
            std::cout << "Score " << (i + 1) << ": " << scoreTracker.getTopScore(i) << std::endl;
        }
        std::cout << "Press Enter to return to menu...";
        std::cin.get();
    }

    //------------------------------------------------------------------------------
    // startGame
    // Manages game mode and level selection, grid setup, and guessing logic.
    //------------------------------------------------------------------------------
    void Game::startGame() {
        int modeSel;
        std::cout << "\033[2J\033[1;1H";
        std::cout << "-------------------------------\n";
        std::cout << "|       Select Game Mode:     |\n";
        std::cout << "-------------------------------\n";

        while (true) {
            std::cout << "| -> Press (1) for Easy Mode  |\n";
            std::cout << "| -> Press (2) for Hard Mode  |\n";
            std::cout << "-------------------------------\n";
            std::cout << "Enter your choice: ";
            std::string input;
            std::getline(std::cin, input);

            if (input == "1" || input == "2") {
                modeSel = std::stoi(input);
                break;
            }
            std::cout << "Invalid choice! Please enter 1 or 2.\n";
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        GameMode* mode = nullptr;
        try {
            mode = (modeSel == 1) ? static_cast<GameMode*>(new EasyMode()) : static_cast<GameMode*>(new HardMode());
        }
        catch (const std::bad_alloc& e) {
            std::cout << "Memory allocation failed: " << e.what() << std::endl;
            return;
        }

        int levelSel;
        while (true) {
            std::cout << "-------------------------------\n";
            std::cout << "|       Select Level:         |\n";
            std::cout << "-------------------------------\n";
            std::cout << "| -> Press (1) for Level 1    |\n";
            std::cout << "| -> Press (2) for Level 2    |\n";
            std::cout << "| -> Press (3) for Level 3    |\n";
            std::cout << "-------------------------------\n";
            std::cout << "Enter your choice: ";
            std::string input;
            std::getline(std::cin, input);

            if (input == "1" || input == "2" || input == "3") {
                levelSel = std::stoi(input);
                break;
            }
            std::cout << "Invalid choice! Please enter 1, 2, or 3.\n";
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        int totalChances = MAX_CHANCES;
        const int maxGuesses = 100;
        std::string guessedWords[maxGuesses];
        int guessedCount = 0;

        std::string dictionary[MAX_WORDS];
        int dictionarySize = 0;
        WordValidator validator;
        try {
            std::string filename = (modeSel == 1) ? "easy_words.txt" : "hard_words.txt";
            validator.loadWords(filename, dictionary, dictionarySize);
        }
        catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << std::endl;
            delete mode;
            return;
        }

        ScoreTracker scoreTracker;

        for (int level = levelSel; level <= 3; ++level) {
            std::string filename = (modeSel == 1) ? "easy_words.txt" : "hard_words.txt";
            mode->Grid(std::to_string(level), filename);
            if (!mode->getGrid()) {
                std::cout << "Failed to initialize grid for level " << level << std::endl;
                break;
            }

            std::cout << "\033[2J\033[1;1H";
            std::cout << "============================\n";
            std::cout << "|          GRID            |\n";
            std::cout << "============================\n";
            mode->printGrid();

            std::string guess;
            int wordCount = mode->getWordCount(level);
            int successfulGuesses = 0;

            std::cout << "You need to guess " << wordCount << " words to complete level " << level << ".\n";
            std::cout << "Chances remaining: " << totalChances << "\n\n";

            while (totalChances > 0 && successfulGuesses < wordCount) {
                std::cout << "Enter a word to guess: ";
                std::getline(std::cin, guess);

                int wordLength = mode->getWordLength(level);
                if (guess.length() != wordLength) {
                    std::cout << "Word must be exactly " << wordLength << " characters long.\n";
                    --totalChances;
                    std::cout << "Chances remaining: " << totalChances << "\n\n";
                    continue;
                }

                bool alreadyGuessed = false;
                for (int i = 0; i < guessedCount; ++i) {
                    if (guessedWords[i] == guess) {
                        alreadyGuessed = true;
                        break;
                    }
                }
                if (alreadyGuessed) {
                    std::cout << "You already guessed this word.\n";
                    --totalChances;
                    std::cout << "Chances remaining: " << totalChances << "\n\n";
                    continue;
                }

                if (validator.checkWordInGrid(guess, mode->getGrid(), mode->getGridSize())) {
                    if (!validator.isValidWord(guess, dictionary, dictionarySize)) {
                        std::cout << "Word is in grid but not in dictionary.\n";
                        --totalChances;
                        std::cout << "Chances remaining: " << totalChances << "\n\n";
                        continue;
                    }

                    std::cout << "Congratulations! You found: " << guess << "\n";
                    guessedWords[guessedCount++] = guess;
                    ++successfulGuesses;
                    scoreTracker.updateScore(true);
                    mode->printGrid();
                }
                else {
                    std::cout << "Word not found in grid.\n";
                    --totalChances;
                    scoreTracker.updateScore(false);
                }

                std::cout << "Guessed words: " << guessedCount << " / " << wordCount << "\n";
                std::cout << "Chances remaining: " << totalChances << "\n\n";
            }

            if (successfulGuesses < wordCount) {
                std::cout << "Game Over! Final score: " << scoreTracker.getScore() << "\n";
                std::cout << "Words found: ";
                for (int i = 0; i < guessedCount; ++i) {
                    std::cout << guessedWords[i] << " ";
                }
                std::cout << "\n";
                std::cout << "Retry level? (y/n): ";
                std::string retry;
                std::getline(std::cin, retry);
                if (retry == "y" || retry == "Y") {
                    --level; // Replay current level
                    totalChances = MAX_CHANCES;
                    guessedCount = 0;
                    scoreTracker = ScoreTracker(); // Reset score
                    continue;
                }
                break;
            }

            std::cout << "Congratulations! You've completed level " << level << "!\n";
            if (level < 3) {
                std::cout << "Continue to next level? (y/n): ";
                std::string choice;
                std::getline(std::cin, choice);
                if (choice != "y" && choice != "Y") {
                    break;
                }
            }
        }

        scoreTracker.compareAndUpdateScores("scores.txt");
        mode->clearGrid();
        delete mode;
    }

    //------------------------------------------------------------------------------
    // ScoreTracker methods
    //------------------------------------------------------------------------------
    void ScoreTracker::updateScore(bool correctGuess) {
        if (correctGuess) {
            score += 10;
        }
    }

    void ScoreTracker::loadScores(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::ofstream initFile(filename);
            for (int i = 0; i < 5; ++i) {
                initFile << "0\n";
                topScores[i] = 0;
            }
            initFile.close();
            return;
        }
        for (int i = 0; i < 5; ++i) {
            if (!(file >> topScores[i])) {
                topScores[i] = 0;
            }
        }
        file.close();
    }

    void ScoreTracker::saveScores(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file " + filename + " for writing");
        }
        for (int i = 0; i < 5; ++i) {
            file << topScores[i] << "\n";
        }
        file.close();
    }

    int ScoreTracker::getScore() const {
        return score;
    }

    int ScoreTracker::getTopScore(int index) const {
        if (index >= 0 && index < 5) {
            return topScores[index];
        }
        return 0;
    }

    void ScoreTracker::compareAndUpdateScores(const std::string& filename) {
        loadScores(filename);
        for (int i = 0; i < 5; ++i) {
            if (score > topScores[i]) {
                for (int j = 4; j > i; --j) {
                    topScores[j] = topScores[j - 1];
                }
                topScores[i] = score;
                break;
            }
        }
        saveScores(filename);
    }

} // namespace WordGame