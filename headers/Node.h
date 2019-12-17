#ifdef NODE_H

#define NODE_H

typedef struct 
{
  Node* children;
  char** leafs; 	
} Node;


void printNode(Node* n);

#endif