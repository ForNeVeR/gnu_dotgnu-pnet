/*
 * cg_rbtree.c - Red-black tree implementation.
 *
 * Copyright (C) 2001  Southern Storm Software, Pty Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*

This red-black tree implementation is based on that in "Algorithms in C++",
Robert Sedgewick, Addison Wesley, 1992.  

The algorithm has been modified to support duplicate keys more effectively.
Each tree node can have a linked list of duplicates attached to it.  This
avoids the unpredictable location of duplicates in traditional red-black trees.

*/

#include "cg_rbtree.h"

#ifdef	__cplusplus
extern	"C" {
#endif

void ILRBTreeInit(ILRBTree *tree, ILRBCompareFunc compareFunc,
				  ILRBFreeFunc freeFunc)
{
	tree->nil._left = &(tree->nil);
	tree->nil._right = &(tree->nil);
	tree->nil._red = 0;
	tree->nil._duplicate = 0;
	tree->nil._continue = 0;
	tree->head._left = 0;
	tree->head._right = &(tree->nil);
	tree->head._red = 0;
	tree->head._duplicate = 0;
	tree->head._continue = 0;
	tree->compareFunc = compareFunc;
	tree->freeFunc = freeFunc;
}

ILRBTreeNode *ILRBTreeSearch(ILRBTree *tree, void *key)
{
	ILRBTreeNode *node = tree->head._right;
	int cmp;
	while(node != &(tree->nil))
	{
		cmp = (*(tree->compareFunc))(key, node);
		if(cmp == 0)
		{
			return node;
		}
		else if(cmp < 0)
		{
			if(!(node->_duplicate))
			{
				node = node->_left;
			}
			else
			{
				node = node->_left->_left;
			}
		}
		else
		{
			node = node->_right;
		}
	}
	return 0;
}

ILRBTreeNode *ILRBTreeNext(ILRBTreeNode *node)
{
	if(node->_duplicate)
	{
		return node->_left;
	}
	else if(node->_continue)
	{
		return node->_right;
	}
	else
	{
		return 0;
	}
}

/*
 * Compare a key against a node, being careful of sentinel nodes.
 */
static int Compare(ILRBTree *tree, void *key, ILRBTreeNode *node)
{
	if(node == &(tree->nil) || node == &(tree->head))
	{
		/* Every key is greater than the sentinel nodes */
		return 1;
	}
	else
	{
		/* Compare a regular node */
		return (*(tree->compareFunc))(key, node);
	}
}

/*
 * Get or set the sub-trees of a node.
 */
#define	GetLeft(node)	\
			((node)->_duplicate ? (node)->_left->_left : (node)->_left)
#define	GetRight(node)	((node)->_right)
#define	SetLeft(node,value)	\
			((node)->_duplicate ? ((node)->_left->_left = (value)) : \
					((node)->_left = (value)))
#define	SetRight(node,value)	\
			((node)->_right = (value))

/*
 * Rotate a sub-tree around a specific node.
 */
static ILRBTreeNode *Rotate(ILRBTree *tree, void *key, ILRBTreeNode *around)
{
	ILRBTreeNode *child, *grandChild;
	int setOnLeft;
	if(Compare(tree, key, around) < 0)
	{
		child = GetLeft(around);
		setOnLeft = 1;
	}
	else
	{
		child = GetRight(around);
		setOnLeft = 0;
	}
	if(Compare(tree, key, child) < 0)
	{
		grandChild = GetLeft(child);
		SetLeft(child, GetRight(grandChild));
		SetRight(grandChild, child);
	}
	else
	{
		grandChild = GetRight(child);
		SetRight(child, GetLeft(grandChild));
		SetLeft(grandChild, child);
	}
	if(setOnLeft)
	{
		SetLeft(around, grandChild);
	}
	else
	{
		SetRight(around, grandChild);
	}
	return grandChild;
}

/*
 * Split a red-black tree at the current position.
 */
#define	Split()		\
			do { \
				temp->_red = 1; \
				GetLeft(temp)->_red = 0; \
				GetRight(temp)->_red = 0; \
				if(parent->_red) \
				{ \
					grandParent->_red = 1; \
					if((Compare(tree, key, grandParent) < 0) != \
							(Compare(tree, key, parent) < 0)) \
					{ \
						parent = Rotate(tree, key, grandParent); \
					} \
					temp = Rotate(tree, key, greatGrandParent); \
					temp->_red = 0; \
				} \
			} while (0)

void ILRBTreeInsert(ILRBTree *tree, ILRBTreeNode *node, void *key)
{
	ILRBTreeNode *temp;
	ILRBTreeNode *greatGrandParent;
	ILRBTreeNode *grandParent;
	ILRBTreeNode *parent;
	ILRBTreeNode *nil = &(tree->nil);
	int cmp;

	/* Search for the insert position */
	temp = &(tree->head);
	greatGrandParent = temp;
	grandParent = temp;
	parent = temp;
	while(temp != nil)
	{
		/* Adjust our ancestor pointers */
		greatGrandParent = grandParent;
		grandParent = parent;
		parent = temp;

		/* Compare the key against the current node */
		cmp = Compare(tree, key, temp);
		if(cmp == 0)
		{
			/* Create a duplicate entry for the key */
			if(temp->_duplicate)
			{
				temp = temp->_left;
				while(temp->_right != 0)
				{
					temp = temp->_right;
				}
				temp->_continue = 1;
				temp->_right = node;
				node->_left = 0;
				node->_right = 0;
				node->_red = 0;
				node->_duplicate = 0;
				node->_continue = 0;
			}
			else
			{
				temp->_duplicate = 1;
				node->_left = temp->_left;
				node->_right = 0;
				temp->_left = node;
				node->_red = 0;
				node->_duplicate = 0;
				node->_continue = 0;
			}
			return;
		}
		else if(cmp < 0)
		{
			temp = GetLeft(temp);
		}
		else
		{
			temp = GetRight(temp);
		}

		/* Do we need to split this node? */
		if(GetLeft(temp)->_red && GetRight(temp)->_red)
		{
			Split();
		}
	}

	/* Insert the new node into the current position */
	node->_left = nil;
	node->_right = nil;
	node->_red = 1;
	node->_duplicate = 0;
	node->_continue = 0;
	if(Compare(tree, key, parent) < 0)
	{
		SetLeft(parent, node);
	}
	else
	{
		SetRight(parent, node);
	}
	Split();
	tree->head._right->_red = 0;
}

static void FreeSubTree(ILRBTreeNode *node, ILRBFreeFunc freeFunc,
						ILRBTreeNode *nil)
{
	ILRBTreeNode *temp;

	/* Free the left sub-tree */
	temp = node->_left;
	if(temp != nil)
	{
		FreeSubTree(temp, freeFunc, nil);
	}

	/* Free the right sub-tree */
	temp = node->_right;
	if(temp != nil)
	{
		FreeSubTree(temp, freeFunc, nil);
	}

	/* Free the node itself */
	(*freeFunc)(node);
}

void ILRBTreeFree(ILRBTree *tree)
{
	if(tree->head._right != &(tree->nil))
	{
		if(tree->freeFunc)
		{
			FreeSubTree(tree->head._right, tree->freeFunc, &(tree->nil));
		}
		tree->head._left = 0;
		tree->head._right = &(tree->nil);
		tree->head._red = 0;
		tree->head._duplicate = 0;
		tree->head._continue = 0;
	}
}

ILRBTreeNode *ILRBTreeGetRoot(ILRBTree *tree)
{
	ILRBTreeNode *root = tree->head._right;
	if(root != &(tree->nil))
	{
		return root;
	}
	else
	{
		return 0;
	}
}

ILRBTreeNode *ILRBTreeGetLeft(ILRBTree *tree, ILRBTreeNode *node)
{
	node = GetLeft(node);
	if(node != &(tree->nil))
	{
		return node;
	}
	else
	{
		return 0;
	}
}

ILRBTreeNode *ILRBTreeGetRight(ILRBTree *tree, ILRBTreeNode *node)
{
	node = GetRight(node);
	if(node != &(tree->nil))
	{
		return node;
	}
	else
	{
		return 0;
	}
}

#ifdef	__cplusplus
};
#endif
