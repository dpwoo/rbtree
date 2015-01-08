/*
 * main.c
 *
 *  Created on: 2014-12-13
 *      Author: dpwoo
 */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "rbtree.h"

static void
sighandle(int signo)
{
	if(signo == SIGSEGV) {
		printf("SIGSEGV catched!\n");
		exit(-1);
	}
}

#if 0
	static int array[] = {10, 85, 15, 70, 20, 60, 30, 50, 65, 80, 90, 40, 5, 55};
#define ITEM_SIZE sizeof(array)/sizeof(int)
#else
#define ITEM_SIZE 200
#endif
static struct rb_tree*
build_rb_tree()
{
	struct rb_tree *root = rb_tree_init();

	if(root) {
		int i;
		for(i = 0; i < ITEM_SIZE; i++) {
			rb_tree_insert(root, i);
		}
	}

	return root;
}

static void
delete_rb_tree(struct rb_tree *root)
{
	int i;
	for(i = 0; i < ITEM_SIZE; i++) {
		rb_tree_delete(root, i);
	}
}

static void
rb_tree_travsal_iter(struct rb_tree *root)
{
	struct rb_node *node;
	for(node = rb_tree_first(root); node; node = rb_tree_next(node)) {
		printf("%d%s\t", node->data, node->color ? "+B" : "+R");
	}
	printf("\n");
}

static void
rb_tree_travsal_iter_reverse(struct rb_tree *root)
{
	struct rb_node *node;
	for(node = rb_tree_last(root); node; node = rb_tree_prev(node)) {
		printf("%d%s\t", node->data, node->color ? "+B" : "+R");
	}
	printf("\n");
}

static void
rb_tree_main()
{
	struct rb_tree *root;

	root = build_rb_tree();
	//delete_rb_tree(root);
	rb_tree_travsal_iter(root);
	rb_tree_travsal_iter_reverse(root);
	rb_tree_destroy(root);
}

int main(void)
{
	signal(SIGSEGV, sighandle);

	rb_tree_main();

	return EXIT_SUCCESS;
}

