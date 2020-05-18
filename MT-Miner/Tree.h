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

	/// tree structure <level, nodes>
	std::map<int, std::vector<std::shared_ptr<TreeNode>>> structure;
public:
	~Tree();

	/// get or create the tree
	/// @return tree
	static std::shared_ptr<Tree> getTree();

	/// add child to the root
	/// @params child node
	void addRoot(std::shared_ptr<TreeNode>& _root);

	/// add child to the tree 
	/// @params child node
	void addChild(std::shared_ptr<TreeNode>& _child, int _level);

	/// add or create branch or leaf to the tree
	/// @params child node
	void addLeaf(std::shared_ptr<TreeNode>& _leaf, int _level);

	void buildStructure(std::shared_ptr<TreeNode>& _node, int _level);

	/// save JSON tree structure into a file
	/// @params name of the file
	void saveJSONTree(std::string _filename);
};

