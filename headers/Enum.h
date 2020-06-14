#ifndef ENUM_H

#define ENUM_H

//typedef enum {false, true} bool;

typedef enum { GLOBAL, FUNC, LOOP, IF_BLOCK, ELSE_BLOCK} SCOPE_TYPE;

typedef enum { VAR, TDS, T_DIRECTIVE} ENTRY_TYPE;

enum smvpart {MAIN = 1, PORTS = 2, AUTOMATA = 3} ;



#endif
