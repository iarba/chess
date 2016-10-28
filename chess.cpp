#include "chess.hpp"
#include "cmrv.hpp"
#include <cstring>
#include <cmath>
#include <cstdio>

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
  white_to_move = true;
  testing = 0;
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
    for(j = 0; j < MAX_CHESS_SIZE; j++)
    {
      pieces[i][j] = other.pieces[i][j];
    }
  }
  white_to_move = other.white_to_move;
  history = other.history;
  en_passant_loc = other.en_passant_loc;
  testing = other.testing;
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
  if(testing)
  {
    return 0;
  }
  if((m.px1 < 0) || (m.py1 < 0) || (m.px1 < 0) || (m.py1 < 0))
  {
    return BASIC_INVALID_MOVE;
  }
  int i, j;
  bool checked = false;
  j = MAX_CHESS_SIZE;
  if((m.px1 >= j) || (m.py1 >= j) || (m.px1 >= j) || (m.py1 >= j))
  {
    return BASIC_INVALID_MOVE;
  }
  Color c1 = pieces[m.px1][m.py1].color;
  if((c1 == WHITE) && (! white_to_move))
  {
    return BASIC_WRONG_COLOR;
  }
  if((c1 == BLACK) && (white_to_move))
  {
    return BASIC_WRONG_COLOR;
  }
  Color c2 = pieces[m.px2][m.py2].color;
  Type t1 = pieces[m.px1][m.py1].type;
  if(t1 == NOTHING)
  {
    return BASIC_NO_PIECE;
  }
  Type t2 = pieces[m.px2][m.py2].type;
  int m1 = pieces[m.px1][m.py1].moves;
  int m2 = pieces[m.px2][m.py2].moves;
  if(m.special == 'O') // castling
  {
    checked = true;
    if(t1 != ROOK)
    {
      return CASTLING_NOT_ROOK;
    }
    if(t2 != KING)
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
      if(pieces[m.px2][i].type != NOTHING)
      {
        return CASTLING_OCCUPIED;
      }
      if(is_threatened(m.px2, i, c1) && (i != m.py1 + 1))
      {
        return CASTLING_CHECK;
      }
    }
    for(i = m.py1 - 1; i > m.py2; i--)
    {
      if(pieces[m.px2][i].type != NOTHING)
      {
        return CASTLING_OCCUPIED;
      }
      if(is_threatened(m.px2, i, c1))
      {
        return CASTLING_CHECK;
      }
    }
    if(is_threatened(m.px2, m.py2, c1))
    {
      return CASTLING_CHECK;
    }
  }
  if(m.special == 'E') // en passant
  {
    checked = true;
    Color c2 = pieces[m.px1][m.py2].color;
    Type t2 = pieces[m.px1][m.py2].type;
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
  if(strchr("RNBQ", m.special) && (m.special != 0))
  { // promotion move
    checked = true;
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
    checked = true;
    if(t1 == PAWN) // pawn movement
    {
      if(abs(m.px2 - m.px1) == 2) // forward 2
      {
        if(m1)
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
      if((t2 != NOTHING) && (c1 == c2))
      {
        return ROOK_COLOR;
      }
        for(i = m.py1 + 1; i < m.py2; i++)
        {
          if(pieces[m.px2][i].type != NOTHING)
          {
            return ROOK_BLOCKED;
          }
        }
        for(i = m.py1 - 1; i > m.py2; i--)
        {
          if(pieces[m.px2][i].type != NOTHING)
          {
            return ROOK_BLOCKED;
          }
        }
        for(i = m.px1 + 1; i < m.px2; i++)
        {
          if(pieces[i][m.py2].type != NOTHING)
          {
            return ROOK_BLOCKED;
          }
        }
        for(i = m.px1 - 1; i > m.px2; i--)
        {
          if(pieces[i][m.py2].type != NOTHING)
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
      if((t2 != NOTHING) && (c1 == c2))
      {
        return KNIGHT_COLOR;
      }
    }
    if(t1 == BISHOP) // bishop movement
    {
      int dx = abs(m.px1 - m.px2);
      int dy = abs(m.py1 - m.py2);
      if(dx != dy)
      {
        return BISHOP_INVALID;
      }
      if((t2 != NOTHING) && (c1 == c2))
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
        if(pieces[m.px1 + fx * i][m.py1 + fy * i].type != NOTHING)
        {
          return BISHOP_BLOCKED;
        }
      }
    }
    if(t1 == QUEEN) // queen movement
    {
      if((t2 != NOTHING) && (c1 == c2))
      {
        return QUEEN_COLOR;
      }
      int dx = abs(m.px1 - m.px2);
      int dy = abs(m.py1 - m.py2);
      if((dx == 0) || (dy == 0))
      { // rook type movement
        for(i = m.py1 + 1; i < m.py2; i++)
        {
          if(pieces[m.px2][i].type != NOTHING)
          {
            return QUEEN_BLOCKED;
          }
        }
        for(i = m.py1 - 1; i > m.py2; i--)
        {
          if(pieces[m.px2][i].type != NOTHING)
          {
            return QUEEN_BLOCKED;
          }
        }
        for(i = m.px1 + 1; i < m.px2; i++)
        {
          if(pieces[i][m.py2].type != NOTHING)
          {
            return QUEEN_BLOCKED;
          }
        }
        for(i = m.px1 - 1; i > m.px2; i--)
        {
          if(pieces[i][m.py2].type != NOTHING)
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
          if(pieces[m.px1 + fx * i][m.py1 + fy * i].type != NOTHING)
          {
            return QUEEN_BLOCKED;
          }
        }
      }
    }
    if(t1 == KING) // king movement
    {
      if((t2 != NOTHING) && (c1 == c2))
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
  if(!checked)
  {
    return BASIC_SPECIAL;
  }
  // test the move to see if it puts you into check
  Board test_board(*this);
  test_board.testing = 1;
  if(test_board.do_move(m))
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
    en_passant_loc = -1;
    history.push_back(m);
    Type t0;
    Color c0;
    int m0;
    if(m.special == 'O') // castling
    {
      if(m.py1 == 0) // on queen's side
      {
        c0 = pieces[m.px1][m.py1].color;
        t0 = pieces[m.px1][m.py1].type;
        m0 = pieces[m.px1][m.py1].moves;
        pieces[m.px2][m.py2 - 1].set_piece(c0, t0, m0 + 1); // rook movement
        t0 = pieces[m.px2][m.py2].type;
        m0 = pieces[m.px2][m.py2].moves;
        pieces[m.px2][m.py2 - 2].set_piece(c0, t0, m0 + 1); // king movement
        pieces[m.px1][m.py1].set_type(NOTHING); // the old rook is removed
        pieces[m.px2][m.py2].set_type(NOTHING); // the old king is removed
      }
      else // on king's side
      {
        c0 = pieces[m.px1][m.py1].color;
        t0 = pieces[m.px1][m.py1].type;
        m0 = pieces[m.px1][m.py1].moves;
        pieces[m.px2][m.py2 + 1].set_piece(c0, t0, m0 + 1); // rook movement
        t0 = pieces[m.px2][m.py2].type;
        m0 = pieces[m.px2][m.py2].moves;
        pieces[m.px2][m.py2 + 2].set_piece(c0, t0, m0 + 1); // king movement
        pieces[m.px1][m.py1].set_type(NOTHING); // the old rook is removed
        pieces[m.px2][m.py2].set_type(NOTHING); // the old king is removed
      }
      white_to_move = ! white_to_move;
      return 0;
    }
    if(m.special == 'E') // en passant
    {
      c0 = pieces[m.px1][m.py1].color;
      t0 = pieces[m.px1][m.py1].type;
      m0 = pieces[m.px1][m.py1].moves;
      pieces[m.px2][m.py2].set_piece(c0, t0, m0 + 1); // pawn movement
      pieces[m.px1][m.py1].set_type(NOTHING); // the old pawn is removed
      pieces[m.px1][m.py2].set_type(NOTHING); // the pawn is captured
      white_to_move = ! white_to_move;
      return 0;
    }
    if(m.special == 0) // normal move
    {
      c0 = pieces[m.px1][m.py1].color;
      t0 = pieces[m.px1][m.py1].type;
      if((t0 == PAWN) && (abs(m.px1 - m.px2) == 2))
      {
        en_passant_loc = m.py2; // in the event it is actually required
      }
      m0 = pieces[m.px1][m.py1].moves;
      pieces[m.px2][m.py2].set_piece(c0, t0, m0 + 1); // piece movement
      pieces[m.px1][m.py1].set_type(NOTHING); // the old piece is removed
      white_to_move = ! white_to_move;
      return 0;
    }
    if(m.special != 0) // promotion move
    {
      c0 = pieces[m.px1][m.py1].color;
      switch(m.special)
      {
        case ROOK_L   : t0 = ROOK;
                        break;
        case KNIGHT_L : t0 = KNIGHT;
                        break;
        case BISHOP_L : t0 = BISHOP;
                        break;
        case QUEEN_L  : t0 = QUEEN;
                        break;
        default       : return -1;
      }
      m0 = pieces[m.px1][m.py1].moves;
      pieces[m.px2][m.py2].set_piece(c0, t0, m0 + 1); // piece movement
      pieces[m.px1][m.py1].set_type(NOTHING); // the old piece is removed
      white_to_move = ! white_to_move;
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
      if(pieces[i][j].type == KING)
      {
        if(is_threatened(i, j, pieces[i][j].color))
        {
          result++;
          if(pieces[i][j].color == BLACK)
          {
            result++;
          }
        }
      }
    }
  }
  return result;
}

bool Board::is_threatened(int px, int py, Color c)
{
  if(c == WHITE) // black pawns
  {
    px--; py--;
    if(!out_of_board(px, py))
    {
      if((pieces[px][py].color == BLACK) && (pieces[px][py].type == PAWN))
      {
        return true;
      }
    }
    py += 2;
    if(!out_of_board(px, py))
    {
      if((pieces[px][py].color == BLACK) && (pieces[px][py].type == PAWN))
      {
        return true;
      }
    }
    px++; py--;
  }
  else // white pawns
  {
    px++; py--;
    if(!out_of_board(px, py))
    {
      if((pieces[px][py].color == WHITE) && (pieces[px][py].type == PAWN))
      {
        return true;
      }
    }
    py += 2;
    if(!out_of_board(px, py))
    {
      if((pieces[px][py].color == WHITE) && (pieces[px][py].type == PAWN))
      {
        return true;
      }
    }
  }
  bool chkdir[4];
  int i;
  Type t0;
  Color c0;
  // rooks or queens
  chkdir[0] = true;
  chkdir[1] = true;
  chkdir[2] = true;
  chkdir[3] = true;
  for(i = 1; i < MAX_CHESS_SIZE; i++)
  {
    if((chkdir[0]) && (!out_of_board(px - i, py)))
    {
      t0 = pieces[px - i][py].type;
      c0 = pieces[px - i][py].color;
      if(((t0 == ROOK) || (t0 == QUEEN)) && (c0 != c))
      {
        return true;
      }
      if(t0 != NOTHING)
      {
        chkdir[0] = false;
      }
    }
    if((chkdir[1]) && (!out_of_board(px + i, py)))
    {
      t0 = pieces[px + i][py].type;
      c0 = pieces[px + i][py].color;
      if(((t0 == ROOK) || (t0 == QUEEN)) && (c0 != c))
      {
        return true;
      }
      if(t0 != NOTHING)
      {
        chkdir[1] = false;
      }
    } 
    if((chkdir[2]) && (!out_of_board(px, py - i)))
    {
      t0 = pieces[px][py - i].type;
      c0 = pieces[px][py - i].color;
      if(((t0 == ROOK) || (t0 == QUEEN)) && (c0 != c))
      {
        return true;
      }
      if(t0 != NOTHING)
      {
        chkdir[2] = false;
      }
    }
    if((chkdir[3]) && (!out_of_board(px, py + i)))
    {
      t0 = pieces[px][py + i].type;
      c0 = pieces[px][py + i].color;
      if(((t0 == ROOK) || (t0 == QUEEN)) && (c0 != c))
      {
        return true;
      }
      if(t0 != NOTHING)
      {
        chkdir[3] = false;
      }
    }
  }
  // bishops or queens
  chkdir[0] = true;
  chkdir[1] = true;
  chkdir[2] = true;
  chkdir[3] = true;
  for(i = 1; i < MAX_CHESS_SIZE; i++)
  {
    if((chkdir[0]) && (!out_of_board(px - i, py - i)))
    {
      t0 = pieces[px - i][py - i].type;
      c0 = pieces[px - i][py - i].color;
      if(((t0 == BISHOP) || (t0 == QUEEN)) && (c0 != c))
      {
        return true;
      }
      if(t0 != NOTHING)
      {
        chkdir[0] = false;
      }
    }
    if((chkdir[1]) && (!out_of_board(px - i, py + i)))
    {
      t0 = pieces[px - i][py + i].type;
      c0 = pieces[px - i][py + i].color;
      if(((t0 == BISHOP) || (t0 == QUEEN)) && (c0 != c))
      {
        return true;
      }
      if(t0 != NOTHING)
      {
        chkdir[1] = false;
      }
    }
    if((chkdir[2]) && (!out_of_board(px + i, py - i)))
    {
      t0 = pieces[px + i][py - i].type;
      c0 = pieces[px + i][py - i].color;
      if(((t0 == BISHOP) || (t0 == QUEEN)) && (c0 != c))
      {
        return true;
      }
      if(t0 != NOTHING)
      {
        chkdir[2] = false;
      }
    }
    if((chkdir[3]) && (!out_of_board(px + i, py + i)))
    {
      t0 = pieces[px + i][py + i].type;
      c0 = pieces[px + i][py + i].color;
      if(((t0 == BISHOP) || (t0 == QUEEN)) && (c0 != c))
      {
        return true;
      }
      if(t0 != NOTHING)
      {
        chkdir[3] = false;
      }
    }
  }
  // knights
  if(!out_of_board(px + 1, py + 2))
  {
    t0 = pieces[px + 1][py + 2].type;
    c0 = pieces[px + 1][py + 2].color;
    if((t0 == KNIGHT) && (c0 != c))
    {
      return true;
    }
  }
  if(!out_of_board(px + 1, py - 2))
  {
    t0 = pieces[px + 1][py - 2].type;
    c0 = pieces[px + 1][py - 2].color;
    if((t0 == KNIGHT) && (c0 != c))
    {
      return true;
    }
  }
  if(!out_of_board(px - 1, py + 2))
  {
    t0 = pieces[px - 1][py + 2].type;
    c0 = pieces[px - 1][py + 2].color;
    if((t0 == KNIGHT) && (c0 != c))
    {
      return true;
    }
  }
  if(!out_of_board(px - 1, py - 2))
  {
    t0 = pieces[px - 1][py - 2].type;
    c0 = pieces[px - 1][py - 2].color;
    if((t0 == KNIGHT) && (c0 != c))
    {
      return true;
    }
  }
  if(!out_of_board(px + 2, py + 1))
  {
    t0 = pieces[px + 2][py + 1].type;
    c0 = pieces[px + 2][py + 1].color;
    if((t0 == KNIGHT) && (c0 != c))
    {
      return true;
    }
  }
  if(!out_of_board(px + 2, py - 1))
  {
    t0 = pieces[px + 2][py - 1].type;
    c0 = pieces[px + 2][py - 1].color;
    if((t0 == KNIGHT) && (c0 != c))
    {
      return true;
    }
  }
  if(!out_of_board(px - 2, py + 1))
  {
    t0 = pieces[px - 2][py + 1].type;
    c0 = pieces[px - 2][py + 1].color;
    if((t0 == KNIGHT) && (c0 != c))
    {
      return true;
    }
  }
  if(!out_of_board(px - 2, py - 1))
  {
    t0 = pieces[px - 2][py - 1].type;
    c0 = pieces[px - 2][py - 1].color;
    if((t0 == KNIGHT) && (c0 != c))
    {
      return true;
    }
  }
  // enemy king
  int j;
  for(i = -1; i <= 1; i++)
  {
    for(j = -1; j <= 1; j++)
    {
      if((i == 0) && (j == 0))
      {
        continue;
      }
      if(!out_of_board(px + i, py + j))
      {
        t0 = pieces[px + i][py + i].type;
        c0 = pieces[px + i][py + i].color;
        if((t0 == KING) && (c0 != c))
        {
          return true;
        }
      }
    }
  }
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

/* Misc implementation */

bool out_of_board(int px, int py)
{
  return ((px < 0) || (py < 0) || 
          (px >= MAX_CHESS_SIZE) || (py >= MAX_CHESS_SIZE));
}

