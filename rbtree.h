/*
 * rbtree.h
 *
 *  Created on: 2014-12-12
 *      Author: dpwoo
 */

#ifndef RBTREE_H_
#define RBTREE_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef int data_type;

#define RB_COLOR_RED 0
#define RB_COLOR_BLACK 1

struct rb_node {
	struct rb_node *parent;
	struct rb_node *left, *right;
	unsigned int color;
	data_type data;
};

struct rb_tree {
	struct rb_node *rb_node;
};

struct rb_tree*
rb_tree_init();

int
rb_tree_insert(struct rb_tree *root, data_type data);

int
rb_tree_delete(struct rb_tree *root, data_type data);

struct rb_node*
rb_tree_search(struct rb_tree *root, data_type data);

void
rb_tree_destroy(struct rb_tree *root);

struct rb_node*
rb_tree_first(struct rb_tree *root);

struct rb_node*
rb_tree_next(struct rb_node *node);

struct rb_node*
rb_tree_prev(struct rb_node *node);

struct rb_node*
rb_tree_last(struct rb_tree *root);

#ifdef __cplusplus
extern "C" }
#endif
#endif /* RBTREE_H_ */
