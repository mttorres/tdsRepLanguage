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


  Node* root;
 
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

%token <sval> RAWNUMBERDATA
%token <sval> EQUAL
%token <sval> NOTEQUAL    
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
%token <sval> POINT
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
%token <sval> LABEL

%type <ast> prog
%type <ast> cmds cmd
%type <ast> functiondefs functiondef
%type <ast> paramsoptional paramslist params  
%type <ast> optionalreturn
%type <ast> data
%type <ast> condstmt
%type <ast> otherstmt
%type <ast> assignment
%type <ast> expr
%type <ast> multiexp
%type <ast> ineqexp
%type <ast> logical
%type <ast> variabledata
%type <ast> content
%type <ast> variableprop
%type <ast> functioncall
%type <ast> param
%type <ast> paramsoptionalCall paramslistCall paramsCall  
%type <ast> tdsformat
%type <ast> matchornot


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
		printf("PROGRAMA \n");
		$$ = prog; 
		root = prog;
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
		printf("comandos \n");
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


functiondef: FUNCTION ID LPAREN paramsoptional RPAREN LBRACE cmds RBRACE optionalreturn {

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

		
		char * folhas[] = {
    		$1,
    		
		};


		Node * filhos[] = {
			 $2,
		};
		
		char* nome = "Params -  parametro(s)  da definição de função ";

		int tamanhofilhos = sizeof(filhos);
		int tamanhofolhas = sizeof(folhas);
		
		Node* params = createNode(filhos, nome, folhas,tamanhofilhos,tamanhofolhas);
		
		$$ = params; 
		}
	    | /* empty */
	    ;

paramslist: paramslist COMMA ID {

		Node * filhos[] = {
			 $1,	 
		};

		char * folhas[] = {
    		$2,
    		$3,
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


cmd: condstmt {
	
		Node * filhos[] = {
			 $1,
		};
		
		char* nome = "CMD -  Comando (statement) ";

		int tamanhofilhos = sizeof(filhos);
	

		Node* cmd = createNode(filhos, nome, NULL,tamanhofilhos,0);
		printf("comando \n");
		$$ = cmd; 
	
	  
	  }
	 | otherstmt {

		Node * filhos[] = {
			 $1,
		};

		char* nome = "CMD -  Comando (statement) ";

		int tamanhofilhos = sizeof(filhos);
	

		Node* cmd = createNode(filhos, nome, NULL,tamanhofilhos,0);
		printf("comando \n");
		$$ = cmd; 		


	 }
	 ;


otherstmt: FOR expr TO expr DO COMMA cmds {
	
		Node * filhos[] = {
			 $2,
			 $4,
			 $7,
		};

		char * folhas[] = {
    		$1,
    		$3,
			$5,
			$6,
		};			
		
		char* nome = "Otherstmt(for) -  comandos diferente de if e else ";

		int tamanhofilhos = sizeof(filhos);
		int tamanhofolhas = sizeof(folhas);

		Node* otherstmt = createNode(filhos, nome, folhas,tamanhofilhos,tamanhofolhas);
		$$ = otherstmt; 
		


	 }
	 | functioncall {

		Node * filhos[] = {
			 $1,
		};

		char* nome = "Otherstmt(functioncall) -  comandos diferente de if e else ";

		int tamanhofilhos = sizeof(filhos);


		Node* otherstmt = createNode(filhos, nome, NULL,tamanhofilhos,0);
		$$ = otherstmt;

	 }
	 | assignment {

		Node * filhos[] = {
			 $1,
		};

		char* nome = "Otherstmt(assignment) -  comandos diferente de if e else ";

		int tamanhofilhos = sizeof(filhos);


		Node* otherstmt = createNode(filhos, nome, NULL,tamanhofilhos,0);
		$$ = otherstmt;

	 }
	 ;


assignment: ID ASSIGN expr {
		
		Node * filhos[] = {
			 $3,
		};

		char * folhas[] = {
    		$1,
    		$2,
		};			
		
		char* nome = "Assignment(simples) -  atribuição de variavel  ";

		int tamanhofilhos = sizeof(filhos);
		int tamanhofolhas = sizeof(folhas);

		Node* assignment = createNode(filhos, nome, folhas,tamanhofilhos,tamanhofolhas);
		printf("atribuicao \n");
		$$ = assignment; 


	 }
	 | ID LBRACK expr RBRACK ASSIGN expr {

		Node * filhos[] = {
			 $3,
			 $6,
		};

		char * folhas[] = {
    		$1,
    		$2,
    		$4,
    		$5,
		};			
		
		char* nome = "Assignment(estrutura/conjunto de dados) -  atribuição de ED/conjunto de dados  ";

		int tamanhofilhos = sizeof(filhos);
		int tamanhofolhas = sizeof(folhas);

		Node* assignment = createNode(filhos, nome, folhas,tamanhofilhos,tamanhofolhas);
		$$ = assignment; 


	 }
	 | ID POINT ID ASSIGN expr {

		Node * filhos[] = {
			 $5,
		};

		char * folhas[] = {
    		$1,
    		$2,
    		$3,
    		$4,
		};			
		
		char* nome = "Assignment(propriedade de variavel/objeto) -  atribuição a uma propriedade  ";

		int tamanhofilhos = sizeof(filhos);
		int tamanhofolhas = sizeof(folhas);

		Node* assignment = createNode(filhos, nome, folhas,tamanhofilhos,tamanhofolhas);
		$$ = assignment;
	 }
	 ; 




expr: MINUS expr {
	
		Node * filhos[] = {
			 $2,
		};

		char * folhas[] = {
    		$1,

		};			
		
		char* nome = "Expressão Básica - negativo";

		int tamanhofilhos = sizeof(filhos);
		int tamanhofolhas = sizeof(folhas);

		Node* expr = createNode(filhos, nome, folhas,tamanhofilhos,tamanhofolhas);
		$$ = expr;	


	 }
	 | expr MINUS expr {

		Node * filhos[] = {
			 $1,
			 $3,
		};

		char * folhas[] = {
    		$2,

		};			
		
		char* nome = "Expressão Básica - subtração ";

		int tamanhofilhos = sizeof(filhos);
		int tamanhofolhas = sizeof(folhas);

		Node* expr = createNode(filhos, nome, folhas,tamanhofilhos,tamanhofolhas);
		$$ = expr;		 




	 }
	 | expr PLUS expr {

		Node * filhos[] = {
			 $1,
			 $3,
		};

		char * folhas[] = {
    		$2,

		};			
		
		char* nome = "Expressão Básica - soma ";

		int tamanhofilhos = sizeof(filhos);
		int tamanhofolhas = sizeof(folhas);

		Node* expr = createNode(filhos, nome, folhas,tamanhofilhos,tamanhofolhas);
		$$ = expr;	


	 }
	 | multiexp {

		Node * filhos[] = {
			 $1,
		};

		
		char* nome = "Expressão Básica ";

		int tamanhofilhos = sizeof(filhos);
		

		Node* expr = createNode(filhos, nome, NULL, tamanhofilhos,0);
		$$ = expr;	

	 }
	 ;


multiexp: multiexp TIMES multiexp {
	
			Node * filhos[] = {
			 	$1,
				 $3,
			};

			char * folhas[] = {
    			$2,

			};			
		
			char* nome = "Expressão Básica - Multiplicação ";

			int tamanhofilhos = sizeof(filhos);
			int tamanhofolhas = sizeof(folhas);

			Node* multiexp = createNode(filhos, nome, folhas,tamanhofilhos,tamanhofolhas);
			$$ = multiexp;	


		}
		|multiexp DIVIDE multiexp {

			Node * filhos[] = {
				 $1,
				 $3,
			};

			char * folhas[] = {
    			$2,

			};			
		
			char* nome = "Expressão Básica - Divisão ";

			int tamanhofilhos = sizeof(filhos);
			int tamanhofolhas = sizeof(folhas);

			Node* multiexp = createNode(filhos, nome, folhas,tamanhofilhos,tamanhofolhas);
			$$ = multiexp;	

		}
		|ineqexp {

			Node * filhos[] = {
				 $1,
			};

		
			char* nome = "Expressão Básica - inequações/lógicas ";

			int tamanhofilhos = sizeof(filhos);
		

			Node* multiexp = createNode(filhos, nome, NULL, tamanhofilhos,0);
			$$ = multiexp;				

		}
		;

ineqexp:  ineqexp LE ineqexp {
	
			Node * filhos[] = {
			 	$1,
				 $3,
			};

			char * folhas[] = {
    			$2,

			};			
		
			char* nome = "Inequação  - Menor igual a ";

			int tamanhofilhos = sizeof(filhos);
			int tamanhofolhas = sizeof(folhas);

			Node* ineqexp = createNode(filhos, nome, folhas,tamanhofilhos,tamanhofolhas);
			$$ = ineqexp;	



		}
		| ineqexp GE ineqexp {

			Node * filhos[] = {
			 	$1,
				 $3,
			};

			char * folhas[] = {
    			$2,

			};			
		
			char* nome = "Inequação  - Maior igual a ";

			int tamanhofilhos = sizeof(filhos);
			int tamanhofolhas = sizeof(folhas);

			Node* ineqexp = createNode(filhos, nome, folhas,tamanhofilhos,tamanhofolhas);
			$$ = ineqexp;	



		}
		| ineqexp LT ineqexp {

			Node * filhos[] = {
			 	$1,
				$3,
			};

			char * folhas[] = {
    			$2,

			};			
		
			char* nome = "Inequação  - Menor que ";

			int tamanhofilhos = sizeof(filhos);
			int tamanhofolhas = sizeof(folhas);

			Node* ineqexp = createNode(filhos, nome, folhas,tamanhofilhos,tamanhofolhas);
			$$ = ineqexp;	



		}
		| ineqexp GT ineqexp {

			Node * filhos[] = {
			 	$1,
				 $3,
			};

			char * folhas[] = {
    			$2,

			};			
		
			char* nome = "Inequação  - Maior que ";

			int tamanhofilhos = sizeof(filhos);
			int tamanhofolhas = sizeof(folhas);

			Node* ineqexp = createNode(filhos, nome, folhas,tamanhofilhos,tamanhofolhas);
			$$ = ineqexp;	



		}
        | ineqexp EQUAL ineqexp {

			Node * filhos[] = {
			 	$1,
				 $3,
			};

			char * folhas[] = {
    			$2,

			};			
		
			char* nome = "Inequação  - Igual a ";

			int tamanhofilhos = sizeof(filhos);
			int tamanhofolhas = sizeof(folhas);

			Node* ineqexp = createNode(filhos, nome, folhas,tamanhofilhos,tamanhofolhas);
			$$ = ineqexp;	



        }
        | ineqexp NOTEQUAL ineqexp {

			Node * filhos[] = {
			 	$1,
				 $3,
			};

			char * folhas[] = {
    			$2,

			};			
		
			char* nome = "Inequação  - Diferente de ";

			int tamanhofilhos = sizeof(filhos);
			int tamanhofolhas = sizeof(folhas);

			Node* ineqexp = createNode(filhos, nome, folhas,tamanhofilhos,tamanhofolhas);
			$$ = ineqexp;	


        }
        | logical {

			Node * filhos[] = {
			 	$1,
				
			};

			char* nome = "Lógico  ";

			int tamanhofilhos = sizeof(filhos);


			Node* ineqexp = createNode(filhos, nome, NULL,tamanhofilhos,0);
			$$ = ineqexp;	

        }
        ;


logical: NOT logical {
	
			Node * filhos[] = {
			 	$2,
			};

			char * folhas[] = {
    			$1,

			};			
		
			char* nome = "Lógico  - Negação ";

			int tamanhofilhos = sizeof(filhos);
			int tamanhofolhas = sizeof(folhas);

			Node* logical = createNode(filhos, nome, folhas,tamanhofilhos,tamanhofolhas);
			$$ = logical;	

		 }
		 | logical AND logical {

			Node * filhos[] = {
			 	$1,
				$3,
			};

			char * folhas[] = {
    			$2,

			};			
		
			char* nome = "Lógico  - AND ";

			int tamanhofilhos = sizeof(filhos);
			int tamanhofolhas = sizeof(folhas);

			Node* logical = createNode(filhos, nome, folhas,tamanhofilhos,tamanhofolhas);
			$$ = logical;	

		 }
		 | logical OR logical {

			Node * filhos[] = {
			 	$1,
				$3,
			};

			char * folhas[] = {
    			$2,

			};			
		
			char* nome = "Lógico  - OR ";

			int tamanhofilhos = sizeof(filhos);
			int tamanhofolhas = sizeof(folhas);

			Node* logical = createNode(filhos, nome, folhas,tamanhofilhos,tamanhofolhas);
			$$ = logical;	

		 }
		 | logical IMPLIES logical {

			Node * filhos[] = {
			 	$1,
				$3,
			};

			char * folhas[] = {
    			$2,

			};			
		
			char* nome = "Lógico  - IMPLICAÇÃO ";

			int tamanhofilhos = sizeof(filhos);
			int tamanhofolhas = sizeof(folhas);

			Node* logical = createNode(filhos, nome, folhas,tamanhofilhos,tamanhofolhas);
			$$ = logical;	

		 }
		 | data {

			Node * filhos[] = {
			 	$1,
				
			};

			char* nome = "Dados do componente de uma expressão  ";

			int tamanhofilhos = sizeof(filhos);


			Node* ineqexp = createNode(filhos, nome, NULL,tamanhofilhos,0);
			$$ = ineqexp;	




		 }
		 ;
		 
data: RAWNUMBERDATA {
	
			char * folhas[] = {
			 	$1,
				
			};

			char* nome = "Número";

			int tamanhofolhas = sizeof(folhas);


			Node* data = createNode(NULL, nome, folhas,0,tamanhofolhas);
			$$ = data;	



	  }
	  | BOOLEAN {

			char * folhas[] = {
			 	$1,
				
			};

			char* nome = "Booleano";

			int tamanhofolhas = sizeof(folhas);


			Node* data = createNode(NULL, nome, folhas,0,tamanhofolhas);
			$$ = data;	



	  }
	  | Null {

			char * folhas[] = {
			 	$1,
				
			};

			char* nome = "NULL";

			int tamanhofolhas = sizeof(folhas);


			Node* data = createNode(NULL, nome, folhas,0,tamanhofolhas);
			$$ = data;	


	  }
	  | functioncall {

			Node * filhos[] = {
			 	$1,
				
			};

			char* nome = "Dados de chamada de função ";

			int tamanhofilhos = sizeof(filhos);


			Node* data = createNode(filhos, nome, NULL ,tamanhofilhos,0);
			$$ = data;	


	  }
	  | variabledata {

			Node * filhos[] = {
			 	$1,
				
			};

			char* nome = "Dados de outras variaveis ou estruturas";

			int tamanhofilhos = sizeof(filhos);


			Node* data = createNode(filhos, nome, NULL ,tamanhofilhos,0);
			$$ = data;	


	  }
	  ;


variabledata: ID {
	
			char * folhas[] = {
			 	$1,
				
			};

			char* nome = "Variavel";

			int tamanhofolhas = sizeof(folhas);


			Node* data = createNode(NULL, nome, folhas,0,tamanhofolhas);
			$$ = data;	


		}
			| LBRACE content RBRACE {

			char * folhas[] = {
			 	$1,
			 	$3,
				
			};

			Node * filhos[] = {
			 	$2,
				
			};

			char* nome = "Dados de conteudos/conjuntos ";

			int tamanhofilhos = sizeof(filhos);
			int tamanhofolhas = sizeof(folhas);

			Node* variabledata = createNode(filhos, nome, folhas ,tamanhofilhos,tamanhofolhas);
			printf("TDS FORMAT! \n");
			$$ = variabledata;	


			  }
		| variableprop {

			Node * filhos[] = {
			 	$1,
				
			};

			char* nome = "Dados de propriedades de um objeto/variavel ";

			int tamanhofilhos = sizeof(filhos);


			Node* variabledata = createNode(filhos, nome, NULL ,tamanhofilhos,0);
			$$ = variabledata;	



			}
		| LABEL {
			
			char * folhas[] = {
			 	$1,
				
			};

			char* nome = "LABEL";

			int tamanhofolhas = sizeof(folhas);


			Node* data = createNode(NULL, nome, folhas,0,tamanhofolhas);
			printf("LABEL \n");
			$$ = data;					
		
		}
		;


variableprop: ID POINT ID {
	

			char * folhas[] = {
			 	$1,
			 	$2,
			 	$3,
				
			};

			char* nome = "Propriedade de variavel";

			int tamanhofolhas = sizeof(folhas);


			Node* variableprop = createNode(NULL, nome, folhas,0,tamanhofolhas);
			$$ = variableprop;	




			  }
			  | ID LBRACK expr LBRACK POINT ID {


			Node * filhos[] = {
			 	$3,
				
			};			


			char * folhas[] = {
			 	$1,
			 	$2,
			 	$4,
			 	$5,
			 	$6,
				
			};

			char* nome = "Propriedade de variavel de estrutura ";

			int tamanhofilhos = sizeof(filhos);
			int tamanhofolhas = sizeof(folhas);


			Node* variableprop = createNode(filhos, nome, folhas,tamanhofilhos,tamanhofolhas);
			$$ = variableprop;	


		}
		;




functioncall: ID LPAREN paramsoptionalCall RPAREN {
	
			Node * filhos[] = {
				$3,
			};

			char * folhas[] = {
    			$1,
    			$2,
    			$4,

			};			
		
			char* nome = "Chamada de função";

			int tamanhofilhos = sizeof(filhos);
			int tamanhofolhas = sizeof(folhas);

			Node* functioncall = createNode(filhos, nome, folhas,tamanhofilhos,tamanhofolhas);
			$$ = functioncall;	


}

paramsoptionalCall: paramsCall {

		
		Node * filhos[] = {
			 $1,
		};
		
		char* nome = "Paramsoptional -  parametro(s) opcionais da chamada de função ";
		int tamanhofilhos = sizeof(filhos);
		Node* paramsoptional = createNode(filhos, nome, NULL,tamanhofilhos,0);
		$$ = paramsoptional; 

		}


		| /* empty */
		;

paramsCall:	expr paramslistCall {

		
		Node * filhos[] = {
			 $1,
			 $2,
		};
		
		char* nome = "Params -  parametro(s)  da chamada de função ";

		int tamanhofilhos = sizeof(filhos);
		

		Node* params = createNode(filhos, nome, NULL,tamanhofilhos,0);
		
		$$ = params; 
		}
	    | /* empty */
	    ;



paramslistCall: paramslistCall COMMA expr {

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



	
condstmt: IF LPAREN expr RPAREN LBRACE cmds RBRACE matchornot {
	
			Node * filhos[] = {
				$3,
				$6,
				$8,
			};

			char * folhas[] = {
    			$1,
    			$2,
    			$4,
    			$5,
    			$7,

			};			
		
			char* nome = "Condicional - IF";

			int tamanhofilhos = sizeof(filhos);
			int tamanhofolhas = sizeof(folhas);

			Node* condstmt = createNode(filhos, nome, folhas,tamanhofilhos,tamanhofolhas);
			$$ = condstmt;	



}
	
matchornot: ELSE LBRACE cmds RBRACE {
	
			Node * filhos[] = {
				$3,
			};

			char * folhas[] = {
    			$1,
    			$2,
    			$4,
			};			
		
			char* nome = "Condicional - ELSE";

			int tamanhofilhos = sizeof(filhos);
			int tamanhofolhas = sizeof(folhas);

			Node* matchornot = createNode(filhos, nome, folhas,tamanhofilhos,tamanhofolhas);
			$$ = matchornot;
			

			}
			| /* empty */

content: content COMMA tdsformat {
	

			Node * filhos[] = {
				$1,
				$3,
			};

			char * folhas[] = {
    			$2,
			};			
		
			char* nome = "Conjunto ";

			int tamanhofilhos = sizeof(filhos);
			int tamanhofolhas = sizeof(folhas);

			Node* content = createNode(filhos, nome, folhas,tamanhofilhos,tamanhofolhas);
			$$ = content;
			
		 }
		 | tdsformat {

			Node * filhos[] = {
			 	$1,
				
			};

			char* nome = "TDS";

			int tamanhofilhos = sizeof(filhos);


			Node* content = createNode(filhos, nome, NULL ,tamanhofilhos,0);
			$$ = content;	

		 }
		 

tdsformat: LPAREN data COMMA expr COMMA ID RPAREN {
	

			Node * filhos[] = {
				$2,
				$4,
			};

			char * folhas[] = {
    			$1,
    			$3,
    			$5,
    			$6,
    			$7,
			};			
		
			char* nome = "Informações de TDS";

			int tamanhofilhos = sizeof(filhos);
			int tamanhofolhas = sizeof(folhas);

			Node* tdsformat = createNode(filhos, nome, folhas,tamanhofilhos,tamanhofolhas);
			$$ = tdsformat;


}


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
  printf("%s \n",root->name);
  fclose(fp);

}


void yyerror(const char *s) {
  printf("Erro de parsing! %s \n",s);
  exit(-1);
}