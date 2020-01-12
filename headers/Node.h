#define NODE_H

typedef struct astnode
{
  struct astnode** children;
  char** leafs;
  char* name;
  int nleafs;
  int nchild;

}Node;


Node* createNode(Node** children, char* name, char** leafs, int sizechildren, int sizeleafs);

void printNode(Node* n);

void letgoNode(Node *);




