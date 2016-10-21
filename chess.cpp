#include "chess.hpp"
#include <cstring>

/* Board implementation */

Board::Board()
{
  int i;
  this -> pieces = new Piece*[MAX_CHESS_SIZE];
  for(i = 0; i < MAX_CHESS_SIZE; i++)
  {
    this -> pieces[i] = new Piece[MAX_CHESS_SIZE];
  }
  for(i = 0; i < MAX_CHESS_SIZE; i++)
  {
    pieces[1][i].set_piece(BLACK, PAWN, 0);
    pieces[6][i].set_piece(WHITE, PAWN, 0);
  }
  pieces[0][0].set_piece(BLACK, ROOK  , 0);
  pieces[0][1].set_piece(BLACK, KNIGHT, 0);
  pieces[0][2].set_piece(BLACK, BISHOP, 0);
  pieces[0][3].set_piece(BLACK, QUEEN , 0);
  pieces[0][4].set_piece(BLACK, KING  , 0);
  pieces[0][5].set_piece(BLACK, BISHOP, 0);
  pieces[0][6].set_piece(BLACK, KNIGHT, 0);
  pieces[0][7].set_piece(BLACK, ROOK  , 0);
  pieces[7][0].set_piece(WHITE, ROOK  , 0);
  pieces[7][1].set_piece(WHITE, KNIGHT, 0);
  pieces[7][2].set_piece(WHITE, BISHOP, 0);
  pieces[7][3].set_piece(WHITE, QUEEN , 0);
  pieces[7][4].set_piece(WHITE, KING  , 0);
  pieces[7][5].set_piece(WHITE, BISHOP, 0);
  pieces[7][6].set_piece(WHITE, KNIGHT, 0);
  pieces[7][7].set_piece(WHITE, ROOK  , 0);
  en_passant_row = -1;
}

Board::Board(Board& other)
{
  int i, j;
  this -> pieces = new Piece*[MAX_CHESS_SIZE];
  for(i = 0; i < MAX_CHESS_SIZE; i++)
  {
    this -> pieces[i] = new Piece[MAX_CHESS_SIZE];
  }
  for(i = 0; i < MAX_CHESS_SIZE; i++)
  {
    for(i = 0; i < MAX_CHESS_SIZE; i++)
    {
      pieces[i][j] = other.pieces[i][j];
    }
  }
  white_to_move = other.white_to_move;
  history = other.history;
  en_passant_row = other.en_passant_row;
}

Board::~Board()
{
  int i;
  for (i = 0; i < MAX_CHESS_SIZE; i++)
  {
    delete[] pieces[i];
  }
  delete[] pieces;
  history.clear();
}

int Board::can_move(Move m)
{
  int i, j;
  color c1 = pieces[m.px1][m.py1].color, c2 = pieces[m.px2][m.py2].color;
  type t1 = pieces[m.px1][m.py1].type, t2 = pieces[m.px2][m.py2].type;
  int s1 = pieces[m.px1][m.py1].special, s2 = pieces[m.px2][m.py2].special;
  if(m.special == 'O') // castling
  {
    if(t1 != ROOK)
    {
      return 100; // not a rook
    }
    if(t2.type != KING)
    {
      return 101; // not a king
    }
    if(s1)
    {
      return 102; // rook moved
    }
    if(s2)
    {
      return 103; // king moved
    }
    if(c1 != c2)
    {
      return 104; // different colors
    }
    if(m.py1 != m.py2)
    {
      return 105; // not on the same row
    }
    for(i = m.px1 + 1; i < m.px2; i++)
    {
      if(piece[i][m.py2].type != NOTHING)
      {
        return 106; // something in the way
      }
      if(is_threatened(i, m.py2) && (i != m.px1 + 1))
      {
        return 107; // position threatened
      }
    }
    for(i = m.px1 - 1; i > m.px2; i--)
    {
      if(piece[i][m.py2].type != NOTHING)
      {
        return 106; // something in the way
      }
      if(is_threatened(i, m.py2))
      {
        return 107; // position threatened
      }
    }
    if(is_threatened(m.py1, m.py2))
    {
      return 1; // in check
    }
  }
  if(m.special == 'E') // en passant
  {
// rework enpassant :(
    if(t1 != PAWN)
    {
      return 200; // not a pawn
    }
    if(t2 != PAWN)
    {
      return 201; // not a pawn
    }
    if(s2 != 1)
    {
      return 202; // pawn not in place, moved twice
    }
    if((m.px2 != 3) && (m.px2 != 4))
    {
      return 203; // pawn not in place, wrong position
    }
    if(c1 == c2)
    {
      return 204; // same colors
    }
    if(en_passant_row != m.py2)
    {
      return 205; // en passant expired
    }
    if(m.px1 )
    {
      return 206; // invalid attack pattern
    }
  }
  if(strchr("ROOK_L KNIGHT_L BISHOP_L QUEEN_L", m.special)) // promotion move
  {
    if((t2 != NOTHING) && (c1 == c2))
    {
      return 300; // same colors
    }
    if(t1 != PAWN)
    {
      return 301; // not a pawn
    }
  }
  return 0;
}

int Board::do_move(Move m)
{
  if(can_move(m) == 0)
  {
    en_passant_row == -1;
    history.push_back(m);
    Type t;
    Color c;
    int s;
    if(m.special == 'O') // castling
    {
      if(m.px1 == 0) // on queen's side
      {
        c = pieces[m.px1][m.py1].color;
        t = pieces[m.px1][m.py1].type;
        s = pieces[m.px1][m.py1].special;
        pieces[m.px2][m.py2 - 1].set_piece(c, t, s + 1); // rook movement
        t = pieces[m.px2][m.py2].type;
        s = pieces[m.px2][m.py2].special;
        pieces[m.px2][m.py2 - 2].set_piece(c, t, s + 1); // king movement
        pieces[m.px1][m.py1].set_type(NOTHING); // the old rook is removed
        pieces[m.px2][m.py2].set_type(NOTHING); // the old king is removed
      }
      else // on king's side
      {
        c = pieces[m.px1][m.py1].color;
        t = pieces[m.px1][m.py1].type;
        s = pieces[m.px1][m.py1].special;
        pieces[m.px2][m.py2 + 1].set_piece(c, t, s + 1); // rook movement
        t = pieces[m.px2][m.py2].type;
        s = pieces[m.px2][m.py2].special;
        pieces[m.px2][m.py2 + 2].set_piece(c, t, s + 1); // king movement
        pieces[m.px1][m.py1].set_type(NOTHING); // the old rook is removed
        pieces[m.px2][m.py2].set_type(NOTHING); // the old king is removed
      }
      return 0;
    }
    if(m.special == 'E') // en passant
    {
      c = pieces[m.px1][m.py1].color;
      t = pieces[m.px1][m.py1].type;
      s = pieces[m.px1][m.py1].special;
      pieces[m.px2][m.py2].set_piece(c, t, s + 1); // pawn movement
      pieces[m.px1][m.py1].set_type(NOTHING); // the old pawn is removed
      pieces[m.px2][m.py1].set_type(NOTHING); // the pawn is captured
      return 0;
    }
    if(m.special == 0) // normal move
    {
      c = pieces[m.px1][m.py1].color;
      t = pieces[m.px1][m.py1].type;
      if((t == PAWN) && (abs(m.px1 - m.px2) == 2))
      {
        en_passant_row = m.px1; // in the event it is actually required
      }
      s = pieces[m.px1][m.py1].special;
      pieces[m.px2][m.py2].set_piece(c, t, s + 1); // piece movement
      pieces[m.px1][m.py1].set_type(NOTHING); // the old piece is removed
      return 0;
    }
    if(m.special != 0) // promotion move
    {
      c = pieces[m.px1][m.py1].color;
      switch(m.special)
      {
        case ROOK_L   : t = ROOK;
                        break;
        case KNIGHT_L : t = KNIGHT;
                        break;
        case BISHOP_L : t = BISHOP;
                        break;
        case QUEEN_L  : t = QUEEN;
                        break;
        default       : return -1;
      }
      s = pieces[m.px1][m.py1].special;
      pieces[m.px2][m.py2].set_piece(c, t, s + 1); // piece movement
      pieces[m.px1][m.py1].set_type(NOTHING); // the old piece is removed
      return 0;
    }
  }
  return -1;
}

/* Piece implementation */

Piece::Piece()
{
  type = NOTHING;
}

void Piece::set_piece(Color c, Type t, int s)
{
  color = c;
  type = t;
  special = s;
}

void Piece::set_type(Type t)
{
  type = t;
}

/* Move implementation */

Move::Move(char px1, int py1, char px2, int py2, char special)
{
  this -> px1 = px1 - 'a';
  this -> py1 = py1 - 1;
  this -> px2 = px2 - 'a';
  this -> py2 = py2 - 1;
  this -> special = special;
}

/*
class Board
{
public:
  Board(Board& other);
  ~Board();
  int can_move(Move m);
  int do_move(Move m);
  bool white_to_move;
  int get_status();
  Piece** pieces;
  vector<Move> history;
}
*/


