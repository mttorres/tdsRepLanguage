#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../../headers/Node.h"



Node* create(Node** children, char* name, char** leafs, int sizechildren, int sizeleafs){
	
	int nt = sizechildren/sizeof (Node*);
	int nl = sizeleafs/sizeof (char *);
	
	printf("%d \n",nt);
	printf("%d \n",nl);

	Node* current_node = malloc(sizeof(Node*));
	
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
		printf("NOT-TERMINAL: \n");
		for(i=0; i < n->nchild; i++){
			printNode(n->children[i]);
		}
	}
	if(n->leafs){
		printf("TERMINAL: \n");
		for(i = 0; i< n->nleafs;i++){
			printf(" -->%s\n",n->leafs[i]);
		}
	}
	
	printf("-------\n");
}

void letgo(Node* n){
	free(n);
}


int main(){
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

	int sizeleafs = sizeof(testeLeaf);
	int sizeleafs1 = sizeof(testeLeaf1);
	int sizeleafs2 = sizeof(testeLeaf2);

	Node* filho1 = create(NULL, "Daze.",testeLeaf1,0,sizeleafs1);
	Node* filho2 = create(NULL, "ZAWORLDO.",testeLeaf2,0,sizeleafs2);

	Node * filhos[] = {
		filho1,
		filho2,
};
	
	int tamanho = sizeof(filhos)/sizeof(Node*);
	
	printNode(filho1);
	printNode(filho2);

	//Node* no = create(filhos, "Yare yare...?", testeLeaf,tamanho,sizeleafs);
	//printNode(no);

	int i;
	for(i = 0; i< tamanho;i++){
		letgo(filhos[i]);
	}
	
	return 0;
}





