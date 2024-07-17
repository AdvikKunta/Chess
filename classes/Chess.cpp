#include "Chess.h"
#include "../log.h"
#include <cctype>
#include <cstddef>
#include <cstring>
#include <ctype.h>
#include <memory>
#include <string>
#include <utility>
#include <vector>

using namespace std;

Chess::Chess() = default;
Chess::~Chess() = default;

void Chess::FENtoBoard(const string& fen) {
    for (int i = 0; i < 8; ++i)
        for (int j = 0; j < 8; ++j)
            _grid[i][j].setBit(nullptr);

    istringstream fenStream(fen);
    string boardPart;
    getline(fenStream, boardPart, ' ');

    int row = 0;
    int col = 0;
    for (char ch : boardPart) {
        if (ch == '/') {
            row++;
            col = 0;
        } else if (isdigit(ch)) {
            col += ch - '0'; // Skip empty squares
        } else {
            // convert ch to a piece
            ChessPiece piece = Pawn;
            switch (toupper(ch)) {
            case 'P':
                piece = Pawn;
                break;
            case 'N':
                piece = Knight;
                break;
            case 'B':
                piece = Bishop;
                break;
            case 'R':
                piece = Rook;
                break;
            case 'Q':
                piece = Queen;
                break;
            case 'K':
                piece = King;
                break;
            }
            Bit* bit = PieceForPlayer(isupper(ch) ? 0 : 1, piece);
            bit->setPosition(_grid[col][row].getPosition());
            bit->setParent(&_grid[col][row]);
            bit->setGameTag(isupper(ch) ? piece : (piece+128));
            _grid[col][row].setBit(bit);
            col++;
        }
    }
}

void Chess::setUpBoard(){
    setNumberOfPlayers(2);

    _gameOptions.rowX = 8;
    _gameOptions.rowY = 8;
    _winner = false;
    _winPlayerNum = -1;
    _inCheck = false;
    _whiteCastle = false;
    _blackCastle = false;

    for(int x = _gameOptions.rowX-1; x >= 0; x--){
        for(int y = _gameOptions.rowY-1; y >= 0; y--){
            ImVec2 position((float)(pieceSize * x + pieceSize), (float)(pieceSize * y + pieceSize));
            
            const char* spriteName = "boardsquare.png";
            _grid[x][y].initHolder(position, spriteName, x, y);
            _grid[x][y].setGameTag(0);
            _grid[x][y].setNotation(indexToNotation(x, y));
        }
    }
    // testing out all but pawns
    // FENtoBoard("rnbqkbnr/8/8/8/8/8/8/RNBQKBNR");
    // testing out only king queen and rook
    // FENtoBoard("r2qk2r/8/8/8/8/8/8/R2QK2R");

    // regular board
    FENtoBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");

    // stalemate checker
    // FENtoBoard("4Q3/6k1/Q7/8/8/5N2/PPPP1PPP/RNB1R1K1");

    if (gameHasAI())
    {
        setAIPlayer(getAIPlayer());
    }
    // start the game
    // Log::log(Log::INFO, "Starting Game");
    _moves = generateMoves(stateString().c_str(), 'W', true);
    _state = stateString();
    startGame();
}

Player* Chess::checkForWinner(){
    if(_moves.empty() && _inCheck == true){
        _winner = true;
    }
    if(_winner == true && _inCheck == true){
        return _players[getCurrentPlayer()->playerNumber()];
    }
    return nullptr;
}

bool Chess::checkForDraw(){
    if(_moves.empty() && _inCheck == false){
        return true;
    }
    return false;
}

string Chess::initialStateString(){
    string s;
    for(int y = 0; y < _gameOptions.rowY; y++){
        for(int x = 0; x < _gameOptions.rowX; x++){
            s += pieceNotation(x, y);
        }
    }
    return s;
}

string Chess::stateString(){
    string s;
    for(int y = 0; y < _gameOptions.rowY; y++){
        for(int x = 0; x < _gameOptions.rowX; x++){
            s += pieceNotation(x, y);
        }
    }
    return s;
}

void Chess::setStateString(const string &s){
    for(int y = 0; y < _gameOptions.rowY; y++){
        for(int x = 0; x < _gameOptions.rowX; x++){
            int index = y * _gameOptions.rowX + x;
            int playerNumber = s[index] - '0';
            if(playerNumber){
                _grid[x][y].setBit(PieceForPlayer(playerNumber-1, Pawn));
            }
            else{
                _grid[x][y].setBit( nullptr );
            }
            
        }
    }
}

bool Chess::canBitMoveFrom(Bit&bit, BitHolder &src){
    ChessSquare &srcSquare = static_cast<ChessSquare&>(src);
    for(auto const& move : _moves){
        if(move.from == srcSquare.getNotation()){
            return true;
        }
    }
    return false;
}

bool Chess::canBitMoveFromTo(Bit& bit, BitHolder& src, BitHolder& dst){
    ChessSquare &srcSquare = static_cast<ChessSquare&>(src);
    ChessSquare &dstSquare = static_cast<ChessSquare&>(dst);
    for(auto const& move : _moves){
        if(move.from == srcSquare.getNotation() && move.to == dstSquare.getNotation()){
            return true;
        }
    }
    return false;
}

void Chess::bitMovedFromTo(Bit& bit, BitHolder& src, BitHolder& dst){
    Game::bitMovedFromTo(bit, src, dst);
    ChessSquare &srcSquare = static_cast<ChessSquare&>(src);
    ChessSquare &dstSquare = static_cast<ChessSquare&>(dst);
    for(auto const& move : _moves){
        if(move.from == srcSquare.getNotation() && move.to == dstSquare.getNotation() && move.special == 1){
            // en passant
            _grid[dstSquare.getColumn()][(_gameOptions.currentTurnNo&1) ? dstSquare.getRow()+1 : dstSquare.getRow()-1].setBit(nullptr);
        }
        else if(move.from == srcSquare.getNotation() && move.to == dstSquare.getNotation() && move.special == 2){
            // castling
            if(dstSquare.getRow() == 7){
                if(dstSquare.getColumn() == 2){
                    Bit* bit = PieceForPlayer(0, Rook);
                    bit->setPosition(_grid[3][7].getPosition());
                    bit->setParent(&_grid[3][7]);
                    bit->setGameTag(Rook);
                    _grid[3][7].setBit(bit);
                    _grid[0][7].setBit(nullptr);
                    _whiteCastle = true;
                } else if(dstSquare.getColumn() == 6){
                    Bit* bit = PieceForPlayer(0, Rook);
                    bit->setPosition(_grid[5][7].getPosition());
                    bit->setParent(&_grid[5][7]);
                    bit->setGameTag(Rook);
                    _grid[5][7].setBit(bit);
                    _grid[7][7].setBit(nullptr);
                    _whiteCastle = true;
                }
            } else if(dstSquare.getRow() == 0){
                if(dstSquare.getColumn() == 2){
                    Bit* bit = PieceForPlayer(1, Rook);
                    bit->setPosition(_grid[3][0].getPosition());
                    bit->setParent(&_grid[3][0]);
                    bit->setGameTag(Rook);
                    _grid[3][0].setBit(bit);
                    _grid[0][0].setBit(nullptr);
                    _blackCastle = true;
                } else if(dstSquare.getColumn() == 6){
                    Bit* bit = PieceForPlayer(1, Rook);
                    bit->setPosition(_grid[5][0].getPosition());
                    bit->setParent(&_grid[5][0]);
                    bit->setGameTag(Rook + 128);
                    _grid[5][0].setBit(bit);
                    _grid[7][0].setBit(nullptr);
                    _blackCastle = true;
                }
            }
        }
        else if(move.from == srcSquare.getNotation() && move.to == dstSquare.getNotation() && move.special == 3){
            //queen promotion
            Bit* bit = PieceForPlayer(((_gameOptions.currentTurnNo&1) ? 0 : 1), Queen);
            bit->setPosition(_grid[dstSquare.getColumn()][dstSquare.getRow()].getPosition());
            bit->setParent(&_grid[dstSquare.getColumn()][dstSquare.getRow()]);
            bit->setGameTag((_gameOptions.currentTurnNo&1) ? Queen : Queen + 128);
            _grid[dstSquare.getColumn()][dstSquare.getRow()].setBit(bit);
        }
    }
    _moves = generateMoves(stateString().c_str(), (_gameOptions.currentTurnNo&1) ? 'B' : 'W', true);

}

void Chess::stopGame(){
    for(int y = 0; y < _gameOptions.rowY; y++){
        for(int x = 0; x < _gameOptions.rowX; x++){
            _grid[x][y].destroyBit();
        }
    }
}

Bit* Chess::PieceForPlayer(const int playerNumber, ChessPiece piece){
    const char *pieces[] = {"pawn.png", "knight.png", "bishop.png", "rook.png", "queen.png", "king.png"};

    Bit *bit = new Bit();
    const char *pieceName = pieces[piece-1];

    string spritePath = string("chess/");
    if(playerNumber == 1) {
        spritePath += "b_";
    }else{
        spritePath += "w_";
    }
    spritePath += pieceName;
    bit->LoadTextureFromFile(spritePath.c_str());
    bit->setOwner(getPlayerAt(playerNumber));
    bit->setSize(pieceSize, pieceSize);
    return bit;
}

char Chess::stateNotation(const char* state, int row, int col) 
{
    return state[col * 8 + row];
}

int Chess::stateColor(const char* state, int row, int col) 
{
    // return 0 for empty, 1 for white, -1 for black
    char piece = stateNotation(state, row, col);
    if (piece == '0') {
        return 0;
    }
    return (piece < 'a') ? 1 : -1;
}

Player* Chess::ownerAt(int x, int y) const{
    if(x < 8 && x >= 0 && y < 8 && y >= 0 && _grid[x][y].bit()){
        return _grid[x][y].bit()->getOwner();
    }
    else{
        return nullptr;
    }
}

void Chess::addMoveIfValid(const char *state, vector<Move>& moves, int fromRow, int fromCol, int toRow, int toCol){
    if(toRow >= 0 && toRow < 8 && toCol >= 0 && toCol < 8){
        int spec = 0;
        if(stateColor(state, fromRow, fromCol) != stateColor(state, toRow, toCol)){
            if(toupper(stateNotation(state, fromRow, fromCol)) == 'P' && (toCol == 7 || toCol == 0)){
                spec = 3;
            }
            moves.push_back({indexToNotation(fromRow, fromCol), indexToNotation(toRow, toCol), spec});
        }
    }
}

string Chess::indexToNotation(int row, int col){
    return string(1, 'a' + row) + string(1, '8' - col);
}

char Chess::pieceNotation(int row, int column) const
{
    const char *wpieces = { "0PNBRQK" };
    const char *bpieces = { "0pnbrqk" };
    Bit *bit = _grid[row][column].bit();
    char notation = '0';
    if (bit) {
        notation = bit->gameTag() < 128 ? wpieces[bit->gameTag()] : bpieces[bit->gameTag()-128];
    }
    return notation;
}
void Chess::generateKnightMoves(const char *state, vector<Move>& moves, int row, int col){
    int xes[] = {2, 1, -1, -2, 2, 1, -1, -2};
    int yes[] = {1, 2, 2, 1, -1, -2, -2, -1};
    for(int i = 0; i < 8; i++){
        addMoveIfValid(state, moves, row, col, row + xes[i], col + yes[i]);
    }
}

void Chess::generatePawnMoves(const char *state, vector<Move>& moves, int row, int col, int color){
    int direction = (color == 1) ? -1 : 1;
    int startRow = (color == 1) ? 6 : 1;
    if(col + direction >= 0 && col + direction < 8 && row >= 0 && row < 8){
        if(stateNotation(state, row, col + direction) == '0'){
            addMoveIfValid(state, moves, row, col, row, col + direction);
            if(col == startRow && stateNotation(state, row , col + 2 * direction) == '0'){
                addMoveIfValid(state, moves, row, col, row, col + 2 * direction);
            }
        }
    }

    for(int i = -1; i <= 1; i += 2){
        if(col + direction >= 0 && col + direction < 8 && row + i >= 0 && row + i < 8){
            if(stateColor(state, row+i, col+direction) == (stateColor(state, row, col) * -1)){
                addMoveIfValid(state, moves, row, col, row + i, col + direction);
            }
            if(stateColor(state, row+i, col) == (stateColor(state, row, col) * -1) && toupper(stateNotation(state, row+i, col)) == 'P' && stateNotation(state, row+i, col+direction) == '0'){
                if(direction == -1 && stateNotation(_state.c_str(), row+i, col+(direction * 2)) == 'p'){
                    moves.push_back({indexToNotation(row, col), indexToNotation(row+i, col+direction), 1});
                }
                else if(direction == 1 && stateNotation(_state.c_str(), row + i, col + (direction * 2)) == 'P'){
                    moves.push_back({indexToNotation(row, col), indexToNotation(row+i, col+direction), 1});                    
                }
            
            }
        }
    }
}

void Chess::generateLinearMoves(const char *state, vector<Move>& moves, int row, int col, const vector<pair<int, int>>& directions){
    for(auto& dir: directions){
        int r = row + dir.first;
        int c = col + dir.second;
        
        while(r >= 0 && r < 8 && c >= 0 && c < 8){
            if(stateNotation(state, r, c) != '0'){
                addMoveIfValid(state, moves, row, col, r, c);
                break;
            }
            addMoveIfValid(state, moves, row, col, r, c);
            r += dir.first;
            c += dir.second;
        }
    }
}

void Chess::generateBishopMoves(const char *state, vector<Move>& moves, int row, int col){
    vector<pair<int, int>> directions = {{1, 1}, {-1, 1}, {1, -1}, {-1, -1}};
    generateLinearMoves(state, moves, row, col, directions);
}

void Chess::generateRookMoves(const char *state, vector<Move>& moves, int row, int col){
    vector<pair<int, int>> directions = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
    generateLinearMoves(state, moves, row, col, directions);
}

void Chess::generateQueenMoves(const char *state, vector<Move>& moves, int row, int col){
    generateRookMoves(state, moves, row, col);
    generateBishopMoves(state, moves, row, col);
}

void Chess::generateKingMoves(const char *state, vector<Move>& moves, int row, int col){
    vector<pair<int, int>> directions = {{1, 1}, {-1, 1}, {1, -1}, {-1, -1}, {1, 0}, {-1, 0}, {0, 1}, {0, -1}};
    for(auto& dir: directions){
        int r = row + dir.first;
        int c = col + dir.second;
        if(r >= 0 && r < 8 && c >= 0 && c < 8){
            addMoveIfValid(state, moves, row, col, r, c);
        }
    }
    if(stateColor(state, row, col) == 1 && row == 4 && col == 7){
        //checking left side for castle
        if(stateNotation(state, 3, 7) == '0' && stateNotation(state, 2, 7) == '0' && stateNotation(state, 1, 7) == '0' && stateNotation(state, 0, 7) == 'R'){
            // add queen side move if allowed
            moves.push_back({indexToNotation(row, col), indexToNotation(row-2, col), 2});
        }
        //checking right side for castle
        if(stateNotation(state, 5, 7) == '0' && stateNotation(state, 6, 7) == '0' && stateNotation(state, 7, 7) == 'R'){
            // add king side move
            moves.push_back({indexToNotation(row, col), indexToNotation(row+2, col), 2});
        }
    }
    else if(stateColor(state, row, col) == -1 && row == 4 && col == 0){
        //checking the black side now
        if(stateNotation(state, 3, 0) == '0' && stateNotation(state, 2, 0) == '0' && stateNotation(state, 1, 0) == '0' && stateNotation(state, 0, 0) == 'r'){
            // add queen side move if allowed
            moves.push_back({indexToNotation(row, col), indexToNotation(row-2, col), 2});
        }
        //checking right side for castle
        if(stateNotation(state, 5, 0) == '0' && stateNotation(state, 6, 0) == '0' && stateNotation(state, 7, 0) == 'r'){
            // add king side move
            moves.push_back({indexToNotation(row, col), indexToNotation(row+2, col), 2});
        }
    }
}

vector<Chess::Move> Chess::generateMoves(const char *state, char color, bool filter){
    vector<Move> moves;
    moves.reserve(64);
    int colorAsInt = (color == 'W') ? 1 : -1;
    for (int i=0; i<64; i++) {
        int row = i % 8;
        int col = i / 8;
        char piece = state[i];
        int pieceColor = (piece == '0') ? 0 : (piece < 'a') ? 1 : -1;
        if (pieceColor == colorAsInt) {
            if (piece > 'a') piece = piece - ('a' - 'A');
            switch (piece) {
                case 'N': // Knight
                    generateKnightMoves(state, moves, row, col);
                    break;
                case 'P': // Pawn
                    generatePawnMoves(state, moves, row, col, colorAsInt);
                    break;
                case 'B': // Bishop
                    generateBishopMoves(state, moves, row, col);
                    break;
                case 'R': // Rook
                    generateRookMoves(state, moves, row, col);
                    break;
                case 'Q': // Queen
                    generateQueenMoves(state, moves, row, col);
                    break;
                case 'K': // King
                    generateKingMoves(state, moves, row, col);
                    break;                
            }
        }
    }
    _state = stateString().c_str();
    if(filter == true){
        _inCheck = filterOutIllegalMoves(moves, color);
    }

    return moves;
}

bool Chess::filterOutIllegalMoves(vector<Move>& moves, char color){
    char baseState[65];
    auto copyState = string(stateString().c_str());
    bool inCheck = false;
    int kingSquare = -1;
    
    for (int i=0; i<64; i++) {
        if (copyState[i] == 'k' && color == 'B' ) { kingSquare = i; break; }
        if (copyState[i] == 'K' && color == 'W' ) { kingSquare = i; break; }
    }
    int saveKing = kingSquare;
    auto checkMoves = generateMoves(stateString().c_str(), color == 'B' ? 'W' : 'B', false);
    for(auto move : checkMoves){
        if(notationToIndex(move.to) == kingSquare) {inCheck = true;}
    }
    int i = 0;
    for (auto it = moves.begin(); it != moves.end();) {
        bool moveBad = false;
        strcpy(&baseState[0], copyState.c_str());
        int srcSquare = notationToIndex(it->from);
        int dstSquare = notationToIndex(it->to);
        // checking if it works without moving the king
        if(srcSquare == kingSquare) { 
            kingSquare = dstSquare; }
        else{ kingSquare = saveKing; }
        baseState[dstSquare] = baseState[srcSquare];
        baseState[srcSquare] = '0';
        auto oppositeMoves = generateMoves(baseState, color == 'W' ? 'B' : 'W', false);
        
        for (auto enemyMoves : oppositeMoves) {
            int enemyDst = notationToIndex(enemyMoves.to);
            if (enemyDst == kingSquare) {
                moveBad = true;
                break;                
            }
            if(color == 'W' && it->special == 2 && _whiteCastle == true){
                moveBad = true;
                break;
            } else if(color == 'B' && it->special == 2 && _blackCastle == true){
                moveBad = true;
                break;
            }
            if(it->special == 2 && (dstSquare == 6 || dstSquare == 62)){
                if (enemyDst == dstSquare - 1) {
                    moveBad = true;
                    break;                
                }
            }
            else if(it->special == 2 && (dstSquare == 2 || dstSquare == 58)){
                if (enemyDst == dstSquare + 1) {
                    moveBad = true;
                    break;                
                }
            }
            
            
        }
        if(inCheck == true && it->special == 2){
            // king is in check and the move is a castle
            moveBad = true;
        }
        if (moveBad) {
            it = moves.erase(it);
        } else {
            ++it;
        }
        kingSquare = saveKing;
    }
    return inCheck;
}

bool Chess::gameHasAI(){
    return _gameOptions.AIPlayer;
}


int Chess::notationToIndex(string& notation)
{
    int row = '8' - notation[1];
    int col = notation[0] - 'a';
    return row * 8 + col;
}

void Chess::updateAI(){
    char baseState[64];
    int bestScore = -999999;
    Move bestMove;
    string copyString = stateString();
    for(auto move: _moves){
        memcpy(&baseState[0], copyString.c_str(),64);
        int srcSquare = notationToIndex(move.from);
        int dstSquare = notationToIndex(move.to);
        baseState[dstSquare] = baseState[srcSquare];
        baseState[srcSquare] = '0';
        _countSearch = 0;
        int bestValue = -negamax(baseState, 3, -999999, 999999, 1);
        if(bestValue > bestScore){
            bestScore = bestValue;
            bestMove = move;
        }
    }
    if(bestScore != -999999){
        int srcSquare = notationToIndex(bestMove.from);
        int dstSquare = notationToIndex(bestMove.to);
        BitHolder& src = getHolderAt(srcSquare&7, srcSquare/8);
        BitHolder& dst = getHolderAt(dstSquare&7, dstSquare/8);
        Bit* bit = src.bit();
        dst.dropBitAtPoint(bit, ImVec2(0,0));
        src.setBit(nullptr);
        bitMovedFromTo(*bit, src, dst);
    }
    
}
static map<char, int> evaluateScores = {
        {'P', 100}, {'p', -100},
        {'N', 200}, {'n', -200},
        {'B', 230}, {'b', -230},
        {'R', 400}, {'r', -400},
        {'Q', 900}, {'q', -900},
        {'K', 2000}, {'k', -2000},
        {'0', 0}
    };
    
int Chess::evaluateBoard(const char* state){

    int score = 0;
    for(int i = 0; i < 64; i++){
        score += evaluateScores[state[i]];
    }
    for (int i=0; i<64; i++) {
        char piece = state[i];
        int j = FLIP(i);
        switch (piece) {
            case 'N': // Knight
                score += knightTable[j];
                break;
            case 'n':
                score -= knightTable[FLIP(j)];
                break;
            case 'P': // Knight
                score += pawnTable[j];
                break;
            case 'p':
                score -= pawnTable[FLIP(j)];
                break;
            case 'K': // Knight
                score += kingTable[j];
                break;
            case 'k':
                score -= kingTable[FLIP(j)];
                break;
            case 'R': // Knight
                score += rookTable[j];
                break;
            case 'r':
                score -= rookTable[FLIP(j)];
                break;
            case 'Q': // Knight
                score += queenTable[j];
                break;
            case 'q':
                score -= queenTable[FLIP(j)];
                break;
        }
    }
    return score;
}

int Chess::negamax(char *state, int depth, int alpha, int beta, int playerColor){
    _countSearch++;
    int score = evaluateBoard(state);

    if(depth == 0) return playerColor * score;

    vector<Chess::Move> nMoves = generateMoves(state, playerColor==1 ? 'W' : 'B', true);
    int bestVal = -999999;
    for(auto moves: nMoves){
        int srcSquare = notationToIndex(moves.from);
        int dstSquare = notationToIndex(moves.to);
        char save = state[dstSquare];
        state[dstSquare] = state[srcSquare];
        state[srcSquare] = '0';
        bestVal = max(bestVal, -negamax(state,depth-1, -beta, -alpha, -playerColor));
        state[srcSquare] = state[dstSquare];
        state[dstSquare] = save;
        alpha = max(alpha, bestVal);
        if (alpha >= beta) {
            break;
        }
    }
    return bestVal;

}
