#ifndef NODE_H

#define NODE_H

#include "Enum.h"


typedef struct astnode
{
  struct astnode** children;
  char** leafs;
  char* name;
  int nleafs;
  int nchild;
  NODE_TYPE type;
  

}Node;


Node* createNode(int numArgs, ...);

void printNode(Node* n);

void filePrintNode(Node* n, FILE* fp);

void letgoNode(Node* n);



#endif
