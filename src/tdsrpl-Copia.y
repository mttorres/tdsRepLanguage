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
%token <sval> FUNCTIONDOMAIN
%token <sval> FUNCTION
%token <sval> LABEL
%token <sval> DATATIME
%token <sval> PORTNAME
%token <sval> LINK
%token <sval> LINKED
%token <sval> ID

%type <ast> prog
%type <ast> cmds
%type <ast> cmd
%type <ast> functiondefs 
%type <ast> functiondef
%type <ast> paramsoptional 
%type <ast> paramslist 
%type <ast> params  
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
%type <ast> paramslistCall 
%type <ast> paramsCall
%type <ast> tdsformat
%type <ast> matchornot


%left ELSE
%left MULT DIV MINUS PLUS LE GE GT LT EQ NE


%start prog


%%

/*
	Gramatica para o parser 
*/

prog: functiondefs cmds  {
		
		Node * filhono[] = {
			 $1,
			 $2,
		};

		//printf("END1: %d \n \n",&filhono[0]);

		char* nome = "Prog -  comandos ou definições de função";
		int tamanhofilhos = sizeof(filhono);
		Node* prog = createNode(filhono, nome,NULL,tamanhofilhos,0);	
		
		//printf("PROGRAMA (%s) (%s) (%d) (filhos : %d) \n",prog->name,prog->children[1]->name ,prog->children[0] == NULL,prog->nchild); 	

		//printf("nome :(%s) (ref : %d) \n \n \n",$2->name,$2->children[0] != NULL); 

		//printf("END2: %d \n \n",prog->children);

		$$ = prog; 
		root = $$;
		

		//infoNode($$);

		//printf("PROGRAMA (%s) (%s) (%d) (filhos : %d) \n \n",prog->name,prog->children[1]->name ,prog->children[1] == NULL,prog->nchild);

		//printf("nome :(%s) (ref : %d) \n \n \n",$$->children[1]->name,$$->children[1]->children[0] != NULL); 
	  } 
      | /* empty */ {printf("nao teve mais (prog) \n \n"); $$ = NULL;}
      ; 


cmds: cmds cmd {
		
		Node * filhos[] = {
			 $1,
			 $2,
		};
		char* nome = "Cmds -  comando(s)";
		int tamanhofilhos = sizeof(filhos);
		Node* cmds = createNode(filhos, nome,NULL,tamanhofilhos,0);
		//printf("%s (%s) (%s) (filhos: %d)\n \n \n",cmds->name,cmds->children[0]->name,cmds->children[1]->name,cmds->nchild);
		$$ = cmds; 
		infoNode($$);

	  } 
	  | cmd {
		
		Node** filhos;

		filhos = (Node*) malloc(sizeof(Node*));

		filhos[0] = $1;
		
		char* nome = "Cmds -  comando(s)(REPASSA)";
		int tamanhofilhos = sizeof(filhos);

		Node* cmds = createNode(filhos, nome,NULL,tamanhofilhos,0);
		//printf("%s (%s) (filhos: %d) \n \n",cmds->name,cmds->children[0]->name,cmds->nchild);
		$$ = cmds; 	

		printf("END1(CMDS): %d \n \n",&filhos);

		printf("END2(CMDS): %d \n \n",cmds->children);

		infoNode($$);

		printf("nome :(%s) (ref : %d) \n \n \n",$$->name,$$->children[0] != NULL); 
						
	  } 	
	  | /* empty */  {
		printf("nao teve mais (cmds) \n \n"); 
		$$ = NULL;
	  }
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
		

		//printf("iniciando definição de função \n");
		infoNode($$);

	  }
	  |  functiondef {
	  	
		$$ = $1; 	
	  }  
	  | /* empty */  {printf("nao teve mais (functionsdefs) \n \n"); $$ = NULL;}
	  ; 	


functiondef: FUNCTION ID LPAREN paramsoptional RPAREN LBRACE cmds optionalreturn RBRACE  {

		Node * filhos[] = {
			 $4,
			 $7,
			 $8,
		};


		char * folhas[] = {
    			$1,
    			$2,
    			$3,
    			$5,
    			$6,
    			$9,

		};

		char* nome = "Functiondef -  definição de função ";

		int tamanhofilhos = sizeof(filhos);
		int tamanhofolhas = sizeof(folhas);

		Node* functiondef = createNode(filhos, nome,folhas,tamanhofilhos,tamanhofolhas);
		//printf("DEFINICAO DE FUNÇÃO \n");
		$$ = functiondef; 
		infoNode($$);
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

		| /* empty */  {printf("nao teve mais (paramsoptional) \n \n"); $$ = NULL;}
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
	    | /* empty */  {printf("nao teve mais(params) \n \n"); $$ = NULL;}
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
	    | COMMA ID {
	    	
		char * folhas[] = {
    			$1,
    			$2,
		};		
		
		char* nome = "Paramslist - lista de parâmetros a mais de uma função ";
		int tamanhofilhos = 0;
		int tamanhofolhas = sizeof(folhas);

		Node* paramslist = createNode(NULL, nome, folhas,tamanhofilhos,tamanhofolhas);
		$$ = paramslist; 
	    }
	    | /* empty */  { printf("nao teve mais (paramslist) \n \n"); $$ = NULL;}
	    ;



optionalreturn: RETURN expr {

		
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

		//printf("RETORNO \n");
		
		}
	    | /* empty */  {printf("nao teve mais(optionalreturn) \n \n"); $$ = NULL;}
	    ;


cmd: condstmt {
	
		Node * filhos[] = {
			 $1,
		};
		
		char* nome = "CMD -  Comando (condstatement) ";

		int tamanhofilhos = sizeof(filhos);
	

		Node* cmd = createNode(filhos, nome, NULL,tamanhofilhos,0);
		//printf("comando(cond) \n");
		$$ = cmd; 
		infoNode($$);
	  
	  }
	 | otherstmt {

		Node** filhos;

		filhos = (Node**) malloc(sizeof(Node*));

		filhos[0] = $1;

		char* nome = "CMD -  Comando (otherstatement) ";

		int tamanhofilhos = sizeof(filhos);
	
		Node* cmd = createNode(filhos, nome, NULL,tamanhofilhos,0);
		//printf("(%s) (%s) (filhos: %d) \n \n",cmd->name,cmd->children[0]->name,cmd->nchild);
		$$ = cmd; 		
		infoNode($$);


	 }
	 ;


otherstmt: FOR assignment TO expr DO COLON cmds {
	
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

		//printf("CMD - NAO IF/ELSE \n",otherstmt->name);
		infoNode($$);


	 }
	 | functioncall {

		Node** filhos;

		filhos = (Node**) malloc(sizeof(Node*));

		filhos[0] = $1;

		char* nome = "Otherstmt(functioncall) -  comandos diferente de if e else ";

		int tamanhofilhos = sizeof(filhos);


		Node* otherstmt = createNode(filhos, nome, NULL,tamanhofilhos,0);
		$$ = otherstmt;
		infoNode($$);

	 }
	 | assignment {

		Node** filhos;

		filhos = (Node**) malloc(sizeof(Node*));

		filhos[0] = $1;

		char* nome = "Otherstmt(assignment) -  comandos diferente de if e else ";

		int tamanhofilhos = sizeof(filhos);


		Node* otherstmt = createNode(filhos, nome, NULL,tamanhofilhos,0);
		//printf("%s (%s) - (filhos: %d) \n \n",otherstmt->name, otherstmt->children[0]->name,otherstmt->nchild);
		$$ = otherstmt;
		infoNode($$);
		

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
		printf("(!!)atribuicao (%s) \n \n",assignment->leafs[0]);
		$$ = assignment; 
		infoNode($$);

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


multiexp: multiexp TIMES expr {
	
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
		|multiexp DIVIDE expr {

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

ineqexp:  ineqexp LE expr {
	
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
		| ineqexp GE expr {

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
		| ineqexp LT expr {

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
		| ineqexp GT expr {

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
        | ineqexp EQUAL expr {

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
        | ineqexp NOTEQUAL expr {

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
			//printf("logical \n");
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

			//printf("dados genericos \n");
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
			//printf("BOOLEANO \n");
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
			//printf("TDS FORMAT! \n");
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
			//printf("LABEL \n");
			$$ = data;					
		
		}
		;

//! trocar para propriedades da TDS
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




functioncall: ID LPAREN paramsCall RPAREN {
	
				Node* functioncall = createNode(7,1,3,"Chamada de função", $3,  $1,$2,$4);
				$$ = functioncall;		

			}
			| ID LPAREN RPAREN {
				
				Node* functioncall = createNode(6,0,3,"Chamada de função", $1,$2,$3);
				$$ = functioncall;				
			
			}


paramsCall: expr {
		
			Node* paramsCall = createNode(4,1,0,"Params -  parametro da chamada de função ", $1);
			$$ = paramsCall; 		

		}
	    | paramsCall COMMA expr {
			
			Node* paramsCall = createNode(6,2,1,"Params -  parametro(s)  da chamada de função LOOP", $1,$3, $2);
			$$ = paramsCall; 	
	    }


//DEAD(!)
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
	    | /* empty */  {printf("nao teve mais (paramslistCall) \n \n"); $$ = NULL;}
	    ;



	
condstmt: IF LPAREN expr RPAREN LBRACE cmds RBRACE matchornot {
	
		Node* condstmt = createNode(11,3,5,"Condicional - IF", $3,$6,$8 , $1,$2,$4,$5,$7);	
		$$ = condstmt;
		//printf("CMD -  IF\n");

}
	
matchornot: ELSE LBRACE cmds RBRACE {
	
		Node* matchornot = createNode(7,1,3,"Condicional - ELSE", $3 , $1,$2,$4);	
		$$ = matchornot;
		//printf("CMD - ELSE \n");

		}
		| /* empty */  { $$ = NULL;}
		;




//DEAD(substituir)
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
			
			//printf("CONTEUDO/conjunto DE TDS \n");

			$$ = content;
			
		 }
		 | tdsformat {
			
			Node* content = createNode(4,1,0,"TDS", $1);			
			//printf("formato  DE TDS \n");
			$$ = content;	

		 }
		 

tdsformat:  LBRACE PORTNAME COLON LABEL COMMA DATATIME COMMA LBRACE dataflow RBRACE COMMA extra RBRACE {
		
	Node* tdsformat = createNode(16,2,11,"Informações de TDS", $9,$12,  $1,$2,$3,$4,$5,$6,$7,$8,$10,$11,$13);
	$$ = tdsformat;

}

dataflow: LBRACE domain RBRACE  {
	
	// evita conflitos (nó a mais porem evita)
	Node* dataflow = createNode(4,1,0,"dataflow-TDS", $1,$3, $2);
	$$ = dataflow; 	
}


domain: timelist { 

			Node* domain = createNode(4,1,0,"DOMAIN-TDS-non-func", $1);
			$$ = domain;    

		}
		| FUNCTIONDOMAIN COLON functionCall {

			Node* domain = createNode(6,1,2,"DOMAIN-TDS-func", $1,$2, $3);
			$$ = domain;    

		}



timecomponent: RAWNUMBERDATA COLON expr {
			   
	Node* timecomponent = createNode(6,1,2,"TIME-COMPONENT-TDS", $3, $1,$2);
	$$ = timecomponent;    
}



timelist: timecomponent { 

			Node* timelist = createNode(4,1,0,"TIME-LIST-TDS-one", $1);
			$$ = timelist;    			 		  
		  }

		  |timelist COMMA timecomponent {

			Node* timelist = createNode(6,2,1,"TIME-LIST-TDS-loop", $1,$3, $2 );
			$$ = timelist;		  		  

		  }


extra : LINK COLON ID {
	
			Node* extra = createNode(6,0,3,"LINK-EXTRA-ARGS-TDS", $1,$2,$3);
			$$ = extra;
	   }
	   | LINKED LBRACE paramslist RBRACE {
	   	 	Node* extra = createNode(7,1,3,"LINKED-EXTRA-ARGS-TDS",$3, $1,$2,$4);
	   	 	$$ = extra;
	   }
	   | DELAYED COLON BOOLEAN {
	   		Node* extra = createNode(6,0,3,"DELAYED-EXTRA-ARGS-TDS", $1,$2,$3);
	   		$$ = extra;
	   }
	   | /* empty */ {  $$ = NULL;}
       ; 

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

  printf("--------------TREE--------------------\n");
  printNode(root);
  letgoNode(root);
  fclose(fp);

}


void yyerror(const char *s) {
  printf("Erro de parsing! %s \n",s);
  exit(-1);
}
