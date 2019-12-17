#ifdef TABLE_H

#define TABLE_H

typedef enum { GLOBAL, FUNC, FOR, IF, ELSE,  } SCOPETYPE;

typedef struct 
{
  TABLE* parent;
  TABLE** children;
  int order;
  SCOPETYPE type;
  // TABLE DATA (como representar!?) 	
} TABLE;



#endif