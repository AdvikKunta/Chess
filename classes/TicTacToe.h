#pragma once
#include "Game.h"
#include "Square.h"
#include "RockPaperScissors.h"

// My implementation of the TicTacToe game
// Advik Kunta

class TicTacToe : public Game{

public:
    TicTacToe();
    ~TicTacToe();
    
    //I'm assuming that everything will end up being a different implementation than
    //How it is in Game.cpp so I would want to override all these functions
    void setUpBoard() override;
    Player* checkForWinner() override;
    bool checkForDraw() override;
    //i'll likely be overriding these functions too so I'll just copy this over
    std::string initialStateString() override;
    std::string stateString() override;
    void setStateString(const std::string &s) override;
    bool actionForEmptyHolder(BitHolder &holder) override;
    bool canBitMoveFrom(Bit&bit, BitHolder &src) override;
    bool canBitMoveFromTo(Bit& bit, BitHolder& src, BitHolder& dst) override;
    void stopGame() override;
    BitHolder &getHolderAt(const int x, const int y) override { return _grid[x][y]; }
    bool gameHasAI() override;
    void updateAI() override;
    int negamax(Square board[3][3], int depth, int alpha, int beta, int player);
//changing grid size so it can be a 2d grid that is 3x3
private:
    Bit *PieceForPlayer(const int playerNumber);
    Player* ownerAt(int x, int y);
    void    scanForMouse();
    Square   _grid[3][3];
};