#include<stdio.h>
#include<stdlib.h>

typedef struct node{
	int value, size, index, depth;
	struct node *left;
	struct node *right;
}Node;


#define switch_node(a, b) do {			\
	Node *tmp = (b);			\
	(b) = (a);				\
	(a) = (tmp);				\
    } while (0)


Node *create_node(int value, int index){
	Node *tmp = (Node *)malloc(sizeof(Node));
	tmp -> index = index;
	tmp -> value = value;
	tmp -> size =  1;
	tmp -> depth =  0;
	tmp -> left = NULL;
	tmp -> right  = NULL;
	return tmp;
}


int min_heap_size(Node *a) {
	return (a == NULL)?  0:a->size ;
}


void a_size(Node *a) {
	a -> size =  min_heap_size(a -> left) +  min_heap_size(a -> right) + 1;
}
Node *merge(Node *a, Node *b) {
	if(a == NULL || b == NULL)
		return (a == NULL)? b:a ;
	

	if(a -> value > b ->  value){
		switch_node(a,b);
	}

	a -> size += b -> size;
	a -> right = merge(a -> right, b);

	if(a -> left == NULL  && a -> right)
		switch_node(a ->left, a -> right);
	
	else if(a -> right && a -> left -> depth < a -> right -> depth)
		switch_node(a ->left, a -> right);
		
	if(a -> right == NULL)
		a -> depth = 0;

	else
		a -> depth = a -> right -> depth + 1;

	a_size(a);
	return a;

}


Node *add_to_minheap(Node *r, int value, int index) {
	r = merge(r,create_node(value,index));
	return r;
}

Node *pop_minheap(Node *r) {
	Node *target = r;
	r = merge(r -> left, r -> right);
	free(target);
	return r;
}


