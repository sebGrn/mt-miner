#pragma once
#include <iostream>
#include <fstream>
#include <algorithm>

#include "TreeNode.h"

/// front-end tree
class Tree
{
private:
	Tree();

	/// unique tree
	static std::shared_ptr<Tree> tree;

	/// root of the tree to call TreeNode functions
	std::shared_ptr<TreeNode> root;
public:
	~Tree();

	/// get or create the tree
	/// @return tree
	static std::shared_ptr<Tree> getTree();

	/// add child to the root
	/// @params child node
	void addRoot(TreeNode& _root);

	/// add child to the tree 
	/// @params child node
	void addChild(TreeNode& _child);

	/// add or create branch or leaf to the tree
	/// @params child node
	void addLeaf(TreeNode& _leaf);

	/// save JSON tree structure into a file
	/// @params name of the file
	void saveJSONTree(std::string _filename);
};

