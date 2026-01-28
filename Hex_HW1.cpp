#include <iostream>
#include <vector>

using namespace std;

const char EMPTY_CELL = '.';
const char PLAYER_1_SYMBOL = 'X';
const char PLAYER_2_SYMBOL = 'O';

// Structure to represent a cell on the board
struct Cell {
    char symbol;
    bool visited;
};

// Function to initialize the HEX game board
vector<vector<Cell>> initializeBoard(int size) {
    vector<vector<Cell>> board(size, vector<Cell>(size, {EMPTY_CELL, false}));
    return board;
}

// Function to draw the HEX game board
void drawBoard(const vector<vector<Cell>>& board) {
    for (size_t i = 0; i < board.size(); ++i) {
        for (size_t j = 0; j < i; ++j)
            cout << ' ';
        for (size_t j = 0; j < board[i].size(); ++j)
            cout << board[i][j].symbol << ' ';
        cout << endl;
    }
}

// Function to check if a move is legal
bool isLegalMove(const vector<vector<Cell>>& board, int row, int col) {
    return row >= 0 && row < static_cast<int>(board.size()) &&
           col >= 0 && col < static_cast<int>(board[row].size()) &&
           board[row][col].symbol == EMPTY_CELL;
}

// Function to determine the winner (simple check if a path connects both sides)
bool determineWinner(const vector<vector<Cell>>& board) {
    // For simplicity, let's assume that player 1 (X) starts at row 0, and player 2 (O) starts at column 0.
    // Check if there is a path connecting both sides (top to bottom and left to right)
    for (size_t i = 0; i < board.size(); ++i) {
        if (board[i][0].symbol == PLAYER_1_SYMBOL && board[i][board.size() - 1].symbol == PLAYER_1_SYMBOL)
            return true;  // Player 1 wins
        if (board[0][i].symbol == PLAYER_2_SYMBOL && board[board.size() - 1][i].symbol == PLAYER_2_SYMBOL)
            return true;  // Player 2 wins
    }
    return false;  // No winner yet
}

int main() {
    int boardSize;
    cout << "Enter the size of the HEX game board: ";
    cin >> boardSize;

    vector<vector<Cell>> board = initializeBoard(boardSize);

    bool player1Turn = true; // Player 1 starts
    int moveRow, moveCol;

    while (true) {
        drawBoard(board);

        char currentPlayerSymbol = (player1Turn) ? PLAYER_1_SYMBOL : PLAYER_2_SYMBOL;
        cout << "Player " << ((player1Turn) ? "1" : "2") << "'s turn. Enter your move (row and column): ";
        cin >> moveRow >> moveCol;

        if (isLegalMove(board, moveRow, moveCol)) {
            board[moveRow][moveCol].symbol = currentPlayerSymbol;

            if (determineWinner(board)) {
                drawBoard(board);
                cout << "Player " << ((player1Turn) ? "1" : "2") << " wins!" << endl;
                break;
            }

            player1Turn = !player1Turn; // Switch turn to the other player
        } else {
            cout << "Invalid move. Please try again." << endl;
        }
    }

    return 0;
}
