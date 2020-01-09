#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../../headers/Node.h"



Node* create(Node** children, char* name, char** leafs){
	Node* current_node = malloc(sizeof(Node *));
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
		printf("Node: - %s \n",n->name);
	}
	printf("-------\n");
}




int main(){

	printNode(NULL);
	return 0;
}





