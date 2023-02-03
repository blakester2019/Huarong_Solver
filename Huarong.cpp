#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <time.h>
#include <stack>
#include <queue>
#include <set>
#include <chrono>

using namespace std;

// Node Struct
struct node
{
    public:
        vector<vector<char>> board;
        node* parent;
        node* next;
        node(vector<vector<char>> board, node* parent = NULL, node* next = NULL)
        {
            this->board = board;
            this->parent = parent;
            this->next = next;
        }
};

// Blank struct
struct blank
{
    public:
        int rowCord = -1;
        int colCord = -1;
};

// Prototypes

string getPuzzle();
vector<vector<char>> buildBoard(string name);
void printBoard(vector<vector<char>> board);
string boardState(vector<vector<char>> board);
void findBlanks(vector<vector<char>> board);
int makeSingleMove(vector<vector<char>> board, vector<vector<char>> &temp, blank b, string position);
int makeDoubleMove(vector<vector<char>> board, vector<vector<char>>& temp, string position);
void bfs(vector<vector<char>> board);
void dfs(vector<vector<char>> board);
int checkGoal(vector<vector<char>> board);
string checkPiece(vector<vector<char>> board, char identifier);
void printsolution(node* n);
void createOutputFile(string name, string algorithm, node* solution);

// Global variables
string puzzleName = "none";
blank blank1;
blank blank2;
int blanksAdjacent = 0;
string adjacentOrientation = "none";

typedef node* nodeP;

// MAIN
int main()
{
    string name = getPuzzle();
    puzzleName = name;

    while (name != "Q" && name != "Quit" && name != "quit") {
        vector<vector<char>> board = buildBoard(name);

        int searchMethod = 0;
        while (searchMethod != 1 && searchMethod != 2) {
            cout << "   Select a search method:" << endl;
            cout << "   1. Depth-first Search" << endl;
            cout << "   2. Breadth-First Search" << endl;
            cin >> searchMethod;
        }

        auto start = chrono::steady_clock::now();
        if (searchMethod == 1)
            dfs(board);
        else if (searchMethod == 2)
            bfs(board);

        auto elapsed1 = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start);
        auto elapsed2 = chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - start);
        cout << "The elapsed run time is: " << elapsed1.count() << " milliseconds" << " or " << elapsed2.count() << " seconds\n\n";


        name = getPuzzle();
        puzzleName = name;
    }

    return 0;
} // end of main


/// <summary>
/// Gets input from user for name of the puzzle
/// </summary>
/// <returns>string: name of the puzzle</returns>
string getPuzzle()
{
    string name;
    cout << "Please enter name of the first puzzle." << endl;
    cout << "Enter Q or Quit to terminate the program." << endl;
    cin >> name;
    return name;
}

/// <summary>
/// Builds the initial board state from the puzzle name
/// </summary>
/// <param name="name">name of the puzzle</param>
/// <returns>vector<vector<char>>: 2D array of the initial game board</returns>
vector<vector<char>> buildBoard(string name)
{
    vector<vector<char>> vect;

    ifstream file;
    file.open("inputs/" + name + ".in");
    
    string line;
    if (file.is_open()) {
        while (getline(file, line)) {
            istringstream iss(line);
            vector<char> row;
            char elems[4];
            iss >> elems[0] >> elems[1] >> elems[2] >> elems[3];

            for (int i = 0; i < 4; i++) {
                row.push_back(elems[i]);
            }

            vect.push_back(row);
        }
    }
    else {
        cout << "There is no puzzle matching this name" << endl;
        exit(-1);
    }

    return vect;
}

/// <summary>
/// Prints the board in a 2D form
/// </summary>
/// <param name="board">Board to be printed</param>
void printBoard(vector<vector<char>> board)
{
    for (int i = 0; i < board.size(); i++) {
        for (int j = 0; j < board[i].size(); j++) {
            cout << board[i][j] << " ";
        }
        cout << endl;
    }
}

/// <summary>
/// Transforms a 2D board into a string
/// </summary>
/// <param name="board">The board to be transformed</param>
/// <returns>string: string representation of the board</returns>
string boardState(vector<vector<char>> board)
{
    string state = "";
    for (int i = 0; i < board.size(); i++) {
        for (int j = 0; j < board[i].size(); j++) {
            state.append(1, board[i][j]);
        }
    }
    return state;
}

/// <summary>
/// Checks to see if a puzzle has successfully been completed
/// </summary>
/// <param name="board">The board to check the goal completion state against</param>
/// <returns>int: 1 for success (goal completed), 0 for failure</returns>
int checkGoal(vector<vector<char>> board)
{
    if (board[3][1] == '0' && board[4][1] == '0' && board[3][2] == '0' && board[4][2] == '0')
        return 1;
    else
        return 0;
}

/// <summary>
/// Moves a single blank square either up, down, left, or right
/// </summary>
/// <param name="board">The state of the board to be used (passed by value)</param>
/// <param name="temp">2D board that is edited in the function (passed by reference)</param>
/// <param name="b">Global variable blank piece</param>
/// <param name="position">Up, down, left, or right</param>
/// <returns></returns>
int makeSingleMove(vector<vector<char>> board, vector<vector<char>> &temp, blank b, string position)
{
    // Set temporary board to the current board state
    temp = board;

    // Up
    if (position == "up") {
        if (b.rowCord == 0)
            return 0;
        
        char adjacentIdentifier = board[b.rowCord - 1][b.colCord];
        string pieceType = checkPiece(board, adjacentIdentifier);
        if (pieceType == "vertical") {
            temp[b.rowCord][b.colCord] = adjacentIdentifier;
            temp[b.rowCord - 2][b.colCord] = 'b';
            return 1;
        }
        else if (pieceType == "square") {
            temp[b.rowCord][b.colCord] = adjacentIdentifier;
            temp[b.rowCord - 1][b.colCord] = 'b';
            return 1;
        }
    }
    // Down
    else if (position == "down") {
        if (b.rowCord == 4)
            return 0;

        char adjacentIdentifier = board[b.rowCord + 1][b.colCord];
        string pieceType = checkPiece(board, adjacentIdentifier);
        if (pieceType == "vertical") {
            temp[b.rowCord][b.colCord] = adjacentIdentifier;
            temp[b.rowCord + 2][b.colCord] = 'b';
            return 1;
        }
        else if (pieceType == "square") {
            temp[b.rowCord][b.colCord] = adjacentIdentifier;
            temp[b.rowCord + 1][b.colCord] = 'b';
            return 1;
        }
    }
    // Left
    else if (position == "left") {
        if (b.colCord == 0)
            return 0;

        char adjacentIdentifier = board[b.rowCord][b.colCord - 1];
        string pieceType = checkPiece(board, adjacentIdentifier);
        if (pieceType == "horizontal") {
            temp[b.rowCord][b.colCord] = adjacentIdentifier;
            temp[b.rowCord][b.colCord - 2] = 'b';
            return 1;
        }
        else if (pieceType == "square") {
            temp[b.rowCord][b.colCord] = adjacentIdentifier;
            temp[b.rowCord][b.colCord - 1] = 'b';
            return 1;
        }
    }
    // Right
    else if (position == "right") {
        if (b.colCord == 3)
            return 0;

        char adjacentIdentifier = board[b.rowCord][b.colCord + 1];
        string pieceType = checkPiece(board, adjacentIdentifier);
        if (pieceType == "horizontal") {
            temp[b.rowCord][b.colCord] = adjacentIdentifier;
            temp[b.rowCord][b.colCord + 2] = 'b';
            return 1;
        }
        else if (pieceType == "square") {
            temp[b.rowCord][b.colCord] = adjacentIdentifier;
            temp[b.rowCord][b.colCord + 1] = 'b';
            return 1;
        }
    }
    return 0;
}

/// <summary>
/// Moves adjacent blank square either up, down, left, or right
/// </summary>
/// <param name="board">The state of the board to be used (passed by value)</param>
/// <param name="temp">2D board that is edited in the function (passed by reference)</param>
/// <param name="position">Up, down, left, or right</param>
/// <returns></returns>
int makeDoubleMove(vector<vector<char>> board, vector<vector<char>>& temp, string position)
{
    // Set temporary board to the current board state
    temp = board;

    // Up
    if (position == "up") {
        if (blank1.rowCord == 0 || blank2.rowCord == 0)
            return 0;

        if (adjacentOrientation == "horizontal") {
            char p1ID = board[blank1.rowCord - 1][blank1.colCord];
            char p2ID = board[blank2.rowCord - 1][blank2.colCord];

            if ((p1ID == p2ID && p1ID != '0') || (checkPiece(board, p1ID) == "square" && checkPiece(board, p2ID) == "square")) {
                temp[blank1.rowCord - 1][blank1.colCord] = 'b';
                temp[blank2.rowCord - 1][blank2.colCord] = 'b';
                temp[blank1.rowCord][blank1.colCord] = p1ID;
                temp[blank2.rowCord][blank2.colCord] = p2ID;
                return 1;
            }
            else if (checkPiece(board, p1ID) == "vertical" && checkPiece(board, p2ID) == "vertical") {
                temp[blank1.rowCord - 2][blank1.colCord] = 'b';
                temp[blank2.rowCord - 2][blank2.colCord] = 'b';
                temp[blank1.rowCord][blank1.colCord] = p1ID;
                temp[blank2.rowCord][blank2.colCord] = p2ID;
                return 1;
            }
            else if (checkPiece(board, p1ID) == "king" && checkPiece(board, p2ID) == "king") {
                temp[blank1.rowCord - 2][blank1.colCord] = 'b';
                temp[blank2.rowCord - 2][blank2.colCord] = 'b';
                temp[blank1.rowCord][blank1.colCord] = p1ID;
                temp[blank2.rowCord][blank2.colCord] = p2ID;
                return 1;
            }
            else
                return 0;
        }
        else if (adjacentOrientation == "vertical") {
            if (blank1.rowCord - 2 < 0 || blank2.rowCord - 2 < 0) {
                char p1ID = board[blank1.rowCord - 1][blank1.colCord];
                if (checkPiece(board, p1ID) == "square") {
                    temp[blank1.rowCord - 1][blank1.colCord] = 'b';
                    temp[blank2.rowCord][blank2.colCord] = p1ID;
                    return 1;
                }
                else
                    return 0;
            }

            char p1ID = board[blank1.rowCord - 1][blank1.colCord];
            char p2ID = board[blank1.rowCord - 2][blank1.colCord];

            if (p1ID == '0' || p2ID == '0')
                return 0;

            if (p1ID == p2ID) {
                temp[blank1.rowCord - 1][blank1.colCord] = 'b';
                temp[blank1.rowCord - 2][blank1.colCord] = 'b';
                temp[blank1.rowCord][blank1.colCord] = p1ID;
                temp[blank2.rowCord][blank2.colCord] = p2ID;
                return 1;
            }
            else if (checkPiece(board, p1ID) == "square")
            {
                temp[blank1.rowCord - 1][blank1.colCord] = 'b';
                temp[blank2.rowCord][blank2.colCord] = p1ID;
                return 1;
            }
            else
                return 0;
        }
    }
    // Down
    else if (position == "down") {
        if (blank1.rowCord == 4 || blank2.rowCord == 4)
            return 0;

        if (adjacentOrientation == "horizontal") {
            char p1ID = board[blank1.rowCord + 1][blank1.colCord];
            char p2ID = board[blank2.rowCord + 1][blank2.colCord];

            if ((p1ID == p2ID && p1ID != '0') || (checkPiece(board, p1ID) == "square" && checkPiece(board, p2ID) == "square")) {
                temp[blank1.rowCord + 1][blank1.colCord] = 'b';
                temp[blank2.rowCord + 1][blank2.colCord] = 'b';
                temp[blank1.rowCord][blank1.colCord] = p1ID;
                temp[blank2.rowCord][blank2.colCord] = p2ID;
                return 1;
            }
            else if (checkPiece(board, p1ID) == "vertical" && checkPiece(board, p2ID) == "vertical") {
                temp[blank1.rowCord + 2][blank1.colCord] = 'b';
                temp[blank2.rowCord + 2][blank2.colCord] = 'b';
                temp[blank1.rowCord][blank1.colCord] = p1ID;
                temp[blank2.rowCord][blank2.colCord] = p2ID;
                return 1;
            }
            else if (checkPiece(board, p1ID) == "king" && checkPiece(board, p2ID) == "king") {
                temp[blank1.rowCord + 2][blank1.colCord] = 'b';
                temp[blank2.rowCord + 2][blank2.colCord] = 'b';
                temp[blank1.rowCord][blank1.colCord] = p1ID;
                temp[blank2.rowCord][blank2.colCord] = p2ID;
                return 1;
            }
            else
                return 0;
        }
        else if (adjacentOrientation == "vertical") {
            if (blank1.rowCord + 2 > 4 || blank2.rowCord + 2 > 4) {
                char p1ID = board[blank2.rowCord + 1][blank2.colCord];
                if (checkPiece(board, p1ID) == "square") {
                    temp[blank2.rowCord + 1][blank2.colCord] = 'b';
                    temp[blank1.rowCord][blank1.colCord] = p1ID;
                    return 1;
                }
                else
                    return 0;
            }

            char p1ID = board[blank2.rowCord + 1][blank2.colCord];
            char p2ID = board[blank2.rowCord + 2][blank2.colCord];

            if (p1ID == '0' || p2ID == '0')
                return 0;

            if (p1ID == p2ID) {
                temp[blank2.rowCord + 1][blank2.colCord] = 'b';
                temp[blank2.rowCord + 2][blank2.colCord] = 'b';
                temp[blank1.rowCord][blank1.colCord] = p1ID;
                temp[blank2.rowCord][blank2.colCord] = p2ID;
                return 1;
            }
            else if (checkPiece(board, p1ID) == "square")
            {
                temp[blank2.rowCord + 1][blank2.colCord] = 'b';
                temp[blank1.rowCord][blank1.colCord] = p1ID;
                return 1;
            }
            else
                return 0;
        }
    }
    // Left
    if (position == "left") {
        if (blank1.colCord == 0 || blank2.colCord == 0)
            return 0;

        if (adjacentOrientation == "vertical") {
            char p1ID = board[blank1.rowCord][blank1.colCord - 1];
            char p2ID = board[blank2.rowCord][blank2.colCord - 1];

            if ((p1ID == p2ID && p1ID != '0') || (checkPiece(board, p1ID) == "square" && checkPiece(board, p2ID) == "square")) {
                temp[blank1.rowCord][blank1.colCord - 1] = 'b';
                temp[blank2.rowCord][blank2.colCord - 1] = 'b';
                temp[blank1.rowCord][blank1.colCord] = p1ID;
                temp[blank2.rowCord][blank2.colCord] = p2ID;
                return 1;
            }
            else if (checkPiece(board, p1ID) == "horizontal" && checkPiece(board, p2ID) == "horizontal") {
                temp[blank1.rowCord][blank1.colCord - 2] = 'b';
                temp[blank2.rowCord][blank2.colCord - 2] = 'b';
                temp[blank1.rowCord][blank1.colCord] = p1ID;
                temp[blank2.rowCord][blank2.colCord] = p2ID;
                return 1;
            }
            else if (checkPiece(board, p1ID) == "king" && checkPiece(board, p2ID) == "king") {
                temp[blank1.rowCord][blank1.colCord - 2] = 'b';
                temp[blank2.rowCord][blank2.colCord - 2] = 'b';
                temp[blank1.rowCord][blank1.colCord] = p1ID;
                temp[blank2.rowCord][blank2.colCord] = p2ID;
                return 1;
            }
            else
                return 0;
        }
        else if (adjacentOrientation == "horizontal") {
            if (blank1.colCord - 2 < 0 || blank2.colCord - 2 < 0) {
                char p1ID = board[blank1.rowCord][blank1.colCord - 1];
                if (checkPiece(board, p1ID) == "square") {
                    temp[blank1.rowCord][blank1.colCord - 1] = 'b';
                    temp[blank2.rowCord][blank2.colCord] = p1ID;
                    return 1;
                }
                else
                    return 0;
            }
            char p1ID = board[blank1.rowCord][blank1.colCord - 2];
            char p2ID = board[blank1.rowCord][blank1.colCord - 1];

            if (p1ID == '0' || p2ID == '0')
                return 0;

            if (p1ID == p2ID) {
                temp[blank1.rowCord][blank1.colCord - 2] = 'b';
                temp[blank1.rowCord][blank1.colCord - 1] = 'b';
                temp[blank1.rowCord][blank1.colCord] = p1ID;
                temp[blank2.rowCord][blank2.colCord] = p2ID;
                return 1;
            }
            else if (checkPiece(board, p2ID) == "square")
            {
                temp[blank1.rowCord][blank1.colCord - 1] = 'b';
                temp[blank2.rowCord][blank2.colCord] = p2ID;
                return 1;
            }
            else
                return 0;
        }
    }
    // Right
    if (position == "right") {
        if (blank1.colCord == 3 || blank2.colCord == 3)
            return 0;

        if (adjacentOrientation == "vertical") {
            char p1ID = board[blank1.rowCord][blank1.colCord + 1];
            char p2ID = board[blank2.rowCord][blank2.colCord + 1];

            if ((p1ID == p2ID && p1ID != '0') || (checkPiece(board, p1ID) == "square" && checkPiece(board, p2ID) == "square")) {
                temp[blank1.rowCord][blank1.colCord + 1] = 'b';
                temp[blank2.rowCord][blank2.colCord + 1] = 'b';
                temp[blank1.rowCord][blank1.colCord] = p1ID;
                temp[blank2.rowCord][blank2.colCord] = p2ID;
                return 1;
            }
            else if (checkPiece(board, p1ID) == "horizontal" && checkPiece(board, p2ID) == "horizontal") {
                temp[blank1.rowCord][blank1.colCord + 2] = 'b';
                temp[blank2.rowCord][blank2.colCord + 2] = 'b';
                temp[blank1.rowCord][blank1.colCord] = p1ID;
                temp[blank2.rowCord][blank2.colCord] = p2ID;
                return 1;
            }
            else if (checkPiece(board, p1ID) == "king" && checkPiece(board, p2ID) == "king") {
                temp[blank1.rowCord][blank1.colCord + 2] = 'b';
                temp[blank2.rowCord][blank2.colCord + 2] = 'b';
                temp[blank1.rowCord][blank1.colCord] = p1ID;
                temp[blank2.rowCord][blank2.colCord] = p2ID;
                return 1;
            }
            else
                return 0;
        }
        else if (adjacentOrientation == "horizontal") {
            if (blank1.colCord + 2 > 3 || blank2.colCord + 2 > 3) {
                char p1ID = board[blank2.rowCord][blank2.colCord + 1];
                if (checkPiece(board, p1ID) == "square") {
                    temp[blank2.rowCord][blank2.colCord + 1] = 'b';
                    temp[blank1.rowCord][blank1.colCord] = p1ID;
                    return 1;
                }
                else
                    return 0;
            }
            char p1ID = board[blank2.rowCord][blank2.colCord + 1];
            char p2ID = board[blank2.rowCord][blank2.colCord + 2];

            if (p1ID == '0' || p2ID == '0')
                return 0;

            if (p1ID == p2ID) {
                temp[blank2.rowCord][blank2.colCord + 2] = 'b';
                temp[blank2.rowCord][blank2.colCord + 1] = 'b';
                temp[blank1.rowCord][blank1.colCord] = p1ID;
                temp[blank2.rowCord][blank2.colCord] = p2ID;
                return 1;
            }
            else if (checkPiece(board, p1ID) == "square")
            {
                temp[blank2.rowCord][blank2.colCord + 1] = 'b';
                temp[blank1.rowCord][blank1.colCord] = p1ID;
                return 1;
            }
            else
                return 0;
        }
    }
    return 0;
}

/// <summary>
/// Populates global variables with coordinates of blank squares,
/// and if/how the blank squares are adjacent
/// </summary>
/// <param name="board">The board to search for the location of blank squares</param>
void findBlanks(vector<vector<char>> board)
{
    int firstFound = 0;

    // Find Blank Coordinates
    for (int i = 0; i < board.size(); i++) {
        for (int j = 0; j < board[i].size(); j++) {
            if (board[i][j] == 'b') {
                if (!firstFound) {
                    blank1.rowCord = i;
                    blank1.colCord = j;
                    firstFound = 1;
                }
                else {
                    blank2.rowCord = i;
                    blank2.colCord = j;
                }
            }
        }
    }

    // Determine if adjacent
    if (blank1.rowCord - 1 == blank2.rowCord || blank1.rowCord + 1 == blank2.rowCord) {
        if (blank1.colCord == blank2.colCord) {
            blanksAdjacent = 1;
            adjacentOrientation = "vertical";
        }
        else
            blanksAdjacent = 0;
    }
    else if (blank1.colCord - 1 == blank2.colCord || blank1.colCord + 1 == blank2.colCord) {
        if (blank1.rowCord == blank2.rowCord) {
            blanksAdjacent = 1;
            adjacentOrientation = "horizontal";
        }
        else
            blanksAdjacent = 0;
    }
    else
        blanksAdjacent = 0;
}

void printsolution(node* n)
{
    cout << endl << "-------- SOLUTION FOUND --------" << endl << endl;
    printBoard(n->board);
    cout << endl;

    while (n->parent) {
        n->parent->next = n;
        n = n->parent;
    }

    int count = -1;
    while (n) {
        count++;
        n = n->next;
    }

    cout << "This solution has " << count << " steps." << endl << endl;
}

/// <summary>
/// Describes the type of piece given a identifier (examples: '1', '2', '3')
/// </summary>
/// <param name="board">The board to search against</param>
/// <param name="identifier">character that represents the piece on the 2D playing board</param>
/// <returns>string: description of the type of the piece</returns>
string checkPiece(vector<vector<char>> board, char identifier)
{
    if (identifier == 'b')
        return "blank";

    if (identifier == '0')
        return "king";

    int row = -1;
    int col = -1;

    for (int i = 0; i < board.size(); i++) {
        for (int j = 0; j < board[i].size(); j++) {
            if (board[i][j] == identifier) {
                row = i;
                col = j;
            }
        }
    }

    if (row == -1 or col == -1) {
        cout << "ERROR -- checkPiece()" << endl;
        return "error";
    }

    // Determine if adjacent
    if (row - 1 >= 0 && board[row - 1][col] == identifier || row + 1 < 5 && board[row + 1][col] == identifier)
        return "vertical";
    if (col - 1 >= 0 && board[row][col - 1] == identifier || col + 1 < 4 && board[row][col + 1] == identifier)
        return "horizontal";
    else
        return "square";
}

/// <summary>
/// Creates an output file showing the steps to finding the solution
/// </summary>
/// <param name="name">The name of the puzzle</param>
/// <param name="algorithm">The algorithm being used.</param>
/// <param name="solution">The solution node</param>
void createOutputFile(string name, string algorithm, node* solution)
{
    
    ofstream outFile;
    outFile.open(name + algorithm + ".out");

    while (solution->parent) {
        solution->parent->next = solution;
        solution = solution->parent;
    }

    int count = -1;
    while (solution) {
        count++;
        solution = solution->next;
        
        if (solution) {
            outFile << "Step: " << count << endl;
            for (int i = 0; i < solution->board.size(); i++) {
                for (int j = 0; j < solution->board[0].size(); j++) {
                    if (solution->board[i][j] == 'b') {
                        outFile << "  ";
                    }
                    else {
                        outFile << solution->board[i][j] << " ";
                    }
                }
                outFile << endl;
            }
        } 
        else
            outFile << "FINAL SOLUTION" << endl;
        
        outFile << endl;
    }

    outFile.close();
}

/// <summary>
/// Breadth First Search to find a solution to the game board
/// </summary>
/// <param name="board">The board to run BFS against</param>
void bfs(vector<vector<char>> board)
{
    cout << "Running BFS....." << endl;
    string moves[4] = { "up", "down", "left", "right" };
    
    queue<node*> open;
    nodeP* np;
    np = new nodeP[200000];
    int npCount = 0;
    set<string> close;
    node *start, *current, *succ;
    string sucnum;
    start = new node(board);
    vector<vector<char>> temp(5, vector<char>(4));
    int success = 0;

    open.push(start);
    np[npCount++] = start;
    close.insert(boardState(start->board));
    long gencount = 1;

    while (!open.empty() && !success)
    {
        current = open.front();
        open.pop();
        if (checkGoal(current->board))
        {
            printsolution(current);
            cout << "Total of: " << gencount << " nodes examined." << endl;
            createOutputFile(puzzleName, "BFS", current);
            success = 1;
        }
        else
        {
            for (int i = 0; i < 4; i++) {
                findBlanks(current->board);
                if (makeSingleMove(current->board, temp, blank1, moves[i])) {
                    sucnum = boardState(temp);
                    if (close.find(sucnum) == close.end()) {
                        succ = new node(temp, current);
                        close.insert(sucnum);
                        open.push(succ);
                        np[npCount++] = succ;
                        gencount++;
                    }
                    findBlanks(current->board);
                }
                if (makeSingleMove(current->board, temp, blank2, moves[i])) {
                    sucnum = boardState(temp);
                    if (close.find(sucnum) == close.end()) {
                        succ = new node(temp, current);
                        close.insert(sucnum);
                        open.push(succ);
                        np[npCount++] = succ;
                        gencount++;
                    }
                    findBlanks(current->board);
                }
                if (blanksAdjacent) {
                    if (makeDoubleMove(current->board, temp, moves[i])) {
                        sucnum = boardState(temp);
                        if (close.find(sucnum) == close.end()) {
                            succ = new node(temp, current);
                            close.insert(sucnum);
                            open.push(succ);
                            np[npCount++] = succ;
                            gencount++;
                        }
                        findBlanks(current->board);
                    }
                }
            }
        }
    }

    if (!success) {
        cout << "No solution.\n";
        cout << "Total of " << gencount
            << " nodes examined.\n\n";
    }

    for (int j = 0; j < npCount; j++)
        delete np[j];

    delete[] np;

    return;
}

/// <summary>
/// Depth First Search to find a solution to the game board
/// </summary>
/// <param name="board">The board to run DFS against</param>
void dfs(vector<vector<char>> board)
{
    cout << "Running DFS....." << endl;
    string moves[4] = { "up", "down", "left", "right" };
    
    stack<node*> open;
    nodeP* np;
    np = new nodeP[200000];
    int npCount = 0;
    set<string> close;
    node* start, * current, * succ;
    string sucnum;
    start = new node(board);
    vector<vector<char>> temp(5, vector<char>(4));
    int success = 0;

    open.push(start);
    np[npCount++] = start;
    close.insert(boardState(start->board));
    long gencount = 1;

    while (!open.empty() && !success)
    {
        current = open.top();
        open.pop();
        if (checkGoal(current->board))
        {
            printsolution(current);
            cout << "Total of: " << gencount << " nodes examined." << endl;
            createOutputFile(puzzleName, "DFS", current);
            success = 1;
        }
        else
        {
            for (int i = 0; i < 4; i++) {
                findBlanks(current->board);
                if (makeSingleMove(current->board, temp, blank1, moves[i])) {
                    sucnum = boardState(temp);
                    if (close.find(sucnum) == close.end()) {
                        succ = new node(temp, current);
                        close.insert(sucnum);
                        open.push(succ);
                        np[npCount++] = succ;
                        gencount++;
                    }
                    findBlanks(current->board);
                }

                if (makeSingleMove(current->board, temp, blank2, moves[i])) {
                    sucnum = boardState(temp);
                    if (close.find(sucnum) == close.end()) {
                        succ = new node(temp, current);
                        close.insert(sucnum);
                        open.push(succ);
                        np[npCount++] = succ;
                        gencount++;
                    }
                    findBlanks(current->board);
                }
                if (blanksAdjacent) {
                    if (makeDoubleMove(current->board, temp, moves[i])) {
                        sucnum = boardState(temp);
                        if (close.find(sucnum) == close.end()) {
                            succ = new node(temp, current);
                            close.insert(sucnum);
                            open.push(succ);
                            np[npCount++] = succ;
                            gencount++;
                        }
                        findBlanks(current->board);
                    }
                }
            }
        }
    }

    if (!success) {
        cout << "No solution.\n";
        cout << "Total of " << gencount
            << " nodes examined.\n\n";
    }

    for (int j = 0; j < npCount; j++)
        delete np[j];

    delete[] np;

    return;
}
