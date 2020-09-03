#ifndef ENUM_H

#define ENUM_H

//typedef enum {false, true} bool;

typedef enum { GLOBAL, FUNC, LOOP, IF_BLOCK, ELSE_BLOCK} SCOPE_TYPE;

typedef enum { VARIABLE, TDS, T_DIRECTIVE} ENTRY_TYPE;

enum smvtype {MAIN = 1, AUTOMATA = 2, PORTS = 3};



#endif
