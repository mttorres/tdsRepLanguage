%{

/*
*	inicilização de headers incluindo AST e Table
*/   

  #include <stdlib.h>
  #include <stdio.h>
  #include <string.h>
  #include "../headers/Node.h"	

  int yylex(void);
  int yyparse();
  FILE *yyin;

  	
  Node* root;
 
  extern void yyerror(const char *s);

  //typedef enum {false, true} BOOLEANO;

%}

/*
*	union para retornar os tipos desejados de dados para os tokens do lexer
	retornados como um yystype.  em que devemos definir os tipos por meio de uma union (pode assumir qualquer um dos valores dela)
*/ 

%locations 

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
%token <sval> MOD
%token <sval> FUNCTIONDOMAIN
%token <sval> FUNCTION
%token <sval> LABEL
%token <sval> DATATIME
%token <sval> DELAYED
%token <sval> PORTNAME
%token <sval> LINKED
%token <sval> INITTIME
%token <sval> CURRENTTIME
%token <sval> FINALTIME
%token <sval> ID
%token <sval> PASS
%token <sval> DPASS

%type <ast> prog
%type <ast> cmds
%type <ast> cmd
%type <ast> functiondefs 
%type <ast> functiondef
%type <ast> functionbody 
%type <ast> params  
%type <ast> optionalreturn
%type <ast> paramsOpcional
%type <ast> data
%type <ast> otherstmt
%type <ast> assignable
%type <ast> expr
%type <ast> multiexp
%type <ast> ineqexp
%type <ast> logical
%type <ast> variabledata
%type <ast> variableprop
%type <ast> functioncall 
%type <ast> paramsCall
%type <ast> matchornot
%type <ast> extraaccesses
%type <ast> extras
%type <ast> delayedoption
%type <ast> dataflow
%type <ast> tdsprop
%type <ast> domain
%type <ast> timelist
%type <ast> timecomponent
%type <ast> timedirective
%type <ast> anonimtdsop
%left ELSE
%left TIMES DIVIDE MINUS PLUS LE GE GT LT EQUALS NOTEQUAL


%start prog


%%

/*
	Gramatica para o parser 
*/

prog: functiondefs cmds  {
		
		Node* prog = createNode(5,2,0,"Prog -  comandos e definições de função", $1,$2);
		
		//printf("PROGRAMA (%s) (%s) (%d) (filhos : %d) \n",prog->name,prog->children[1]->name ,prog->children[0] == NULL,prog->nchild); 	

		//printf("nome :(%s) (ref : %d) \n \n \n",$2->name,$2->children[0] != NULL); 

		//printf("END2: %d \n \n",prog->children);
		
		//infoNode($$);

		//printf("PROGRAMA (%s) (%s) (%d) (filhos : %d) \n \n",prog->name,prog->children[1]->name ,prog->children[1] == NULL,prog->nchild);

		//printf("nome :(%s) (ref : %d) \n \n \n",$$->children[1]->name,$$->children[1]->children[0] != NULL); 

		$$ = prog; 
		root = $$;

	  }
	  | cmds {

		Node* prog = createNode(4,1,0,"Prog -  comandos ", $1);	  
	  	$$ = prog; 
	  	root = $$;
	  } 
      ; 


cmds: cmds cmd {
		
	  	Node* cmds = createNode(5,2,0,"Cmds -  comando(s)", $1,$2);
		//printf("%s (%s) (%s) (filhos: %d)\n \n \n",cmds->name,cmds->children[0]->name,cmds->children[1]->name,cmds->nchild);
		$$ = cmds; 
		//infoNode($$);

	  } 
	  | cmd {
		
		
	    $$ = $1; 

		//printf("%s (%s) (filhos: %d) \n \n",cmds->name,cmds->children[0]->name,cmds->nchild);
			

		//printf("END1(CMDS): %d \n \n",&filhos);

		//printf("END2(CMDS): %d \n \n",cmds->children);

		//infoNode($$);

		//printf("nome :(%s) (ref : %d) \n \n \n",$$->name,$$->children[0] != NULL); 
						
	  } 	
	  ;	

functiondefs: functiondefs functiondef {
		
	  	Node* functiondefs = createNode(5,2,0,"Functiondefs -  definição(ões) de função - loop", $1,$2);
		$$ = functiondefs; 
		//printf("iniciando definição de função \n");
		//infoNode($$);

	  }
	  |  functiondef {
	  	
		   $$ = $1; 	
	  }  
	  ; 	

functiondef: FUNCTION ID LPAREN params RPAREN LBRACE functionbody  RBRACE  {

			Node* functiondef = createNode(11,2,6,"Functiondef -  definição de função ", $4,$7, $1,$2,$3,$5,$6,$8);
			//printf("DEFINICAO DE FUNÇÃO \n");
			$$ = functiondef; 
			//infoNode($$);
		
		}
		| FUNCTION ID LPAREN RPAREN LBRACE functionbody  RBRACE  {

			Node* functiondef = createNode(10,1,6,"Functiondef -  definição de procedimento ", $6, $1,$2,$3,$4,$5,$7);
			//printf("DEFINICAO DE FUNÇÃO \n");
			$$ = functiondef; 
			//infoNode($$);

		}

		
;

// MUDANÇA - NAO PERMITIR CHAMADA A TDS CREATE dentro de funções! (ou verificar em tempo de semantica?)
functionbody: cmds optionalreturn {
	
	  		Node* functionbody = createNode(5,2,0,"Functionbody - função composta/programa ", $1,$2);
			$$ = functionbody; 
			
		}


optionalreturn: RETURN expr {

		Node* optionalreturn = createNode(5,1,1,"Optionalreturn -  retorno opcional ", $2, $1);
		$$ = optionalreturn; 

		//printf("RETORNO \n");
		
		}
	    | /* empty */  {printf("nao teve mais(optionalreturn) \n \n"); $$ = NULL;}
	    ;




params: ID {
		
			Node* params = createNode(4,0,1,"Params -  parametro(s)  da definição de função", $1);
			$$ = params; 		

		}
	    | params COMMA ID {
			
			Node* params = createNode(6,1,2,"Paramslist - lista de parâmetros a mais de uma função ", $1, $2,$3);
			$$ = params; 	
	    }




cmd: IF LPAREN expr RPAREN LBRACE cmds RBRACE matchornot {
	
		Node* condstmt = createNode(11,3,5,"cmd - Condicional - IF", $3,$6,$8 , $1,$2,$4,$5,$7);	
		$$ = condstmt;
		//printf("CMD -  IF\n");
	  
	 }
	 | otherstmt {

		Node* cmd = createNode(4,1,0,"CMD -  Comando (otherstatement)", $1); 
		//infoNode($$);
		//printf("(%s) (%s) (filhos: %d) \n \n",cmd->name,cmd->children[0]->name,cmd->nchild);
		$$ = cmd; 		
	 }
	 // jogar para nó mais abaixo a operação para avaliar antes de toda a expressão!

	 | LPAREN params RPAREN anonimtdsop ID {

		Node* cmd = createNode(8,2,3,"CMD -  Comando (criação TDS-anonima)", $2,$4,  $1,$3,$5); 
		//infoNode($$);
		//printf("(%s) (%s) (filhos: %d) \n \n",cmd->name,cmd->children[0]->name,cmd->nchild);
		$$ = cmd; 	 	
	 }
	 ;


anonimtdsop: PASS  {
			Node* p = createNode(4,0,1,"CMD -  PASS", $1); 
			$$ = p;
		}
		| DPASS  {
			Node* p = createNode(4,0,1,"CMD -  DPASS", $1); 
			$$ = p;
		} 


paramsOpcional: paramsCall {

		Node* paramsCall = createNode(4,1,0,"ParamsOpcional -  parametros são utilizados", $1);
		$$ = paramsCall; 

	}
	| /* empty */ {  $$ = NULL;}


paramsCall: expr {
		
			Node* paramsCall = createNode(4,1,0,"Params -  parametro da chamada de função ", $1);
			$$ = paramsCall; 		

		}
	    | paramsCall COMMA expr {
			
			Node* paramsCall = createNode(6,2,1,"Params -  parametro(s)  da chamada de função LOOP", $1,$3, $2);
			$$ = paramsCall; 	
	    }



functioncall: ID LPAREN paramsOpcional RPAREN {
	
				Node* functioncall = createNode(7,1,3,"Chamada de função", $3,  $1,$2,$4);
				$$ = functioncall;		

			}






otherstmt: FOR ID ASSIGN expr TO expr DO COLON cmds {
	
		Node* otherstmt = createNode(12,3,6,"Otherstmt(for) -  comandos diferente de if e else ",   $4,$6,$9,  $1,$2,$3,$5,$7,$8);
		$$ = otherstmt; 
		//printf("CMD - NAO IF/ELSE \n",otherstmt->name);
		//infoNode($$);


	 }
	 | functioncall {

		Node* otherstmt = createNode(4,1,0,"Otherstmt(functioncall) -  comandos diferente de if e else ", $1);
		$$ = otherstmt; 
		//infoNode($$);

	 }
	 | assignable ASSIGN expr {
		
		Node* assignment = createNode(6,2,1,"Assignment", $1,$3, $2);
		$$ = assignment;	

		//printf("(!!)atribuicao (%s) \n \n",assignment->leafs[0]);
		//infoNode($$);

	 }	 
	 ;



assignable : ID extraaccesses {
		
		Node* assignment = createNode(5,1,1,"Assignment(simples) -  atribuição de variavel  ", $2, $1);
		$$ = assignment;	

		//printf("(!!)atribuicao (%s) \n \n",assignment->leafs[0]);
		//infoNode($$);

	 }
	 | timedirective {
		
		Node* assignment = createNode(4,1,0,"Assignment(simples) -  atribuição diretiva ", $1);
		$$ = assignment;	

		//printf("(!!)atribuicao (%s) \n \n",assignment->leafs[0]);
		//infoNode($$);

	 }	 
	 ;	 


expr: MINUS expr {
	
		Node* expr = createNode(5,1,1,"Expressão Básica - negativo", $2,  $1);
		$$ = expr;		


	 }
	 | expr MINUS multiexp {
		
		Node* expr = createNode(6,2,1,"Expressão Básica - subtração ", $1,$3,  $2);
		$$ = expr;			

	 }
	 | expr PLUS multiexp {

		Node* expr = createNode(6,2,1,"Expressão Básica - soma ", $1,$3,  $2);
		$$ = expr;						
		

	 }
	 | multiexp {

		$$ = $1;	

	 }
	 ;


multiexp: multiexp TIMES ineqexp {
	
			Node* multiexp = createNode(6,2,1,"Expressão Básica - Multiplicação", $1,$3,  $2);
			$$ = multiexp;					
		

		}
		|multiexp DIVIDE ineqexp {
	
			Node* multiexp = createNode(6,2,1,"Expressão Básica - Divisão ", $1,$3,  $2);
			$$ = multiexp;					
		

		}
		|multiexp MOD ineqexp {
	
			Node* multiexp = createNode(6,2,1,"Expressão Básica - MOD ", $1,$3,  $2);
			$$ = multiexp;

		}
		|ineqexp {

			Node* multiexp = createNode(4,1,0,"Expressão Básica - inequações/lógicas ", $1);
			$$ = multiexp;			
			
		}
		;

ineqexp:  ineqexp LE logical {
	

			Node* ineqexp = createNode(6,2,1,"Inequação  - Menor igual a ", $1,$3,  $2);
			$$ = ineqexp;						
		

		}
		| ineqexp GE logical {

			Node* ineqexp = createNode(6,2,1,"Inequação  - Maior igual a ", $1,$3,  $2);
			$$ = ineqexp;				

		}
		| ineqexp LT logical {

			Node* ineqexp = createNode(6,2,1,"Inequação  - Menor que ", $1,$3,  $2);
			$$ = ineqexp;

		}
		| ineqexp GT logical {

			Node* ineqexp = createNode(6,2,1,"Inequação  - Maior que ", $1,$3,  $2);
			$$ = ineqexp;			

	
		}
        | ineqexp EQUAL logical {

			Node* ineqexp = createNode(6,2,1,"Inequação  - Igual a ", $1,$3,  $2);
			$$ = ineqexp;				
		
        }
        | ineqexp NOTEQUAL logical {

			Node* ineqexp = createNode(6,2,1,"Inequação  - Diferente de ", $1,$3,  $2);
			$$ = ineqexp;	
	
        }
        | logical {

			Node* ineqexp = createNode(4,1,0,"Expressão Básica - Lógica", $1);
			$$ = ineqexp;				
			//printf("logical \n");
        }
        ;


logical: NOT data {
	
			Node* logical = createNode(5,1,1,"Lógico  - Negação ", $2,  $1);
			$$ = logical;					

		 }
		 | logical AND data {

			Node* logical = createNode(6,2,1,"Lógico  - AND ", $1,$3,  $2);
			$$ = logical;	

		 }
		 | logical OR data {

			Node* logical = createNode(6,2,1,"Lógico  - OR ", $1,$3,  $2);
			$$ = logical;			

		 }
		 | data {

		 	$$ = $1;

		 }
		 ;
		 
data: RAWNUMBERDATA {
	
			Node* data = createNode(4,0,1,"Número", $1);	
			$$ = data;	

	  }
	  | BOOLEAN {

			Node* data = createNode(4,0,1,"Booleano", $1);	
			$$ = data;	

	  }
	  | Null {

			Node* data = createNode(4,0,1,"NULL", $1);	
			$$ = data;	

	  }

	  | INITTIME {
		
			Node* data = createNode(4,0,1,"INITTIME-DIRECTIVE", $1);	
			$$ = data;
		
	  }


	  | CURRENTTIME {

			Node* data = createNode(4,0,1,"CURRENTTIME-DIRECTIVE", $1);	
			$$ = data;	

	  }

	  | FINALTIME {

			Node* data = createNode(4,0,1,"FINALTIME-DIRECTIVE", $1);	
			$$ = data;	

	  }
	  	  

	  | LABEL {
			
			Node* data = createNode(4,0,1,"LABEL", $1);	
			$$ = data;	

	  }

	  | ID extraaccesses {

			Node* data = createNode(5,1,1,"variáveis simples ou compostas", $2, $1);	
			$$ = data;		

	  }

	  | variabledata {

	  	Node* data = createNode(4,1,0,"Dados de tds ou função", $1);	
		$$ = data;	

	  }
	  ;


timedirective: INITTIME {
		
			Node* data = createNode(4,0,1,"CHANGE-INITTIME-DIRECTIVE", $1);	
			$$ = data;
		
	  }


	  | CURRENTTIME {

			Node* data = createNode(4,0,1,"CHANGE-CURRENTTIME-DIRECTIVE", $1);	
			$$ = data;	

	  }

	  | FINALTIME {

			Node* data = createNode(4,0,1,"CHANGE-FINALTIME-DIRECTIVE", $1);	
			$$ = data;

	  }
// diferenciar depoisnumber (index) e number data! (apesar de serem tokens "identicos em teoria")
// vetores de tds?  avaliar
extraaccesses : LBRACK expr RBRACK variableprop {
	
			  	 	Node* extraaccesses = createNode(7,2,2,"propriedades extra de variavel composta", $2,$4,  $1,$3);	
					$$ = extraaccesses;		

			  }
			  | variableprop {
			  	 	
			  	 	Node* extraaccesses = createNode(4,1,0,"propriedades extra de variavel simples", $1);	
					$$ = extraaccesses;	
			  }

//! trocar para propriedades da TDS (vai ser vetor e tds as "estruturas dessa linguagem")

variableprop: POINT tdsprop {
	
				Node* variableprop = createNode(5,1,1,"Propriedade de variavel - tdsprop", $2,   $1);
				$$ = variableprop;   

			}
			| /* empty */  { $$ = NULL;}
			;





tdsprop: functioncall {
		
		Node* prop = createNode(4,1,0,"Props TDS - function", $1);
		$$ = prop; 	
	  
	  }
	  | PORTNAME {

		Node* prop = createNode(4,0,1,"Props TDS - portname", $1);
		$$ = prop; 	

	  }
	  | DATATIME {

		Node* prop = createNode(4,0,1,"Props TDS - datatime", $1);
		$$ = prop; 		  

	  }
	  | LINKED {

		Node* prop = createNode(4,0,1,"Props TDS - linked", $1);
		$$ = prop; 		  

	  }
	  | DELAYED {

		Node* prop = createNode(4,0,1,"Props TDS - delay", $1);
		$$ = prop; 	

	  }



variabledata: LBRACE PORTNAME COLON LABEL COMMA DATATIME COLON LBRACE dataflow RBRACE extras RBRACE {
		
			Node* tdsformat = createNode(15,2,10,"Informações de TDS", $9,$11,  $1,$2,$3,$4,$5,$6,$7,$8,$10,$12);
			$$ = tdsformat;

		}
		| LBRACE PORTNAME COLON LABEL extras RBRACE {
			
			Node* tdsformat = createNode(9,1,5,"Informações de TDS", $5,  $1,$2,$3,$4,$6);
			$$ = tdsformat;
		
		} 		 
		// causa possível da redundancia (podemos separar em procedure call e function call?)
	  	| functioncall {

			Node* variabledata = createNode(4,1,0,"Dados de chamada de função", $1);	
			$$ = variabledata;	

	  	}			  
		;

	
	
matchornot: ELSE LBRACE cmds RBRACE {
	
		Node* matchornot = createNode(7,1,3,"Condicional - ELSE", $3 , $1,$2,$4);	
		$$ = matchornot;
		//printf("CMD - ELSE \n");

		}
		| /* empty */  { $$ = NULL;}
		;



dataflow:  domain   {
	
	// evita conflitos (nó a mais porem evita)
	Node* dataflow = createNode(4,1,0,"dataflow-TDS", $1);
	$$ = dataflow; 	
}


domain: timelist { 

			Node* domain = createNode(4,1,0,"DOMAIN-TDS-non-func", $1);
			$$ = domain;    

		}
		| FUNCTIONDOMAIN COLON expr {

			Node* domain = createNode(6,1,2,"DOMAIN-TDS-func", $3, $1,$2);
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

			Node* timelist = createNode(6,2,1,"TIME-LIST-TDS-loop", $1,$3,  $2 );
			$$ = timelist;		  		  

		  }




extras: COMMA LINKED COLON LBRACE params RBRACE delayedoption {
	
	   	 	Node* extra = createNode(10,2,5,"LINKED-EXTRA-ARGS-TDS",$5,$7,  $1,$2,$3,$4,$6);
	   	 	$$ = extra;
	}
	| delayedoption {
			Node* extra = createNode(4,1,0,"LINKED-EXTRA-ARGS-TDS",$1);
	   	 	$$ = extra;
	}
	

delayedoption: COMMA DELAYED COLON BOOLEAN {
	   		
		Node* extra = createNode(7,0,4,"DELAYED-EXTRA-ARGS-TDS", $1,$2,$3,$4);
	   	$$ = extra;
	   }
	   | /* empty */ {  $$ = NULL;}
       ; 

%%


