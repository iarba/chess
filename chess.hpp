#ifndef Included_chess_h
#define Included_chess_h
#include <vector>
#include "defines.hpp"

using namespace std;

enum Color
{
  WHITE, BLACK
};

enum Type
{
  NOTHING, PAWN, ROOK, KNIGHT, BISHOP, QUEEN, KING
};

class Piece
{
public:
  Piece();
  void set_piece(Color c, Type t, int m);
  void set_type(Type t);
  Color color;
  Type type;
  int moves;
};

class Move
{
public:
  Move(char px1, int py1, char px2, int py2, char special);
  int px1, py1, px2, py2;
  char special;
};

class Board
{
public:
  Board();
  Board(Board& other);
  ~Board();
  int can_move(Move m);
  int do_move(Move m);
  int get_status();
  bool is_threatened(int px, int py, Color c);
  bool white_to_move;
  Piece **pieces;
  vector<Move> history;
  int testing;
protected:
  int en_passant_loc;
};

/* Board statuses:
 * 0 - nothing happened
 * 1 - white is in check
 * 2 - black is in check
 * 3 - both players are in check
 */

/* misc functions */

bool out_of_board(int px, int py);

#endif
