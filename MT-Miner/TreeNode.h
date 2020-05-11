#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

class TreeNode
{
private:
	std::string name;
	std::vector<TreeNode> children;
public:
	TreeNode(const std::string _name);
	~TreeNode();
	void addRoot(TreeNode& _root);
	void addChild(TreeNode& _child);
	bool findTreeNode(std::string _name, TreeNode& _node);
	void addLeaf(TreeNode& _leaf);
	std::string getName();
	std::string getParentName();
	std::vector<TreeNode> getChildren();
	std::string getTreeStructure();
};

