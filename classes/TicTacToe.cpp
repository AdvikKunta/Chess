#include "TicTacToe.h"
#include "../log.h"
#include <cstring>
#include <memory>
#include <string>

TicTacToe::TicTacToe(){

}

TicTacToe::~TicTacToe(){

}

void TicTacToe::setUpBoard(){
    setNumberOfPlayers(2);
    // Going to make a grid of 3x3 for tictactoe
    Log::log(Log::INFO, "Making a 3x3 board for Tic Tac Toe");
    _gameOptions.rowX = 3;
    _gameOptions.rowY = 3;

    for (int x=0; x<3; x++) {
        for(int y = 0; y < 3; y++){
            _grid[x][y].initHolder(ImVec2(100*(float)x + 100, 100*(float)y + 100), "square.png", x, y);
        }
    }

    // allowing use of an AI if playing with that
    if (gameHasAI())
    {
        Log::log(Log::INFO, "Initializing the AI player");
        setAIPlayer(_gameOptions.AIPlayer);
    }
    // start the game
    Log::log(Log::INFO, "Starting Game");
    startGame();
}

Player* TicTacToe::checkForWinner(){
    // check each row to make sure that if there is a 3 in a row that has a win, it gets counted
    for (int row = 0; row < 3; ++row) {
        if (_grid[row][0].bit() && _grid[row][1].bit() && _grid[row][2].bit() && 
            _grid[row][0].bit()->gameTag() == _grid[row][1].bit()->gameTag() &&
            _grid[row][0].bit()->gameTag() == _grid[row][2].bit()->gameTag()) {
                return _grid[row][0].bit()->gameTag() == 1 ? _players[0] : _players[1];
        }
    }
    // do the same thing but this time for column
    for (int col = 0; col < 3; ++col) {
        if (_grid[0][col].bit() && _grid[1][col].bit() && _grid[2][col].bit() &&
            _grid[0][col].bit()->gameTag() == _grid[1][col].bit()->gameTag() &&
            _grid[0][col].bit()->gameTag() == _grid[2][col].bit()->gameTag()) {
                return _grid[0][col].bit()->gameTag() == 1 ? _players[0] : _players[1];
        }
    }

    // same thing, but for diagonals (only 2 so you don't need a loop)
    if (_grid[0][0].bit() && _grid[1][1].bit() && _grid[2][2].bit() &&
        _grid[0][0].bit()->gameTag() == _grid[1][1].bit()->gameTag() &&
        _grid[0][0].bit()->gameTag() == _grid[2][2].bit()->gameTag()) {
            return _grid[0][0].bit()->gameTag() == 1 ? _players[0] : _players[1];
    }
    if (_grid[0][2].bit() && _grid[1][1].bit() && _grid[2][0].bit() &&
        _grid[0][2].bit()->gameTag() == _grid[1][1].bit()->gameTag() &&
        _grid[0][2].bit()->gameTag() == _grid[2][0].bit()->gameTag()) {
            return _grid[0][2].bit()->gameTag() == 1 ? _players[0] : _players[1];
    }

    // if a winner hasn't been determined yet, return a nullptr
    return nullptr;
}

bool TicTacToe::checkForDraw(){
    // double checking that all squares are filled 
    Player* winner = checkForWinner();
    if(!winner){
        bool allFilled = true;
        for (int row = 0; row < 3; ++row) {
            for (int col = 0; col < 3; ++col) {
                if (!_grid[row][col].bit()) {
                    allFilled = false;
                    break;
                }
            }
            if (!allFilled) {
                break;
            }
        }
        if(allFilled){
            return true;
        }
    }
    return false;
}

std::string TicTacToe::initialStateString(){
    return "";
}

std::string TicTacToe::stateString(){
    std::string s;
    for (int y=0; y<3; y++) {
        for(int x = 0; x<3; x++){
            Bit *bit = _grid[x][y].bit();
            if (bit) {
                s += std::to_string(bit->gameTag());
            } else {
                s += "0";
            }
        }
        
    }
    return s;
}

void TicTacToe::setStateString(const std::string &s){
    for (int y=0; y<3; y++) {
        for(int x = 0; x<3; x++){
            _grid[x][y].setBit( PieceForPlayer(0) );
        }
    }
}


bool TicTacToe::actionForEmptyHolder(BitHolder& holder){
    if(checkForWinner()){
        return false;
    }
    if (holder.bit()) {
        return false;
    }
    Bit *bit = PieceForPlayer(getCurrentPlayer()->playerNumber());
    if (bit) {
        bit->setPosition(holder.getPosition());
        Log::log(Log::INFO, "Setting position for player");
        holder.setBit(bit);
        endTurn();
        Log::log(Log::INFO, "Ending turn");

        return true;
    }   
    return false;
}
//these functions are in the rockpaperscissors class but since you cannot MOVE pieces around i'll assume that
//we copy them over here for the most part
bool TicTacToe::canBitMoveFrom(Bit& bit, BitHolder& src){
    return false;
}

bool TicTacToe::canBitMoveFromTo(Bit& bit, BitHolder& src, BitHolder& dst){
    return false;
}

void TicTacToe::stopGame(){
    for (int x=0; x<3; x++) {
        for(int y = 0; y < 3; y++){
            _grid[x][y].destroyBit();
        }
    }
}

int TicTacToe::negamax(Square board[3][3], int depth, int alpha, int beta, int player) {
    for (int row = 0; row < 3; ++row) {
        if (board[row][0].bit() && board[row][1].bit() && board[row][2].bit() && 
            board[row][0].bit()->gameTag() == board[row][1].bit()->gameTag() &&
            board[row][0].bit()->gameTag() == board[row][2].bit()->gameTag()) {
                if(board[row][0].bit()->getOwner()->playerNumber() == getAIPlayer()){
                    return depth;
                }
                else{
                    return -1 * (depth);
                }
        }
    }
    // do the same thing but this time for column
    for (int col = 0; col < 3; ++col) {
        if (board[0][col].bit() && board[1][col].bit() && board[2][col].bit() &&
            board[0][col].bit()->gameTag() == board[1][col].bit()->gameTag() &&
            board[0][col].bit()->gameTag() == board[2][col].bit()->gameTag()) {
                if(board[0][col].bit()->getOwner()->playerNumber() == getAIPlayer()){
                    return depth;
                }
                else{
                    return -1 * (depth);
                }
        }
    }
    if (board[0][0].bit() && board[1][1].bit() && board[2][2].bit() &&
        board[0][0].bit()->gameTag() == board[1][1].bit()->gameTag() &&
        board[0][0].bit()->gameTag() == board[2][2].bit()->gameTag()) {
            if(board[0][0].bit()->getOwner()->playerNumber() == getAIPlayer()){
                return depth;
            }
            else{
                return -1 * (depth);
            }
    }
    if (board[0][2].bit() && board[1][1].bit() && board[2][0].bit() &&
        board[0][2].bit()->gameTag() == board[1][1].bit()->gameTag() &&
        board[0][2].bit()->gameTag() == board[2][0].bit()->gameTag()) {
            if(board[0][2].bit()->getOwner()->playerNumber() == getAIPlayer()){
                return depth;
            }
            else{
                return -1 * (depth);
            }
    }

    bool allFilled = true;
    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 3; ++col) {
            if (!board[row][col].bit()) {
                allFilled = false;
                break;
            }
        }
        if (!allFilled) {   
            break;
        }
    }
    if(allFilled || depth == 0){
        return 0;
    }
    int newPlayer;
    int bestScore = -std::numeric_limits<int>::max();
    for (int x = 0; x < 3; x++) {
        for (int y = 0; y < 3; y++) {
            if (!board[x][y].bit()) {
                // making a move to throw in recursion
                board[x][y].setBit(PieceForPlayer(player));
                // getting the next player to put in recursion
                if(player == 1){
                    newPlayer = 0;
                }else{
                    newPlayer = 1;
                }
                int score = -negamax(board, depth - 1, -beta, -alpha, newPlayer);
                // deleting the move so it doesn't actually happen
                board[x][y].destroyBit();

                bestScore = std::max(bestScore, score);
                // alpha = std::max(alpha, bestScore);
                // if (alpha >= beta) {
                //     break; // Beta cutoff
                // }
            }
        }
    }
    return bestScore;
}


bool TicTacToe::gameHasAI(){
	return _gameOptions.AIPlayer;
}

void TicTacToe::updateAI(){
    /* random nonsense, I was just testing to see how exactly i would set pieces by the AI
    bool what = false;
    while(what == false){
        if(checkForWinner()){
            break;
        }
        int randomRow = rand() % 3;
        int randomCol = rand() % 3;
        what = actionForEmptyHolder(_grid[randomRow][randomCol]);
    }
    */
    int bestScore = -std::numeric_limits<int>::max();
    int bestX = -1;
    int bestY = -1;
 
    for (int x = 0; x < 3; x++) {
        for (int y = 0; y < 3; y++) {
            if (!_grid[x][y].bit()) {
                // Make a move
                _grid[x][y].setBit(PieceForPlayer(_gameOptions.AIPlayer));

                int score = -negamax(_grid, 9, -std::numeric_limits<int>::max(), std::numeric_limits<int>::max(), 0);
                // Undo the move
                _grid[x][y].destroyBit();

                if (score > bestScore) {
                    bestScore = score;
                    bestX = x;
                    bestY = y;
                }
            }
        }
    }
    actionForEmptyHolder(_grid[bestX][bestY]);
    
}

Bit* TicTacToe::PieceForPlayer(const int playerNumber){
    const char *textures[] = {"x.png", "o.png"};
    Bit *bit = new Bit();

    bit->LoadTextureFromFile(textures[playerNumber]);
    bit->setOwner(getPlayerAt(playerNumber));
    bit->setGameTag(playerNumber+1);
    bit->setSize(pieceSize, pieceSize);
    return bit;
}

Player* TicTacToe::ownerAt(int x, int y)
{
    if ((x < 0 || x > 2) || (y < 0 || y > 2)) {
        return nullptr;
    }
    if (!_grid[x][y].bit()) {
        return nullptr;
    }
    return _grid[x][y].bit()->getOwner();
}