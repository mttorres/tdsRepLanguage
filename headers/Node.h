#ifdef NODE_H

#define NODE_H

typedef struct ast_node
{
  ast_node** children;
  char** leafs;
  char* name;

} Node;


Node create(Node* firstchild, char* name, char** leafs);

void printNode(Node* n);



#endif