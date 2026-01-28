#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <random>
#include <exception>

const int BOARD_SIZE = 11;  // Size of the hex board

enum class Player {
    EMPTY = 0,
    PLAYER_X,
    PLAYER_O
};

class HexGame {
private:
    std::vector<std::vector<Player>> board;
    Player currentPlayer;

public:
    HexGame() : board(BOARD_SIZE, std::vector<Player>(BOARD_SIZE, Player::EMPTY)), currentPlayer(Player::PLAYER_X) {}

    void printBoard() const {
        for (int i = 0; i < BOARD_SIZE; ++i) {
            std::cout << std::string(i, ' '); // Adjust spacing for hexagon shape
            for (int j = 0; j < BOARD_SIZE; ++j) {
                char symbol;
                if (board[i][j] == Player::PLAYER_X)
                    symbol = 'X';
                else if (board[i][j] == Player::PLAYER_O)
                    symbol = 'O';
                else
                    symbol = '.';
                std::cout << symbol << " ";
            }
            std::cout << "\n";
        }
    }

    bool dfs(int row, int col, std::vector<std::vector<bool>>& visited, Player player) const {
        if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE)
            return false;

        if (visited[row][col] || board[row][col] != player)
            return false;

        visited[row][col] = true;

        if ((player == Player::PLAYER_X && col == BOARD_SIZE - 1) ||
            (player == Player::PLAYER_O && row == BOARD_SIZE - 1))
            return true;

        // Check all adjacent cells
        bool found = false;
        found |= dfs(row - 1, col, visited, player); // Up
        found |= dfs(row + 1, col, visited, player); // Down
        found |= dfs(row, col - 1, visited, player); // Left
        found |= dfs(row, col + 1, visited, player); // Right

        return found;
    }

    bool checkWin(Player player) const {
        std::vector<std::vector<bool>> visited(BOARD_SIZE, std::vector<bool>(BOARD_SIZE, false));

        if (player == Player::PLAYER_X) {
            for (int i = 0; i < BOARD_SIZE; ++i) {
                if (board[i][0] == Player::PLAYER_X) {
                    if (dfs(i, 0, visited, Player::PLAYER_X))
                        return true;
                }
            }
        } else if (player == Player::PLAYER_O) {
            for (int i = 0; i < BOARD_SIZE; ++i) {
                if (board[0][i] == Player::PLAYER_O) {
                    if (dfs(0, i, visited, Player::PLAYER_O))
                        return true;
                }
            }
        }

        return false;
    }

    std::pair<int, int> getRandomValidMove() const {
        std::vector<std::pair<int, int>> validMoves;

        for (int i = 0; i < BOARD_SIZE; ++i) {
            for (int j = 0; j < BOARD_SIZE; ++j) {
                if (board[i][j] == Player::EMPTY)
                    validMoves.push_back({i, j});
            }
        }

        if (validMoves.empty()) {
            std::cerr << "No valid moves available.\n";
            return {-1, -1};  // Return an invalid pair
        }

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, validMoves.size() - 1);
        int index = dis(gen);

        return validMoves[index];
    }

    bool isValidMove(int row, int col) const {
        if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE) {
            if (currentPlayer == Player::PLAYER_X) {
                std::cerr << "Invalid move: Out of bounds.\n";
                std::cerr << "Invalid move. Try again.\n";  // Additional message for Player X
            }
            return false;
        }

        int leftmostCol = row;
        int rightmostCol = BOARD_SIZE + row - 1;

        if (col < leftmostCol || col >= rightmostCol) {  // Adjusted bounds check
            if (currentPlayer == Player::PLAYER_X) {
                std::cerr << "Invalid move: Out of bounds.\n";
                std::cerr << "Invalid move. Try again.\n";  // Additional message for Player X
            }
            return false;
        }

        if (board[row][col] != Player::EMPTY) {
            if (currentPlayer == Player::PLAYER_X) {
                std::cerr << "Invalid move: Cell already occupied.\n";
                std::cerr << "Invalid move. Try again.\n";  // Additional message for Player X
            }
            return false;
        }

        return true;
    }

    void makeMove(int row, int col) {
        if (isValidMove(row, col)) {
            board[row][col] = currentPlayer;
            currentPlayer = (currentPlayer == Player::PLAYER_X) ? Player::PLAYER_O : Player::PLAYER_X;
        }
    }

    bool isGameOver() const {
        return checkWin(Player::PLAYER_X) || checkWin(Player::PLAYER_O);
    }

    Player getCurrentPlayer() const {
        return currentPlayer;
    }
};

class MonteCarloAI {
private:
    HexGame& hexGame;
    int simulations;

public:
    MonteCarloAI(HexGame& game, int numSimulations) : hexGame(game), simulations(numSimulations) {}

    std::pair<int, int> getBestMove() {
        Player currentPlayer = hexGame.getCurrentPlayer();
        double bestScore = -1;
        std::pair<int, int> bestMove;

        for (int i = 0; i < simulations; ++i) {
            HexGame tempGame = hexGame;  // Create a copy of the current game state

            // Simulate a random game from the current state
            std::pair<int, int> randomMove = tempGame.getRandomValidMove();
            if (randomMove.first == -1 || randomMove.second == -1)
                continue;  // Skip this simulation if the move is invalid

            tempGame.makeMove(randomMove.first, randomMove.second);

            // Calculate the score based on the outcome of the simulated game
            double score = (tempGame.checkWin(currentPlayer)) ? 1.0 : 0.0;

            if (score > bestScore) {
                bestScore = score;
                bestMove = randomMove;  // Best move so far
            }
        }

        return bestMove;
    }
};

int main() {
    HexGame hexGame;
    MonteCarloAI mcAI(hexGame, 1000);  // Initialize MonteCarloAI

    bool aiMadeMove = false;  // Flag to track if AI made a move

    try {
        while (!hexGame.isGameOver()) {
            if (!aiMadeMove) {
                hexGame.printBoard();  // Print the board only if AI has not made a move yet
            }

            if (hexGame.getCurrentPlayer() == Player::PLAYER_X) {
                int row, col;
                std::cout << "Player X, enter your move (row and column): ";

                while (!(std::cin >> row >> col) || !hexGame.isValidMove(row, col)) {
                    std::cout << "Invalid move. Try again.\n";  // Additional message for Player X
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                }

                hexGame.makeMove(row, col);
                aiMadeMove = false;  // Player X made a move, AI hasn't
            } else {
                std::pair<int, int> aiMove = mcAI.getBestMove();

                if (aiMove.first == -1 || aiMove.second == -1) {
                    std::cerr << "AI made an invalid move. Game ends in a draw.\n";
                    break;
                }

                hexGame.makeMove(aiMove.first, aiMove.second);
                aiMadeMove = true;  // AI made a move
            }
        }

        hexGame.printBoard();
        if (hexGame.checkWin(Player::PLAYER_X))
            std::cout << "Game over. Player X wins!\n";
        else
            std::cout << "Game over. Player O wins!\n";

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "An exception occurred: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "An unknown exception occurred.\n";
        return 1;
    }

    return 0;
}
