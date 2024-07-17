#pragma once
#include "Game.h"
#include "Square.h"
#include "ChessSquare.h"
#include "Evaluate.h"
#include <utility>
#include <vector>

//My Chess Game - Advik Kunta

const int pieceSize = 70;

enum ChessPiece{
    Pawn = 1,
    Knight,
    Bishop,
    Rook,
    Queen,
    King
};

class Chess : public Game{

public:
    Chess();
    ~Chess();

    struct Move {
        std::string from;
        std::string to;
        int special;
    };

    void FENtoBoard(const std::string& fen);
    void setUpBoard() override;
    Player* checkForWinner() override;
    bool checkForDraw() override;

    std::string initialStateString() override;
    std::string stateString() override;
    void setStateString(const std::string &s) override;

    bool actionForEmptyHolder(BitHolder &holder) override {return false;}
    bool canBitMoveFrom(Bit&bit, BitHolder &src) override;
    bool canBitMoveFromTo(Bit& bit, BitHolder& src, BitHolder& dst) override;
    void bitMovedFromTo(Bit &bit, BitHolder& src, BitHolder& dst) override;

    void stopGame() override;
    BitHolder &getHolderAt(const int x, const int y) override { return _grid[x][y]; }
    int notationToIndex(std::string& notation);
    bool gameHasAI() override;
    void updateAI() override;
    int negamax(char* state, int depth, int alpha, int beta, int playerColor);
    int evaluateBoard(const char* state);
    void FENToBoard(const std::string& fen);
    bool filterOutIllegalMoves(std::vector<Move>& moves, char color);

    private:
    Bit *PieceForPlayer(const int playerNumber, ChessPiece piece);
    Player* ownerAt(int x, int y) const;
    void addMoveIfValid(const char *state, std::vector<Move>& moves, int fromRow, int fromCol, int toRow, int toCol);
    std::string indexToNotation(int row, int col);
    char pieceNotation(int row, int column) const;
    char stateNotation(const char* state, int row, int col);
    int stateColor(const char* state, int row, int col);
    
    void generateKnightMoves(const char *state, std::vector<Move>& moves, int row, int col);
    void generatePawnMoves(const char *state, std::vector<Move>& moves, int row, int col, int color);

    void generateLinearMoves(const char *state, std::vector<Move>& moves, int row, int col, const std::vector<std::pair<int, int>>& direction);
    void generateBishopMoves(const char *state, std::vector<Move>& moves, int row, int col);
    void generateRookMoves(const char *state, std::vector<Move>& moves, int row, int col);
    void generateQueenMoves(const char *state, std::vector<Move>& moves, int row, int col);
    void generateKingMoves(const char *state, std::vector<Move>& moves, int row, int col);

    std::vector<Chess::Move> generateMoves(const char *state, char color, bool filter);

    void    scanForMouse();
    ChessSquare _grid[8][8];
    std::vector<Move> _moves;
    std::string _state;
    int _countSearch = 0;
    bool _winner;
    int _winPlayerNum;
    bool _inCheck;
    bool _whiteCastle;
    bool _blackCastle;
};