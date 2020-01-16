#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../../headers/Node.h"



Node* createNode(Node** children, char* name, char** leafs, int sizechildren, int sizeleafs){
	

	Node* current_node = (Node*) malloc(sizeof(Node)); // se por node depois de size childreen ele crasha!? SE TIRAR O PRINT TMB?

	int nt = sizechildren/sizeof (Node*);
	int nl = sizeleafs/sizeof (char *);
		
	
	current_node->nchild = nt;
	current_node->nleafs = nl;

	if(children){
		current_node->children = children;
	}
	if(name){
		current_node->name = name;
	}
	if(leafs){
		current_node->leafs = leafs;
		
	}

	return current_node;
}


void printNode(Node* n){	
	if(n){
		printf("NODE: - %s \n",n->name);
	}
	int i;
	if(n->children){
		printf("--------------CHILDREN: \n");
		for(i=0; i < n->nchild; i++){
			printNode(n->children[i]);
		}
		printf("--------------------------- \n");
	}
	if(n->leafs){
		printf("---------------NOT-TERMINAL: \n");
		for(i = 0; i< n->nleafs;i++){
			printf(" -->%s\n",n->leafs[i]);
		}
		printf("--------------------------- \n");
	}
	
	printf("-------\n");
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
	}
	if(n->leafs){
		for(i=0; i < n->nleafs; i++){
			free(n->leafs[i]);
		}
	}	
	free(n);
}






