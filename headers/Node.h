#ifndef NODE_H

#define NODE_H

typedef struct astnode
{
  struct astnode** children;
  char** leafs;
  char* name;
  int nleafs;
  int nchild;

}Node;


Node* createNode(int numArgs, ...);

void printNode(Node* n);

void letgoNode(Node* n);



#endif
