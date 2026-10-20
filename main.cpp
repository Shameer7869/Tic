#include <array>
#include <cstdlib>
#include <iostream>
#include <string>

using Board = std::array<char, 9>;

// Set all cells to empty.
void initializeBoard(Board& board) {
    board.fill(' ');
}

// Display the board. Empty cells are shown as their position number.
void displayBoard(const Board& board) {
    std::cout << '\n';

    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 3; ++col) {
            int index = row * 3 + col;
            char cell = (board[index] == ' ')
                            ? static_cast<char>('1' + index)
                            : board[index];

            std::cout << ' ' << cell;
            if (col < 2) {
                std::cout << " |";
            }
        }
        std::cout << '\n';

        if (row < 2) {
            std::cout << "---+---+---\n";
        }
    }

    std::cout << '\n';
}

// A move is valid only if the position is 0..8 and the cell is empty.
bool isValidMove(const Board& board, int position) {
    return position >= 0 && position < 9 && board[position] == ' ';
}

// Return 'X', 'O', or ' ' if there is no winner.
char checkWinner(const Board& board) {
    static const int winningLines[8][3] = {
        {0, 1, 2}, // rows
        {3, 4, 5},
        {6, 7, 8},
        {0, 3, 6}, // columns
        {1, 4, 7},
        {2, 5, 8},
        {0, 4, 8}, // diagonals
        {2, 4, 6}
    };

    for (const auto& line : winningLines) {
        char first = board[line[0]];
        if (first != ' ' &&
            first == board[line[1]] &&
            first == board[line[2]]) {
            return first;
        }
    }

    return ' ';
}

bool isBoardFull(const Board& board) {
    for (char cell : board) {
        if (cell == ' ') {
            return false;
        }
    }
    return true;
}

// Minimax from the computer's perspective.
// Computer 'O' maximizes; human 'X' minimizes.
int minimax(Board& board, bool isMaximizing, int depth) {
    char winner = checkWinner(board);

    // Prefer faster computer wins and delay unavoidable human wins.
    if (winner == 'O') {
        return 10 - depth;
    }
    if (winner == 'X') {
        return depth - 10;
    }
    if (isBoardFull(board)) {
        return 0;
    }

    if (isMaximizing) {
        int bestScore = -1000000;

        for (int i = 0; i < 9; ++i) {
            if (board[i] == ' ') {
                board[i] = 'O';
                int score = minimax(board, false, depth + 1);
                board[i] = ' '; // restore simulated board

                if (score > bestScore) {
                    bestScore = score;
                }
            }
        }

        return bestScore;
    } else {
        int bestScore = 1000000;

        for (int i = 0; i < 9; ++i) {
            if (board[i] == ' ') {
                board[i] = 'X';
                int score = minimax(board, true, depth + 1);
                board[i] = ' '; // restore simulated board

                if (score < bestScore) {
                    bestScore = score;
                }
            }
        }

        return bestScore;
    }
}

// Try every legal computer move and return the best one.
int getBestComputerMove(Board& board) {
    int bestScore = -1000000;
    int bestMove = -1;

    for (int i = 0; i < 9; ++i) {
        if (board[i] == ' ') {
            board[i] = 'O';
            int score = minimax(board, false, 1);
            board[i] = ' '; // undo move

            if (score > bestScore) {
                bestScore = score;
                bestMove = i;
            }
        }
    }

    // This should never happen in a valid non-terminal game.
    if (bestMove == -1) {
        for (int i = 0; i < 9; ++i) {
            if (board[i] == ' ') {
                return i;
            }
        }
    }

    return bestMove;
}

// Read and validate one human move. Returns a zero-based index 0..8.
int getHumanMove(const Board& board) {
    while (true) {
        std::cout << "Enter your move (1-9): " << std::flush;

        std::string line;
        if (!std::getline(std::cin, line)) {
            std::cout << "\nEnd of input detected. Exiting.\n";
            std::exit(0);
        }

        // Accept exactly one digit from '1' to '9'.
        if (line.size() != 1 || line[0] < '1' || line[0] > '9') {
            std::cout << "Invalid input! Please enter a number between 1 and 9.\n";
            continue;
        }

        int position = line[0] - '1';

        if (!isValidMove(board, position)) {
            std::cout << "That cell is already occupied! Please choose another empty cell.\n";
            continue;
        }

        return position;
    }
}

// Play one complete game.
void playGame() {
    Board board;
    initializeBoard(board);
    displayBoard(board);

    while (true) {
        // Human turn: X always moves first.
        int humanMove = getHumanMove(board);
        board[humanMove] = 'X';
        displayBoard(board);

        char winner = checkWinner(board);
        if (winner == 'X') {
            std::cout << "Congratulations! You won!\n";
            return;
        }
        if (isBoardFull(board)) {
            std::cout << "It's a tie!\n";
            return;
        }

        // Computer turn: O uses minimax.
        int computerMove = getBestComputerMove(board);
        std::cout << "Computer chose cell " << (computerMove + 1) << ".\n";
        board[computerMove] = 'O';
        displayBoard(board);

        winner = checkWinner(board);
        if (winner == 'O') {
            std::cout << "The computer wins!\n";
            return;
        }
        if (isBoardFull(board)) {
            std::cout << "It's a tie!\n";
            return;
        }
    }
}

// Validate replay input. Only a single y/Y/n/N is accepted.
bool askToPlayAgain() {
    while (true) {
        std::cout << "Do you want to play again? (y/n): " << std::flush;

        std::string line;
        if (!std::getline(std::cin, line)) {
            std::cout << "\nEnd of input detected. Exiting.\n";
            std::exit(0);
        }

        if (line.size() == 1) {
            char answer = line[0];

            if (answer == 'y' || answer == 'Y') {
                return true;
            }
            if (answer == 'n' || answer == 'N') {
                return false;
            }
        }

        std::cout << "Invalid input! Please enter 'y' or 'n'.\n";
    }
}

int main() {
    std::cout << "Welcome to Tic Tac Toe!\n\n";
    std::cout << "You are Player X.\n";
    std::cout << "The computer is Player O.\n\n";
    std::cout << "Choose a cell by entering its number from 1 to 9.\n";
    std::cout << "Enter a number corresponding to an empty cell.\n";

    bool playAgain = true;

    while (playAgain) {
        playGame();
        playAgain = askToPlayAgain();
    }

    std::cout << "Thank you for playing Tic Tac Toe! Goodbye!\n";
    return 0;
}