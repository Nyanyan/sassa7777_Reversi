//
//  reversi.h
//  othello
//
//  Created by sasa on 2023/09/30.
//

#ifndef reversi_h
#define reversi_h

#define BLACK_TURN 100
#define WHITE_TURN -100
#define UP_BOARD 0xFF00000000000000ULL
#define DOWN_BOARD 0x00000000000000FFULL
#define LEFT_BOARD 0x8080808080808080ULL
#define RIGHT_BOARD 0x0101010101010101ULL
#define MIN_INF -2147483646
#define MAX_INF 2147483646

#include <iostream>
#include <bit>
#include <cstdint>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <string>

extern int DEPTH;
extern int Level;
extern int px, py;
extern int whitec;
extern int blackc;
extern int tmpx, tmpy;
extern uint64_t tmpbit;
extern int think_percent;
extern int think_count;
extern int botplayer;
extern int cpu_core;
extern int nowTurn;
extern int nowIndex;
extern int firstDEPTH;
extern int afterIndex;
extern uint64_t legalboard;
extern uint64_t rev;

class board{
public:
    uint64_t playerboard;
    uint64_t opponentboard;
    int score;
public:
    struct hash {
        size_t operator()(const board& b) const {
            return std::hash<std::string>{}(std::to_string(b.playerboard)+"&"+std::to_string(b.opponentboard));
        }
    };
    bool operator<(const board& b) const {
        return score > b.score;
    }
    bool operator==(const board& other) const {
        return this->playerboard == other.playerboard && this->opponentboard == other.opponentboard;
    }
};

class move_order{
public:
    uint64_t playerboard;
    uint64_t opponentboard;
    uint64_t put;
    int score;
public:
    bool operator<(const move_order& m) const {
        return score > m.score;
    }
};

extern board b;


//main functions
void reset(void);
int winner(void);
int ai(void);
int putstone(char y, char x);
uint64_t cordinate_to_bit(char *x, char *y);
bool canput(uint64_t *put, uint64_t *legalboard);
uint64_t makelegalBoard(uint64_t *playerboard, uint64_t *opponentboard);
void reversebit(uint64_t put);
uint64_t transfer(uint64_t *put, char *i);
bool isPass(void);
bool isFinished(void);
void swapboard(void);
int bitcount(uint64_t bits);
void revbit(uint64_t *put, uint64_t *playerboard, uint64_t *opponentboard, uint64_t *rev);
void moveordering(uint64_t moveorder[64], uint64_t *playerboard, uint64_t *opponentboard);
int nega_alpha(char depth, int alpha, int beta, uint64_t *playerboard, uint64_t *opponentboard);
int nega_alpha_transpose_table(char depth, int alpha, int beta, uint64_t *playerboard, uint64_t *opponentboard);
int nega_alpha_moveorder(char depth, int alpha, int beta, uint64_t *playerboard, uint64_t *opponentboard);
int nega_scout(char depth, int alpha, int beta, uint64_t *playerboard, uint64_t *opponentboard);
void search(uint64_t *playerboard, uint64_t *opponentboard);
int nega_alpha_move_order(char depth, int alpha, int beta, uint64_t *playerboard, uint64_t *opponentboard, uint64_t *put);

//flip boards
void flipHorizontal(uint64_t *x);
void flipVertical(uint64_t *x);
void flipDiagonalA8H1(uint64_t *x);
void flipDiagonalA1H8(uint64_t *x);
void rotateClockwise90(uint64_t *x);
void rotateCounterclockwise90(uint64_t *x);

//evaluation
int score_stone(uint64_t *playerboard, uint64_t *opponentboard);
int score_stone2(uint64_t *playerboard, uint64_t *opponentboard);
int score_putable(uint64_t *playerboard, uint64_t *opponentboard);
int score_fixedstone(uint64_t *playerboard, uint64_t *opponentboard);
int countscore(uint64_t *playerboard, uint64_t *opponentboard, int *afterIndex);

#endif /* othello_h */
