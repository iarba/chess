#ifndef Included_chess_h
#define Included_chess_h

class Chess_game
{
public:
  Chess_game();
protected:
  Board b;
}

class Board
{
public:
  Board();
  Board(Board&);
  ~Board();
  Piece get_piece(char, int);
protected:
  bool white_to_move;
  Piece[MAX_CHESS_SIZE][MAX_CHESS_SIZE] pieces;
}

class Piece
{
public:
  enum Color
  {
    WHITE, BLACK
  };
  enum Type
  {
    PAWN, ROOK, KNIGHT, BISHOP, QUEEN, KING
  };
  Piece(Piece::Color, Piece::Type, pos_x, pos_y, bool, Board*);
  Piece(Piece&, Board*);
  char* can_move(Move);
  void move(Move);
protected:
  Piece::Color color;
  Piece::Type type;
  int pos_x, pos_y;
  bool special;
  Board* b;
}

#endif
