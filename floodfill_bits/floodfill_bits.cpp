#include <bitset>
#include <cstdint>
#include <iostream>
#include <utility>
using namespace std;


constexpr int WIDTH = 8;
constexpr int HEIGHT = 8;
constexpr int SQUARES = WIDTH*HEIGHT;


typedef uint64_t u64;
typedef bitset<64> Board;


constexpr u64 R_BORDER = 0x8080808080808080ULL;
constexpr u64 L_BORDER = 0x0101010101010101ULL;
constexpr u64 T_BORDER = 0xFF00000000000000ULL;
constexpr u64 B_BORDER = 0x00000000000000FFULL;

// shift right
constexpr int LEFT = 1;
constexpr int UP_LEFT = WIDTH+1;
constexpr int UP = WIDTH;
constexpr int UP_RIGHT = WIDTH-1;

// shift left
constexpr int RIGHT = -1;
constexpr int DOWN_RIGHT = -(WIDTH+1);
constexpr int DOWN = -WIDTH;
constexpr int DOWN_LEFT = -(WIDTH-1);

constexpr int DIRECTIONS[8] = { LEFT, UP_LEFT, UP, UP_RIGHT, RIGHT, DOWN_RIGHT, DOWN, DOWN_LEFT };
constexpr size_t DIRECTION_BLOCKERS[8] = { R_BORDER, R_BORDER, 0, L_BORDER, L_BORDER, L_BORDER, 0, R_BORDER };


u64 REACHABLE[SQUARES];
u64 BLOCKED[SQUARES][SQUARES];


u64 signed_shift(u64 n, int s) {
    if (s < 0) {
        n = n >> -s;
    }
    else {
        n = n << s;
    }
    return n;
}

u64 shift_sliding_piece(u64 sliding_piece, u64 obstacles, int direction) {
    u64 destinations = 0;
    while ((sliding_piece=signed_shift(sliding_piece, direction)&~obstacles)) {
        destinations |= sliding_piece;
    }
    return destinations;
}


void init() {
    for (int i = 0; i < SQUARES; ++i) {
        
    }
}

void show_board(const Board& board) {
    for (int i = 0; i < HEIGHT; ++i) {
        for (int j = 0; j < WIDTH; ++j) {
            int index = i*8+j;
            cout << board[index];
        }
        cout << endl;
    }
}

void set(Board& board, int i, int j, bool v) {
    board[i*WIDTH+j] = v;
}

template<u64 mask, int dir>
u64 floodfill_expand(u64 seed, u64 obstacles) {
    if constexpr (dir < 0) {
        seed |= (seed>>-dir)&~(obstacles|mask);
    }
    else {
        seed |= (seed<<dir)&~(obstacles|mask);
    }
    return seed;
}

u64 floodfill_expand(u64 seed, u64 obstacles) {
    u64 expanded = seed;
    expanded |= (seed>>1)&~obstacles&~R_BORDER;
    expanded |= (seed>>(WIDTH+1))&~obstacles&~R_BORDER;
    expanded |= (seed>>WIDTH)&~obstacles;
    expanded |= (seed>>(WIDTH-1))&~obstacles&~L_BORDER;
    expanded |= (seed<<1)&~obstacles&~L_BORDER;
    expanded |= (seed<<(WIDTH+1))&~obstacles&~L_BORDER;
    expanded |= (seed<<WIDTH)&~obstacles;
    expanded |= (seed<<(WIDTH-1))&~obstacles&~R_BORDER;
    return expanded;
}

pair<u64,u64> voronoi(u64 p1, u64 p2, u64 obstacles) {
    u64 prev_p1, prev_p2;
    do {    
        prev_p1 = p1;
        prev_p2 = p2;
        u64 frontier_p1 = p1^floodfill_expand(p1, obstacles|p2);
        u64 frontier_p2 = p2^floodfill_expand(p2, obstacles|p1);
        p1 |= frontier_p1&~frontier_p2;
        p2 |= frontier_p2&~frontier_p1;
    } while (p1 != prev_p1 || p2 != prev_p2);
    return make_pair(p1, p2);
}

u64 floodfill(u64 seed, u64 obstacles) {
    u64 prev = seed;
    while ((seed=floodfill_expand(seed,obstacles)) != prev) {
        prev = seed;
    }
    return seed;
}


u64 get_queen_moves(u64 amazon, u64 obstacles) {
    u64 destinations = 0;
    destinations |= shift_sliding_piece(amazon, obstacles|R_BORDER, UP_RIGHT);
    destinations |= shift_sliding_piece(amazon, obstacles|R_BORDER, RIGHT);
    destinations |= shift_sliding_piece(amazon, obstacles|R_BORDER, DOWN_RIGHT);
    destinations |= shift_sliding_piece(amazon, obstacles, DOWN);
    destinations |= shift_sliding_piece(amazon, obstacles|L_BORDER, DOWN_LEFT);
    destinations |= shift_sliding_piece(amazon, obstacles|L_BORDER, LEFT);
    destinations |= shift_sliding_piece(amazon, obstacles|L_BORDER, UP_LEFT);
    destinations |= shift_sliding_piece(amazon, obstacles, UP);
    return destinations;
}


int main() {
    
    Board b = 0;
    Board obs = 0;
    
    set(b, 0,0, true);
    
    set(obs, 2,3,true);
    set(obs, 1,1, true);
    
    show_board(b); cout << endl;
    show_board(obs); cout << endl;
    
    show_board(get_queen_moves(b.to_ullong(), obs.to_ullong()));
    
    /*
    Board seed1, seed2, obstacles;
    
    set(seed1, 3, 3, true);


    set(seed2, 6,7, true);
    
    auto[p1,p2] = voronoi(seed1.to_ullong(), seed2.to_ullong(), 0);
    
    //show_board(seed1); cout << endl;
    show_board(p1); cout << endl;
    
    //show_board(seed2); cout << endl;
    show_board(p1|p2); cout << endl;
    */
    
    /*
    set(obstacles, 1, 1, true);
    set(obstacles, 1, 2, true);
    set(obstacles, 1, 3, true);
    set(obstacles, 1, 4, true);
    set(obstacles, 2, 4, true);
    set(obstacles, 2, 5, true);
    set(obstacles, 3, 5, true);
    set(obstacles, 4, 5, true);
    set(obstacles, 5, 5, true);
    set(obstacles, 6, 5, true);
    set(obstacles, 6, 4, true);
    set(obstacles, 6, 3, true);
    set(obstacles, 5, 3, true);
    set(obstacles, 5, 2, true);
    set(obstacles, 5, 1, true);
    set(obstacles, 4, 1, true);
    set(obstacles, 3, 1, true);
    set(obstacles, 2, 1, true);
    */
    
    //show_board(obstacles); cout << endl;
    
    //u64 mask = floodfill(seed.to_ullong(), obstacles.to_ullong());
    
    //show_board(mask);
    
    //show_board(board);
    
}
