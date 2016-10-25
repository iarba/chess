#include "chess.hpp"
#include <iostream>
#include <cstdio>

using namespace std;

void print_board(Board b)
{
  int i, j;
  cout << "-+-1++-2++-3++-4++-5++-6++-7++-8+" << endl;
  cout << "-+--++--++--++--++--++--++--++--+" << endl;
  for(i = 0; i < 8; i++)
  {
    cout<< (char) ('A' + i);
    for(j = 0; j < 8; j++)
    {
      Type t = b.pieces[i][j].type;
      Color c = b.pieces[i][j].color;
      if(t == NOTHING)
      {
        cout<<"|  |";
      }
      if(t == PAWN)
      {
        cout << "|P";
        if(c == WHITE)
        {
          cout << "W|";
        }
        else
        {
          cout << "B|";
        }
      }
      if(t == ROOK)
      {
        cout << "|R";;
        if(c == WHITE)
        {
          cout << "W|";
        }
        else
        {
          cout << "B|";
        }
      }
      if(t == KNIGHT)
      {
        cout << "|N";
        if(c == WHITE)
        {
          cout << "W|";
        }
        else
        {
          cout << "B|";
        }
      }
      if(t == BISHOP)
      {
        cout << "|B";
        if(c == WHITE)
        {
          cout << "W|";
        }
        else
        {
          cout << "B|";
        }
      }
      if(t == QUEEN)
      {
        cout << "|Q";
        if(c == WHITE)
        {
          cout << "W|";
        }
        else
        {
          cout << "B|";
        }
      }
      if(t == KING)
      {
        cout << "|K";
        if(c == WHITE)
        {
          cout << "W|";
        }
        else
        {
          cout << "B|";
        }
      }
    }
    cout << endl;
    cout << "-+--++--++--++--++--++--++--++--+" << endl;
  }
}

int main()
{
  Board b;
  while(1)
  {
    print_board(b);
    int y1, y2;
    char x1, x2, s;
    scanf("%c %d %c %d %c", &x1, &y1, &x2, &y2, &s);
    cout << x1 << y1 << x2 << y2 << s << endl;
    if(s == '0')
    {
      s = 0;
    }
    Move m(x1, y1, x2, y2, s);
    int c = b.can_move(m);
    if(c)
    {
      cout << c << endl;
      continue;
    }
    else
    {
      b.do_move(m);
    }
  }
  return 0;
}
