#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

class TreeNode
{
private:
	/// JSON name
	std::string name;

	std::vector<std::shared_ptr<TreeNode>> children;
public:
	TreeNode(const std::string _name);
	~TreeNode();

	/// add _root node to the children 
	/// @params child node
	void addRoot(std::shared_ptr<TreeNode>& _root);

	/// add _child to the children if it has the good parent name
	/// @params child node
	void addChild(std::shared_ptr<TreeNode>& _child);

	/// recursive function 
	/// @params node name, current node
	/// @return true if the name of the _node is _name
	bool findTreeNode(std::string _name, TreeNode& _node);

	/// recursive function create parent node if not exist and add it
	/// @params child node
	void addLeaf(std::shared_ptr<TreeNode>& _leaf);

	std::string getName();

	/// cut the node name to get the parent name
	/// @return parent name
	std::string getParentName();

	std::vector<std::shared_ptr<TreeNode>> getChildren();

	/// recursive function
	/// @return JSON structure 
	std::string getTreeStructure();
};

