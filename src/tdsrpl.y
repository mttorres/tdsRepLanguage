%{

/*
*	inicilização de headers incluindo AST e Table
*/   

  #include <stdlib.h>
  #include <stdio.h>
  #include <string.h>
  #include "../headers/Enum.h"	
  #include "../headers/STable.h"
  #include "../headers/constants.h"
  #include "../headers/Node.h"	

  int yylex(void);
  FILE *fp1;   //AST
  FILE *fp2;  //(scope) - Stable

  int yyparse();
  FILE *yyin;
 
  void yyerror(const char *s);

  //typedef enum {false, true} BOOLEANO;

%}

/*
*	union para retornar os tipos desejados de dados para os tokens do lexer
	retornados como um yystype.  em que devemos definir os tipos por meio de uma union (pode assumir qualquer um dos valores dela)
*/ 



%union {
  int ival;
  float fval;
  char *sval;
  Node* ast;
  //BOOLEANO boolval
};


/*
  definição dos tokens relacionados aos tipos
*/

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
%token <sval> LBRACK
%token <sval> RBRACK
%token <sval> LPAREN
%token <sval> RPAREN
%token <sval> IF
%token <sval> COLON
%token <sval> COMMA
%token <sval> AND
%token <sval> OR
%token <sval> NOT
%token <sval> IMPLIES
%token <sval> ELSE
%token <sval> Null
%token <sval> XOR
%token <sval> LE
%token <sval> GE
%token <sval> LT
%token <sval> GT
%token <sval> PLUS
%token <sval> MINUS
%token <sval> TIMES
%token <sval> DIVIDE
%token <sval> ID
%token <sval> FUNCTION


%type <ast> prog
%type <ast> cmds cmd
%type <ast> functiondefs functiondef
%type <ast> paramsoptional paramslist params  


%left LOWER_THAN_ELSE
%left ELSE
%left MULT DIV MINUS PLUS LE GE GT LT EQ NE


%start prog


%%

/*
	Gramatica para o parser 
*/

prog: cmds functiondefs {
		
		Node * filhos[] = {
			 $1,
			 $2,
		};
		char* nome = "Prog -  comandos ou definições de função";
		int tamanhofilhos = sizeof(filhos);
		Node* prog = createNode(filhos, nome,NULL,tamanhofilhos,0);
		$$ = prog; 
	  }
      | /* empty */
      ; 


cmds: cmds cmd {
		
		Node * filhos[] = {
			 $1,
			 $2,
		};
		char* nome = "Cmds -  comando(s)";
		int tamanhofilhos = sizeof(filhos);
		Node* cmds = createNode(filhos, nome,NULL,tamanhofilhos,0);
		$$ = cmds; 

	  } 
	  | /* empty */
	  ;	

functiondefs: functiondefs functiondef {
		
		Node * filhos[] = {
			 $1,
			 $2,
		};
		char* nome = "Functiondefs -  definição(ões) de função ";
		int tamanhofilhos = sizeof(filhos);
		Node* functiondefs = createNode(filhos, nome,NULL,tamanhofilhos,0);
		$$ = functiondefs; 

	  }  
	  | /* empty */
	  ; 	


functiondef: FUNCTION ID LPAREN paramsoptional RPAREN LBRACE cmd RBRACE optionalreturn {

		Node * filhos[] = {
			 $4,
			 $7,
			 $9,
		};


		char * folhas[] = {
    		$1,
    		$2,
    		$3,
    		$5,
    		$6,
    		$8,

		};

		char* nome = "Functiondef -  definição de função ";

		int tamanhofilhos = sizeof(filhos);
		int tamanhofolhas = sizeof(folhas);

		Node* functiondef = createNode(filhos, nome,folhas,tamanhofilhos,tamanhofolhas);
		$$ = functiondef; 
}
;

paramsoptional: params {

		
		Node * filhos[] = {
			 $1,
		};
		
		char* nome = "Paramsoptional -  parametro(s) opcionais da definição de função ";
		int tamanhofilhos = sizeof(filhos);
		Node* paramsoptional = createNode(filhos, nome, NULL,tamanhofilhos,0);
		$$ = paramsoptional; 

		}
		| /* empty */
		;

params:	ID paramslist {

		Node * filhos[] = {
			 $1,
		};
		
		char* nome = "Params -  parametro(s)  da definição de função ";
		int tamanhofilhos = sizeof(filhos);
		Node* params = createNode(filhos, nome, NULL,tamanhofilhos,0);
		$$ = params; 
		}
	    | /* empty */
	    ;

paramslist: paramslist COMMA expr {

		Node * filhos[] = {
			 $1,
			 $3,
		};

		char * folhas[] = {
    		$2,
		};		
		
		char* nome = "Paramslist - lista de parâmetros a mais de uma função ";
		int tamanhofilhos = sizeof(filhos);
		int tamanhofolhas = sizeof(folhas);

		Node* paramslist = createNode(filhos, nome, folhas,tamanhofilhos,tamanhofolhas);
		$$ = paramslist; 
		}
	    | /* empty */
	    ;



optionalreturn: RETURN data {

		
		Node * filhos[] = {
			 $2,
		};

		char * folhas[] = {
    		$1,
		};			
		
		char* nome = "Optionalreturn -  retorno opcional ";

		int tamanhofilhos = sizeof(filhos);
		int tamanhofolhas = sizeof(folhas);

		Node* optionalreturn = createNode(filhos, nome, folhas,tamanhofilhos,tamanhofolhas);
		$$ = optionalreturn; 
		}
	    | /* empty */
	    ;

cmd: expr
	 | FOR ID ASSIGN expr TO expr DO COMMA cmd
	 | functioncall
	 | cond
	 | ID ASSIGN LBRACE content RBRACE 
	 | ID ASSIGN data
	 {printf("comando \n");}





data: ID
	  | RAWNUMBERDATA
	  | BOOLEAN
	  | Null
	  | functioncall
	  {printf("dados \n");}






expr: arit 
	 | logical
	 | ineq
	 {printf("expressao \n");}


arit: aritoperator
	  | arit aritoperation aritoperator
	  {printf("expressao ARITIMETICA \n");}

aritoperator: RAWNUMBERDATA
	  		  | ID
	  		  {printf("operarador ARITIMETICO \n");}

aritoperation: PLUS
			   | MINUS
			   | TIMES
			   | DIVIDE
			   {printf("operacao ARITIMETICO \n");}

logical: clause
		 | logical conec clause
		 {printf("exp logica  \n");}

clause: BOOLEAN 
		| NOT BOOLEAN
		| ID
		| NOT ID
		{printf(" clausula \n");}

conec: AND
	   | OR
	   | IMPLIES
	   | XOR
	   {printf("conector logico  \n");}

ineq: expr ineqop expr
	  {printf("inequacao \n");}

ineqop: LT 
	  | LE 
	  | GT 
	  | GE 
	  | EQUAL
	  {printf("inequacao operador  \n");}

functioncall: ID LPAREN paramsoptional RPAREN
			  {printf("chamada de função \n");}

cond: IF LPAREN logical RPAREN LBRACE cmd RBRACE matchornot
	  {printf("if \n");}

matchornot: ELSE LBRACE cmd RBRACE 
			| /* empty */
			{printf("else \n");}

content: content COMMA tdsformat 
		 | tdsformat
		 {printf("conjunto de tds's \n");}

tdsformat: LPAREN data COMMA expr COMMA ID RPAREN
		   {printf("tds \n");}


%%


int main(int argc, char* argv[]) {
  
  
  FILE *fp;

  fp = fopen(argv[1], "r");
  if(!fp){
  	printf("CADE MEU CODIGO!?");
  	return -1;
  } 
  yyin = fp;
  yyparse();
  fclose(fp);

}


void yyerror(const char *s) {
  printf("Erro de parsing! %s \n",s);
  exit(-1);
}