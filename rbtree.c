/*
 * rbtree.c
 *
 *  Created on: 2014-12-12
 *      Author: dpwoo
 */
#include <stdlib.h>
#include <stdio.h>
#include "rbtree.h"

struct rb_tree*
rb_tree_init()
{
	struct rb_tree *root;

	if((root = malloc(sizeof(*root))) == NULL) {
		printf("out of memory!\n");
		return NULL;
	}
	root->rb_node = NULL;
	return root;
}

static struct rb_node*
create_node(data_type data)
{
	struct rb_node *n = calloc(1, sizeof(*n));
	if(n){
		n->data = data;
		n->parent = n; /* point itself means an empty node */
	}
	return n;
}

static inline  void
destroy_node(struct rb_node *n)
{
	free(n);
}

static inline void
set_rb_parent(struct rb_node *n, struct rb_node *parent)
{
	n->parent = parent;
}

static inline void
set_rb_color(struct rb_node *n, int color)
{
	n->color = color;
}

static void
rb_rorate_left(struct rb_node *n, struct rb_tree *root)
{
	struct rb_node *right = n->right;
	struct rb_node *parent = n->parent;

	if((n->right = right->left) != NULL) {
		set_rb_parent(right->left, n);
	}

	right->left = n;
	set_rb_parent(n, right);
	set_rb_parent(right, parent);

	if(parent) {
		if(parent->left == n) {
			parent->left = right;
		} else {
			parent->right = right;
		}
	} else {
		root->rb_node = right;
	}
}

static void
rb_rorate_right(struct rb_node *n, struct rb_tree *root)
{
	struct rb_node *left = n->left;
	struct rb_node *parent = n->parent;

	if((n->left = left->right)) {
		set_rb_parent(left->right, n);
	}

	left->right = n;
	set_rb_parent(n, left);
	set_rb_parent(left, parent);

	if(parent) {
		if(parent->left == n) {
			parent->left = left;
		} else {
			parent->right = left;
		}
	} else {
		root->rb_node = left;
	}
}

static inline struct rb_node*
get_uncle_node(struct rb_node *gparent, struct rb_node *parent)
{
	return gparent->left == parent ? gparent->right : gparent->left;
}

static void
rb_tree_insert_rebalance(struct rb_tree *root, struct rb_node *n)
{
	struct rb_node *parent, *gparent, *uncle;

	while((parent = n->parent) && parent->color == RB_COLOR_RED) {
		/* because parent is red, so gparent never be null. */
		gparent = parent->parent;
		uncle = get_uncle_node(gparent, parent);

		if(uncle && uncle->color == RB_COLOR_RED) {
			set_rb_color(parent, RB_COLOR_BLACK);
			set_rb_color(uncle, RB_COLOR_BLACK);
			set_rb_color(gparent, RB_COLOR_RED);
			n = gparent; /* backtrace to root */
			continue;
		}

		if(gparent->left == parent) {
			if(parent->right == n) { /* left&right rotate case */
				rb_rorate_left(parent, root);
				struct rb_node *tmp = parent;
				parent = n;
				n = tmp;
			}
			rb_rorate_right(gparent, root);
			set_rb_color(gparent, RB_COLOR_RED);
			set_rb_color(parent, RB_COLOR_BLACK); /* parent black, done. */
		} else {
			if(parent->left == n) { /* right&left rotate case */
				rb_rorate_right(parent, root);
				struct rb_node *tmp = parent;
				parent = n;
				n = tmp;
			}
			rb_rorate_left(gparent, root);
			set_rb_color(gparent, RB_COLOR_RED);
			set_rb_color(parent, RB_COLOR_BLACK); /* parent black, done. */
		}
	}
	set_rb_color(root->rb_node, RB_COLOR_BLACK); /* root always set black in case uncle is red case. */
}

int
rb_tree_insert(struct rb_tree *root, data_type data)
{
	struct rb_node *n, *parent = NULL, **cur = &root->rb_node;

	while(*cur) {
		parent = *cur;
		if((*cur)->data > data) {
			cur = &(*cur)->left;
		} else if((*cur)->data < data) {
			cur = &(*cur)->right;
		} else {
			return 0;
		}
	}

	if((n = create_node(data)) == NULL) {
		return -1;
	}
	n->parent = parent;
	*cur = n;

	rb_tree_insert_rebalance(root, n);

	return 0;
}

static void
rb_tree_delete_rebalance(struct rb_tree *root, struct rb_node *parent, struct rb_node *node)
{
	struct rb_node *bro;

	/* According to <<Introduction to Algorithms>>, null means black color */
	while((!node || node->color == RB_COLOR_BLACK) && node != root->rb_node) {
		if(parent->left == node) {
			bro = parent->right;
			if(bro->color == RB_COLOR_RED) {// case 1
				set_rb_color(parent, RB_COLOR_RED);
				set_rb_color(bro, RB_COLOR_BLACK);
				rb_rorate_left(parent, root);
				bro = parent->right;
			} else if((!bro->left || bro->left->color == RB_COLOR_BLACK) &&
					(!bro->right || bro->right->color == RB_COLOR_BLACK)) {//case 2
				set_rb_color(bro, RB_COLOR_RED);
				node = parent;
				parent = parent->parent;
			} else {
				if(!bro->right || bro->right->color == RB_COLOR_BLACK) {// case 3
					set_rb_color(bro->left, RB_COLOR_BLACK);
					set_rb_color(bro, RB_COLOR_RED);
					rb_rorate_right(bro, root);
					bro = parent->right;
				}//case 4
				set_rb_color(bro, parent->color);
				set_rb_color(parent, RB_COLOR_BLACK);
				set_rb_color(bro->right, RB_COLOR_BLACK);
				rb_rorate_left(parent, root);
				node = root->rb_node;//quit
			}
		} else {
			bro = parent->left;
			if(bro->color == RB_COLOR_RED) {// case 1
				set_rb_color(parent, RB_COLOR_RED);
				set_rb_color(bro, RB_COLOR_BLACK);
				rb_rorate_right(parent, root);
				bro = parent->left;
			} else if((!bro->left || bro->left->color == RB_COLOR_BLACK) &&
					(!bro->right || bro->right->color == RB_COLOR_BLACK)) { //case 2
				set_rb_color(bro, RB_COLOR_RED);
				node = parent;
				parent = parent->parent;
			} else {
				if(!bro->left || bro->left->color == RB_COLOR_BLACK) {// case 3
					set_rb_color(bro->right, RB_COLOR_BLACK);
					set_rb_color(bro, RB_COLOR_RED);
					rb_rorate_left(bro, root);
					bro = parent->left;
				}//case 4
				set_rb_color(bro, parent->color);
				set_rb_color(parent, RB_COLOR_BLACK);
				set_rb_color(bro->left, RB_COLOR_BLACK);
				rb_rorate_right(parent, root);
				node = root->rb_node;//quit
			}
		}
	}
	if(node) {
		set_rb_color(node, RB_COLOR_BLACK);
	}
}

int
rb_tree_delete(struct rb_tree *root, data_type data)
{
	int color = RB_COLOR_RED;
	struct rb_node *find;
	struct rb_node **cur = &root->rb_node;
	struct rb_node *parent = NULL, *child = NULL;

	while(*cur) {
		if((*cur)->data == data) {
			break;
		} else if((*cur)->data > data) {
			cur = &(*cur)->left;
		} else {
			cur = &(*cur)->right;
		}
	}

	if(!*cur) {
		return -1;
	}
	find = *cur;
	parent = find->parent;
	child = find->right ? find->right : find->left;
	color = find->color;

	if(find->right && find->left){
		struct rb_node *n = find->right;
		struct rb_node *del = n;
		while(del->left) {
			del = del->left;
		}
		if(del == n) { // have no left child
			if((del->parent->right = del->right)) {
				del->right->parent = del->parent;
			}
		} else {
			if((del->parent->left = del->right)) {
				del->right->parent = del->parent;
			}
		}
		find->data = del->data;
		color = del->color;
		parent = del->parent;
		child = del->right;
		free(del);
	} else {
		if(find->left) {
			if((*cur = (*cur)->left)) {
				find->left->parent = find->parent;
			}
		} else {
			if((*cur = (*cur)->right)) {
				find->right->parent = find->parent;
			}
		}
		free(find);
	}

	if(color == RB_COLOR_BLACK) {
		rb_tree_delete_rebalance(root, parent, child);
	}

	return 0;
}

struct rb_node*
rb_tree_search(struct rb_tree *root, data_type data)
{
	struct rb_node **cur = &root->rb_node;

	while(*cur) {
		if((*cur)->data == data) {
			break;
		} else if((*cur)->data > data) {
			cur = &(*cur)->left;
		} else {
			cur = &(*cur)->right;
		}
	}
	return *cur;
}

struct rb_node*
rb_tree_first(struct rb_tree *root)
{
	struct rb_node *cur = root->rb_node;

	if(cur) {
		while(cur->left) {
			cur = cur->left;
		}
	}
	return cur;
}

struct rb_node*
rb_tree_next(struct rb_node *node)
{
	if(node->right) {
		node = node->right;
		while(node->left) {
			node = node->left;
		}
		return node;
	}
	while(node->parent && node->parent->right == node) {
		node =  node->parent;
	}
	return node->parent;
}

struct rb_node*
rb_tree_prev(struct rb_node *node)
{
	if(node->left) {
		node = node->left;
		while(node->right) {
			node = node->right;
		}
		return node;
	}
	while(node->parent && node->parent->left == node) {
		node = node->parent;
	}
	return node->parent;
}

struct rb_node*
rb_tree_last(struct rb_tree *root)
{
	struct rb_node *cur = root->rb_node;

	if(cur) {
		while(cur->right) {
			cur = cur->right;
		}
	}
	return cur;
}

static void
rb_tree_destroy_inner(struct rb_node *node)
{
	if(node) {
		rb_tree_destroy_inner(node->left);
		rb_tree_destroy_inner(node->right);
		printf("del%d%s\t", node->data, node->color ? "+B" : "+R");
		free(node);
	}
}

void
rb_tree_destroy(struct rb_tree *root)
{
	rb_tree_destroy_inner(root->rb_node);
	free(root);
}

/* end of file */
