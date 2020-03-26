#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

typedef struct astnode
{
  struct astnode** children;
  char** leafs;
  char* name;
  int nleafs;
  int nchild;

}Node;


Node* createNode(int numArgs, ...);

void printNode(Node* n);

void letgoNode(Node *);

void testeFunction(int numArgs, ...);



void testeFunction(int numArgs, ...) {
    
   va_list args;
   va_start(args, numArgs); // (nada)
   int i;
   
   int sizechildren = va_arg(args, int); // 1
   printf("arg(1): %d \n",sizechildren); 
   int sizeleafs = va_arg(args, int); // 2
   printf("arg(2): %d \n",sizeleafs);
   
   
   
   for (i = 3; i < numArgs; i++) {
     if(i < 3+sizechildren){
        Node * n = va_arg(args, Node*);
        printf("arg(%d): %s\n",i,n->name); 
     }
     if(3+sizechildren <= i && i < 3+sizechildren+sizeleafs)
         printf("arg(%d): %s\n",i,va_arg(args, char*));
   }
   
   va_end(args);
    
}
//int sizechildren, int sizeleafs, char* name, Node* children..., char** leafs ... ? 

Node* createNode(int numArgs, ...){
	
    Node* current_node = (Node*) malloc(sizeof(Node)); // se por node depois de size childreen ele crasha!? SE TIRAR O PRINT TMB?

   va_list args;
   va_start(args, numArgs); // (nada)
   int i;
   int parametrosInicializacao = 4;
   
   current_node->nchild = va_arg(args, int); // 1
   //printf("arg(1): %d \n",current_node->nchild); 
   current_node->nleafs = va_arg(args, int); // 2
   //printf("arg(2): %d \n",current_node->nleafs);   
   current_node->name = va_arg(args, char*); // 3
   //printf("arg(3): %s \n",current_node->name);
   
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
	printf("info: - %d \n",info);		
	if(info) {
		printf("!! \n");			
		printf("NODE: - %s \n",n->name);
		int i = 0;
		if(n->children){
			printf("--------------CHILDREN: \n");
			for(i= 0; i < n->nchild; i++){
				if(n->children[i]) {
					printNode(n->children[i]);
				}
				else {
					printf("NULL \n");	
				}

			}
			printf("------------END CHILDREN--------------- \n");
		}
		if(n->leafs) {
			printf("---------------NOT-TERMINAL: \n");
			for(i = 0; i< n->nleafs;i++){					
			   printf(" --> %s \n",n->leafs[i]);
			}
			printf("------------END NOT-TERMINAL--------------- \n");
		}
		printf("-----------------------------------------------\n");
	}
}

void infoNode(Node* n){
	if(n){
		printf("-------------INFO---------------\n");
		printf("NODE: - %s \n",n->name);
		printf("nChild: - %d  ref: %d \n",n->nchild,n->children != NULL);
		printf("nLeafs: - %d  ref: %d \n",n->nleafs,n->leafs != NULL);
		printf("--------------END-INFO--------------\n \n");
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

	if(n->leafs) {
		//for(i = 0; i < n->nleafs; i++){					
		//	printf("oioioi\n");
		//	free(n->leafs[i]);
		//}
		free(n->leafs);
	}
	//printf("damn son \n");
	free(n);
}







int main()
{
    	char * testeLeaf[] = {
    "oi",
    "eu",
    "sou",
    "o",
    "goku!",
};
	
	
	char * testeLeaf1[] = {
    "eu",
    "tu",
    "nós",
    "...",
};

	char * testeLeaf2[] = {
    "chega",
    "de",
    "memes",
};

	//int sizeleafs = sizeof(testeLeaf);
	//int sizeleafs1 = sizeof(testeLeaf1);
	//int sizeleafs2 = sizeof(testeLeaf2);

	char* nome = "Yare Yare...?";
	char* nome1 = "Daze?";
	char* nome2 = "ZAWAROLDO.";



	Node* filho1 = createNode(7,0,4,nome1,testeLeaf1[0],testeLeaf1[1],testeLeaf1[2],testeLeaf1[3]);
	Node* filho2 = createNode(6,0,3,nome2,testeLeaf2[0],testeLeaf2[1],testeLeaf2[2]);

	printNode(filho1);
	printNode(filho2);

	Node* no = createNode(10,2,5,nome,filho1,filho2,testeLeaf[0],testeLeaf[1],testeLeaf[2],testeLeaf[3],testeLeaf[4]);
	printNode(no);

	//tamanho = sizeof(filhos)/sizeof(Node);
	printf("we are here \n");
    int tamanho = 2;

    letgoNode(no);
    
    
//	for(i = 0; i< tamanho;i++){
//		letgoNode(filhos[i]);
//	}
	//testeFunction(10,3,5,filho1,filho2,no,"1?","2?","3?","4?","5?");


    return 0;
}
