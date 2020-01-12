#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../../headers/Node.h"



Node* create(Node** children, char* name, char** leafs, int sizechildren, int sizeleafs){
	

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

	char* nome = "Yare Yare...?";
	char* nome1 = "Daze?";
	char* nome2 = "ZAWAROLDO.";





	Node* filho1 = create(NULL, nome1,testeLeaf1,0,sizeleafs1);
	Node* filho2 = create(NULL, nome2,testeLeaf2,0,sizeleafs2);

	Node * filhos[] = {
		filho1,
		filho2,
};
	
	int tamanho = sizeof(filhos);



	//printNode(filho1);
	//printNode(filho2);

	Node* no = create(filhos, "Yare yare...?", testeLeaf,tamanho,sizeleafs);
	printNode(no);

	int i;
	tamanho = sizeof(filhos)/sizeof(Node);



	for(i = 0; i< tamanho;i++){
		letgo(filhos[i]);
	}

	
	return 0;
}





