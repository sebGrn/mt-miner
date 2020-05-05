#pragma once
#include "TreeNode.h"

class Tree
{
private:
	Tree();
	static Tree* tree;
	TreeNode* root;
public:
	~Tree();
	static Tree* getTree();
	void addRoot(TreeNode& _root);
	void addChild(TreeNode& _child);
	void saveJSONTree(std::string& _filename);
};

