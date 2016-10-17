/*



*/

/* Chess_game implementation */
/*
class Chess_game
{
public:
  Chess_game();
protected:
  Board b;
}
*/

Chess_game::Chess_game()
{
  this->b = new Board();
}

/* Board implementation */
/*
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
*/

Board::Board()
{
  this->white_to_move = true;
  int i, j;
  for(i = 0; i < MAX_CHESS_SIZE; i++)
  {
    this->pieces[1][i] = new Piece(0, 0, 1, i, false, this);
    this->pieces[6][i] = new Piece(1, 0, 6, i, false, this);
  }
  for(i = 0; i <= 2; i++)
  {
    this->pieces[0][i] = new Piece(0, i + 1, 0, i + 1, false, this);
    this->pieces[0][MAX_CHESS_SIZE - i - 1] = new Piece(0, i + 1, 0, MAX_CHESS_SIZE - i - 1, false, this);
    this->pieces[7][i] = new Piece(1, i + 1, 0, i + 1, false, this);
    this->pieces[7][MAX_CHESS_SIZE - i - 1] = new Piece(1, i + 1, 0, MAX_CHESS_SIZE - i - 1, false, this);
  }
  this->pieces[0][3] = new Piece(0, 4, 0, 4, false, this);
  this->pieces[0][4] = new Piece(0, 5, 0, 5, false, this);
  this->pieces[7][3] = new Piece(1, 4, 1, 4, false, this);
  this->pieces[7][4] = new Piece(1, 5, 1, 5, false, this);
}

Board::Board(Board& other)
{
  this->white_to_move = other->white_to_move;
  int i, j;
  for(i = 0; i < MAX_CHESS_SIZE; i++)
  {
    for(j = 0; j < Max_Chess_size; j++)
    {
      this->pieces[i][j] = Piece(other->pieces[i][j], this);
    }
  }
}

Piece Board::get_piece(char i, int j)
{
  i = toUpper(i)-'A';
  return this->pieces[i][j];
}

/* Piece implementation */
/*
class Piece
{
public:
  Piece(Piece::Color, Piece::Type, pos_x, pos_y, bool, Board*);
  char* can_move(Move);
  void move(Move);
protected:
  Piece::Color color;
  Piece::Type type;
  int pos_x, pos_y;
  bool special;
  Board* b;
}
*/

