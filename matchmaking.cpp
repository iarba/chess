#include <>

struct trie_node {
  trie_node()
  {
    sonst = NULL;
    sonsf = NULL;
    next = NULL;
    value = 0;
  }
  struct trie_node *sonst, *sonsf, *next;
  int value;
};

int main()
{

}
