#include "chess.hpp"
#include <cstring>
#include <cmath>

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
  en_passant_loc = -1;
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
  en_passant_loc = other.en_passant_loc;
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
  if((m.px1 < 0) || (m.py1 < 0) || (m.px1 < 0) || (m.py1 < 0))
  {
    return BASIC_INVALID_MOVE;
  }
  int i, j;
  j = MAX_CHESS_SIZE;
  if((m.px1 >= j) || (m.py1 >= j) || (m.px1 >= j) || (m.py1 >= j))
  {
    return BASIC_INVALID_MOVE;
  }
  color c1 = pieces[m.px1][m.py1].color;
  if((c1 == WHITE) && (! white_to_move))
  {
    return BASIC_WRONG_COLOR;
  }
  if((c1 == BLACK) && (white_to_move))
  {
    return BASIC_WRONG_COLOR;
  }
  color c2 = pieces[m.px2][m.py2].color;
  type t1 = pieces[m.px1][m.py1].type;
  if(t1 == NOTHING)
  {
    return BASIC_NO_PIECE;
  }
  type t2 = pieces[m.px2][m.py2].type;
  int m1 = pieces[m.px1][m.py1].moves;
  int m2 = pieces[m.px2][m.py2].moves;
  if(m.special == 'O') // castling
  {
    if(t1 != ROOK)
    {
      return CASTLING_NOT_ROOK;
    }
    if(t2.type != KING)
    {
      return CASTLING_NOT_KING;
    }
    if(m1)
    {
      return CASTLING_ROOK_MOVED;
    }
    if(m2)
    {
      return CASTLING_KING_MOVED;
    }
    if(c1 != c2)
    {
      return CASTLING_COLOR;
    }
    if(m.px1 != m.px2)
    {
      return CASTLING_NOT_SAME_ROW;
    }
    // the 2 fors checks for both king' and queen's side castling
    for(i = m.py1 + 1; i < m.py2; i++)
    {
      if(piece[m.px2][i].type != NOTHING)
      {
        return CASTLING_OCCUPIED;
      }
      if(is_threatened(m.px2, i) && (i != m.py1 + 1))
      {
        return CASTLING_CHECK;
      }
    }
    for(i = m.py1 - 1; i > m.py2; i--)
    {
      if(piece[m.px2][i].type != NOTHING)
      {
        return CASTLING_OCCUPIED;
      }
      if(is_threatened(m.px2, i))
      {
        return CASTLING_CHECK;
      }
    }
    if(is_threatened(m.px2, m.py2))
    {
      return CASTLING_CHECK;
    }
  }
  if(m.special == 'E') // en passant
  {
    color c2 = pieces[m.px1][m.py2].color;
    type t2 = pieces[m.px1][m.py2].type;
    int m2 = pieces[m.px1][m.py2].moves;
    if(t1 != PAWN)
    {
      return PASSANT_NOT_PAWN_O;
    }
    if(t2 != PAWN)
    {
      return PASSANT_NOT_PAWN_T;
    }
    if(m2 != 1)
    {
      return PASSANT_INVALID_MOVE;
    }
    if((m.px1 != 3) && (m.px1 != 4))
    {
      return PASSANT_INVALID_LOC;
    }
    if(c1 == c2)
    {
      return PASSANT_WRONG_COLOR;
    }
    if(en_passant_loc != m.py2)
    {
      return PASSANT_EXPIRED;
    }
    if(abs(m.py2 - m.py1) != 1)
    {
      return PASSANT_INVALID;
    }
    if(white_to_move)
    {
      if(m.px1 - m.px2 != 1)
      {
        return PASSANT_INVALID;
      }
    }
    else
    {
      if(m.px2 - m.px1 != 1)
      {
        return PASSANT_INVALID;
      }
    }
  }
  if(strchr("ROOK_L KNIGHT_L BISHOP_L QUEEN_L", m.special) && (m.special != 0))
  { // promotion move
    if((m.px2 != 0) && (m.px2 != 7))
    {
      return PROMOTE_NO;
    }
    if((t2 != NOTHING) && (c1 == c2))
    {
      return PROMOTE_COLOR;
    }
    if(t1 != PAWN)
    {
      return PROMOTE_PAWN;
    }
    if(abs(m.py2 - m.py1) > 1)
    {
      return PROMOTE_INVALID;
    }
    if((abs(m.py2 - m.py1) == 1) && (t2 == NOTHING))
    {
      return PROMOTE_NO_CAPTURE;
    }
    if((abs(m.py2 - m.py1) == 0) && (t2 != NOTHING))
    {
      return PROMOTE_BLOCKED;
    }
    if(white_to_move)
    {
      if(m.px1 - m.px2 != 1)
      {
        return PROMOTE_INVALID;
      }
    }
    else
    {
      if(m.px2 - m.px1 != 1)
      {
        return PROMOTE_INVALID;
      }
    }
  }
  if(m.special == 0) // normal move
  {
    if(t1 == PAWN) // pawn movement
    {
      if(abs(m.px2 - m.px1) == 2) // forward 2
      {
        if(s1)
        {
          return PAWN_FORWARD_EXPIRED;
        }
        if(m.py1 != m.py2)
        {
          return PAWN_INVALID;
        }
        if(white_to_move)
        {
          if(m.px1 - m.px2 != 2)
          {
            return PAWN_INVALID;
          }
          if(pieces[m.px1 - 1][m.py1].type != NOTHING)
          {
            return PAWN_BLOCKED;
          }
        }
        else
        {
          if(m.px2 - m.px1 != 2)
          {
            return PAWN_INVALID;
          }
          if(pieces[m.px1 + 1][m.py1].type != NOTHING)
          {
            return PAWN_BLOCKED;
          }
        }
        if(t2 != NOTHING)
        {
          return PAWN_BLOCKED;
        }
      }
      else // forward 1 or capture
      {
        if(white_to_move)
        {
          if(m.px1 - m.px2 != 1)
          {
            return PAWN_INVALID;
          }
          if(m.px2 == 0)
          {
            return PAWN_PROMOTION;
          }
        }
        else
        {
          if(m.px2 - m.px1 != 1)
          {
            return PAWN_INVALID;
          }
          if(m.px2 == 7)
          {
            return PAWN_PROMOTION;
          }
        }
        if((t2 != NOTHING) && (c1 == c2))
        {
          return PAWN_COLOR;
        }
        if(abs(m.py2 - m.py1) > 1)
        {
          return PAWN_INVALID;
        }
        if((abs(m.py2 - m.py1) == 1) && (t2 == NOTHING))
        {
          return PAWN_NO_CAPTURE;
        }
        if((abs(m.py2 - m.py1) == 0) && (t2 != NOTHING))
        {
          return PAWN_BLOCKED;
        }
      }
    }
    if(t1 == ROOK) // rook movement
    {
      if((m.px1 != m.px2) && (m.py1 != m.py2))
      {
        return ROOK_INVALID;
      }
      if((t2 != NOTHING) && (c1 != c2))
      {
        return ROOK_COLOR;
      }
      for(i = m.py1 + 1; i < m.py2; i++)
      {
        if(piece[m.px2][i].type != NOTHING)
        {
          return ROOK_BLOCKED;
        }
      }
      for(i = m.px1 + 1; i < m.px2; i++)
      {
        if(piece[i][py2].type != NOTHING)
        {
          return ROOK_BLOCKED;
        }
      }
    }
    if(t1 == KNIGHT) // knight movement
    {
      int dx = abs(m.px1 - m.px2);
      int dy = abs(m.py1 - m.py2);
      if(((dx != 1) || (dy != 2)) && ((dx != 2) || (dy != 1)))
      {
        return KNIGHT_INVALID;
      }
      if((t2 != NOTHING) && (c1 != c2))
      {
        return KNIGHT_COLOR;
      }
    if(t1 == BISHOP) // bishop movement
    {
      int dx = abs(m.px1 - m.px2);
      int dy = abs(m.py1 - m.py2);
      if(dx != dy)
      {
        return BISHOP_INVALID;
      }
      if((t2 != NOTHING) && (c1 != c2))
      {
        return BISHOP_COLOR;
      }
      int fx = 1, fy = 1;
      if(m.px2 < m.px1)
      {
        fx = -1;
      }
      if(m.py2 < m.py1)
      {
        fy = -1;
      }
      for(i = 1; i < dx; i++)
      {
        if(piece[m.px1 + fx * i][m.py1 + fy * i].type != NOTHING)
        {
          return BISHOP_BLOCKED;
        }
      }
    }
    if(t1 == QUEEN) // queen movement
    {
      if((t2 != NOTHING) && (c1 != c2))
      {
        return QUEEN_COLOR;
      }
      int dx = abs(m.px1 - m.px2);
      int dy = abs(m.py1 - m.py2);
      if((dx == 0) || (dy == 0))
      { // rook type movement
        for(i = m.py1 + 1; i < m.py2; i++)
        {
          if(piece[m.px2][i].type != NOTHING)
          {
            return QUEEN_BLOCKED;
          }
        }
        for(i = m.px1 + 1; i < m.px2; i++)
        {
          if(piece[i][py2].type != NOTHING)
          {
            return QUEEN_BLOCKED;
          }
        }
      }
      else
      { // bishop type movement
        if(dx != dy)
        {
          return QUEEN_INVALID;
        }
        int fx = 1, fy = 1;
        if(m.px2 < m.px1)
        {
          fx = -1;
        }
        if(m.py2 < m.py1)
        {
          fy = -1;
        }
        for(i = 1; i < dx; i++)
        {
          if(piece[m.px1 + fx * i][m.py1 + fy * i].type != NOTHING)
          {
            return QUEEN_BLOCKED;
          }
        }
      }
    }
    if(t1 == KING) // king movement
    {
      if((t2 != NOTHING) && (c1 != c2))
      {
        return KING_COLOR;
      }
      int dx = abs(m.px1 - m.px2);
      int dy = abs(m.py1 - m.py2);
      if((dx > 1) || (dy > 1))
      {
        return KING_INVALID;
      }
    }
  }
  // test the move to see if it puts you into check
  Board test_board(*this);
  if(test_board.do_move(m)
  {
    return BASIC_TERRIBLE;
  }
  int status = test_board.get_status();
  if(white_to_move && (status == 1))
  {
    return BASIC_CHECK;
  }
  if(! white_to_move && (status == 2))
  {
    return BASIC_CHECK;
  }
  if(status == 3)
  {
    return BASIC_CHECK;
  }
  return 0;
}

int Board::do_move(Move m)
{
  if(can_move(m) == 0)
  {
    en_passant_loc == -1;
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
        pieces[m.px2][m.py2 - 1].set_piece(c, t, m + 1); // rook movement
        t = pieces[m.px2][m.py2].type;
        s = pieces[m.px2][m.py2].special;
        pieces[m.px2][m.py2 - 2].set_piece(c, t, m + 1); // king movement
        pieces[m.px1][m.py1].set_type(NOTHING); // the old rook is removed
        pieces[m.px2][m.py2].set_type(NOTHING); // the old king is removed
      }
      else // on king's side
      {
        c = pieces[m.px1][m.py1].color;
        t = pieces[m.px1][m.py1].type;
        s = pieces[m.px1][m.py1].special;
        pieces[m.px2][m.py2 + 1].set_piece(c, t, m + 1); // rook movement
        t = pieces[m.px2][m.py2].type;
        s = pieces[m.px2][m.py2].special;
        pieces[m.px2][m.py2 + 2].set_piece(c, t, m + 1); // king movement
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
      pieces[m.px2][m.py2].set_piece(c, t, m + 1); // pawn movement
      pieces[m.px1][m.py1].set_type(NOTHING); // the old pawn is removed
      pieces[m.px1][m.py2].set_type(NOTHING); // the pawn is captured
      return 0;
    }
    if(m.special == 0) // normal move
    {
      c = pieces[m.px1][m.py1].color;
      t = pieces[m.px1][m.py1].type;
      if((t == PAWN) && (abs(m.px1 - m.px2) == 2))
      {
        en_passant_loc = m.py2; // in the event it is actually required
      }
      s = pieces[m.px1][m.py1].special;
      pieces[m.px2][m.py2].set_piece(c, t, m + 1); // piece movement
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
      pieces[m.px2][m.py2].set_piece(c, t, m + 1); // piece movement
      pieces[m.px1][m.py1].set_type(NOTHING); // the old piece is removed
      return 0;
    }
  }
  return -1;
}

int Board::get_status()
{
  int result = 0, i, j;
  for(i = 0; i < MAX_CHESS_SIZE; i++)
  {
    for(j = 0; j < MAX_CHESS_SIZE; j++)
    {
      if(piece[i][j].type == KING)
      {
        if(is_threatened(i, j))
        {
          result++;
          if(piece[i][j].color == BLACK)
          {
            result++;
          }
        }
      }
    }
  }
  return result;
}

bool Board::is_threatened(int px, int py)
{
  return false;
}

/* Piece implementation */

Piece::Piece()
{
  type = NOTHING;
}

void Piece::set_piece(Color c, Type t, int m)
{
  color = c;
  type = t;
  moves = m;
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


