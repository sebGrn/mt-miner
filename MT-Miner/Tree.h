#pragma once
#include <iostream>
#include <fstream>
#include <algorithm>
#include <map>

#include "TreeNode.h"

/// front-end tree
class Tree
{
private:
	Tree();

	/// unique tree
	static std::shared_ptr<Tree> tree;

	/// root 
	std::shared_ptr<TreeNode> root;

	/// tree structure <name, node>
	std::map<std::string, std::shared_ptr<TreeNode>> structure;
public:
	~Tree();

	/// get or create the tree
	/// @return tree
	static std::shared_ptr<Tree> getTree();

	/// add _child to the tree 
	/// @params child node
	void addChild(std::shared_ptr<TreeNode>& _child);

	/// add _node to the tree structure
	/// @params tree node
	void buildStructure(std::shared_ptr<TreeNode>& _node);

	/// save JSON tree structure into a file
	/// @params name of the file
	void saveJSONTree(std::string _filename);
};

