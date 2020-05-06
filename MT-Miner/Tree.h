#pragma once
#include <iostream>
#include <fstream>
#include <algorithm>

#include "TreeNode.h"

class Tree
{
private:
	Tree();
	static std::shared_ptr<Tree> tree;
	std::shared_ptr<TreeNode> root;
public:
	~Tree();
	static std::shared_ptr<Tree> getTree();
	void addRoot(TreeNode& _root);
	void addChild(TreeNode& _child);
	void saveJSONTree(std::string& _filename);
};

