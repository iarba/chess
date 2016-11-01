#ifndef netcode_header
#define netcode_header

data encode_move(Move m);

Move decode_move(data e);

data encode_int(int m);

int decode_int(data e);

data encode_char(string m);

string decode_char(data e);

data encode_checksum(int m);

int decode_checksum(data e);

/* Connection class:
 * Connection(char*, int) is for connecting to a server
 * Connection(int) is for hosting a server
 */

class Connection
{
public:
  Connection(char* destination, int port);
  Connection(int port);
  data read();
  void write(data);
};

#endif
