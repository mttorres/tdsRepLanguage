#ifdef NODE_H

#define NODE_H

typedef struct ast_node
{
  ast_node** children;
  char** leafs;
  char* name;

} Node;


void printNode(Node* n);

#endif