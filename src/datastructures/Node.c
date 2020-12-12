#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "../../headers/Node.h"


const char* mappingEnumNode[] =  {

        "NUMBER", "L_BOOL", "STRING", "D_NULL", "IDVAR", "TIME_DIRECTIVE",

        "DATA_V", "PARAMS_CALL", "DEFINE_INTERVAL",

        "AC_V", "OTHER_ASSIGN", "V_PROP", "ADD_V", "ADD_V_PROP", "V_PROP_TDS",

        "EXPR", "CMD_IF", "MATCH_IF",

        "ASSIGN_IDVAR", "ASSIGN_AC_V", "ASSIGN_TDIRECTIVE",

        "TDS_DEF_COMPLETE", "TD_DEF_DEPEN",

        "DOMAIN_FUNCTION", "TIME_LIST", "TIME_COMP",

        "DEF_EXTRAS_LINKED", "DEF_EXTRAS_DELAYED",

        "HEADERS_E_PROG", "PROG", "FUNC_DEFS", "CMD",

        "FUNC_DEF", "PROC_DEF", "F_BODY", "OPT_RETURN", "PARAMS", "PARAM",

         "CMD_OTHER",

        "CMD_TDS_ANON", "TDS_ANON_OP_PASS", "TDS_ANON_OP_DPASS",

        "OTHER_LOOP", "FUNC_CALL", "PROC_CALL"
};


Node* createNode(int numArgs, ...){
	
   Node* current_node = (Node*) malloc(sizeof(Node)); // se por node depois de size childreen ele crasha!? SE TIRAR O PRINT TMB?

   va_list args;
   va_start(args, numArgs); // (nada)
   int i;
   int parametrosInicializacao = 5;
   
   current_node->nchild = va_arg(args, int); // 1
   //printf("arg(1): %d \n",current_node->nchild); 
   current_node->nleafs = va_arg(args, int); // 2
   //printf("arg(2): %d \n",current_node->nleafs);   
   current_node->name = va_arg(args, char*); // 3
   //printf("arg(3): %s \n",current_node->name);
   current_node->type = va_arg(args, EVAL_TYPE); // 4
  	

   
   int parametrosFilhos = parametrosInicializacao + current_node->nchild;
   //printf("parametrosFilhosSTART : %d \n",parametrosFilhos);
   
	
	if(current_node->nchild > 0){
		//printf("CRIANDO FILHOS \n");
		Node** children = (Node**) malloc(current_node->nchild*sizeof(Node*));
		int pos = 0;
		for(i = parametrosInicializacao; i < parametrosFilhos; i++){
		    children[pos] = va_arg(args, Node*); 
		    pos++;
		}
		current_node->children = children;
	}

	if(current_node->nleafs > 0){
		//printf("CRIANDO FOLHAS \n");
		char** leafs = (char**) malloc(current_node->nleafs*sizeof(char*));
		int pos = 0;
		for(i = parametrosFilhos; i <= numArgs; i++){
		    
		    leafs[pos] = va_arg(args, char*); 
		    pos++;
		}		
		current_node->leafs = leafs;
		
	}
	
	va_end(args);

	return current_node;
}


void printNode(Node* n){	

	int info = n != NULL;
	//printf("info: - %d \n",info);		
	if(info) {
		//printf("!! \n");			
		printf("NODE: - %s \n\n",mappingEnumNode[n->type]);
		int i = 0;
		if(n->children){
			printf("--------------CHILDREN: \n\n");
			for(i= 0; i < n->nchild; i++){
				if(n->children[i]) {
					printNode(n->children[i]);
				}
				else {
					printf("NULL \n");	
				}

			}
			printf("------------END CHILDREN(NODE: - %s)--------------- \n",n->name);
		}
		if(n->leafs) {
			printf("---------------TERMINAL: \n\n");
			for(i = 0; i< n->nleafs;i++){					
			   printf(" --> %s \n",n->leafs[i]);
			}
			printf("------------END TERMINAL(NODE: - %s)--------------- \n\n",n->name);
		}
		printf("-----------------------------------------------\n\n");
	}
}

void infoNode(Node* n){
	if(n){
		printf("-------------INFO---------------\n\n");
		printf("NODE: - %s \n",n->name);
		printf("nChild: - %d  ref: %d \n",n->nchild,n->children != NULL);
		printf("nLeafs: - %d  ref: %d \n",n->nleafs,n->leafs != NULL);
		printf("--------------END-INFO--------------\n \n");
	}
	
		
}

void filePrintNode(Node* n, FILE* fp){	

	int info = n != NULL;	
	if(info) {		
		fprintf(fp,"NODE: - %s \n\n",mappingEnumNode[n->type]);
		int i = 0;
		if(n->children){
			fprintf(fp,"--------------CHILDREN: \n\n");
			for(i= 0; i < n->nchild; i++){
				if(n->children[i]) {
					filePrintNode(n->children[i],fp);
				}
				else {
					fprintf(fp,"NULL \n");	
				}

			}
			fprintf(fp,"------------END CHILDREN(NODE: - %s)--------------- \n",n->name);
		}
		if(n->leafs) {
			fprintf(fp,"---------------TERMINAL: \n\n");
			for(i = 0; i< n->nleafs;i++){					
			   fprintf(fp," --> %s \n",n->leafs[i]);
			}
			fprintf(fp,"------------END TERMINAL(NODE: - %s)--------------- \n\n",n->name);
		}
		fprintf(fp,"-----------------------------------------------\n\n");
	}
}


void letgoNode(Node* n){
	if(!n){
		return;
	}
	int i;
	if(n->children){
		for(i=0; i < n->nchild; i++){
			letgoNode(n->children[i]);
		}
		free(n->children);
	}
	if(n->leafs){
		free(n->leafs);
	}	
	free(n);
}






