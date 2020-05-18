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

	/// add _child to the children
	/// @params child node
	void addChild(std::shared_ptr<TreeNode>& _child);

	std::string getName();

	/// cut the node name to get the parent name
	/// @return parent name
	std::string getParentName();

	/// recursive function
	/// @return JSON structure 
	std::string getTreeStructure();
};

