%{
  #include <stdlib.h>
  #include <stdio.h>
  #include <string.h>
  #include "../headers/Node.h"

  extern int yylex(void);
  FILE *fp;   //AST
  FILE *fp2;  //(scope) - Stable

  int yyparse();
  FILE *yyin;
 
  void yyerror(const char *s);

%}

%union {
  int ival;
  char *sval;
}

%token <ival> RAWNUMBERDATA
%token <sval> EQUAL  
%token <sval> ASSIGN
%token <sval> BOOLEAN
%token <sval> RETURN
%token <sval> FOR
%token <sval> TO
%token <sval> DO
%token <sval> LBRACE
%token <sval> RBRACE
%token <sval> LPAREN
%token <sval> RPAREN
%token <sval> IF
%token <sval> COLON
%token <sval> COMMA
%token <sval> AND
%token <sval> OR
%token <sval> IMPLIES
%token <sval> ELSE
%toekn <sval> NULL
%token <sval> XOR
%token <sval> LE
%token <sval> GE
%token <sval> LT
%token <sval> GT
%token <sval> ID


%%
prog: cmd
      | functiondef
      ;

data: ID
	  | RAWNUMBERDATA
	  | BOOLEAN
	  | NULL
	  ;

functiondef: ID LPAREN  paramsoptional RPAREN LBRACE cmd RBRACE optionalreturn
            ;

paramsoptional: params 
			    | /* empty */
			    ;

params:	ID paramslist 
	    | tdsformat paramslist
	    ;

paramslist: paramslist COMMA ID
		    | paramslist COMMA tdsformat
		    | /* empty */
		    ;

optionalreturn: RETURN data
				| RETURN tdsformat
				| /* empty */
				;

cmd: expr
	 | FOR ID ASSIGN expr TO expr DO COMMA cmd
	 | functioncall
	 | cond
	 | ID ASSIGN LBRACE content RBRACE 
	 | ID ASSIGN data
	 ;

expr: arit 
	 | logical
	 | ineq
	 ;

logical: clause
		 | logical conec clause
		 ;

clause: BOOLEAN 
		| NOT BOOLEAN
		| ID
		| NOT ID
		;

conec: AND
	   | OR
	   | IMPLIES
	   | XOR
	   ;

ineq: expr ineqop expr
	  ;

ineqop: LT 
	  | LE 
	  | GT 
	  | GE 
	  | EQUAL
	  ;

functioncall: ID LPAREN paramsoptional RPAREN
			  ;

cond: IF LPAREN logical RPAREN LBRACE cmd RBRACE matchornot
	  ;

matchornot: ELSE LBRACE cmd RBRACE 
			| /* empty */
			;

content: content COMMA tdsformat 
		 | tdsformat
		 ;

tdsformat: LPAREN data COMMA RAWNUMBERDATA COMMA ID RPAREN
		   ;


%%


int main() {
  
  //Não esquecer, alterar a gramatica na produção PARAMS
  FILE *fp;

  fp = fopen("input", "r");
  if(!fp){
  	printf("CADE MEU CODIGO!?");
  	return -1;
  } 
  yyin = fp;
  while (yylex());

}


void yyerror(const char *s) {
  printf("Erro de parsing! %s",s);
  exit(-1);
}