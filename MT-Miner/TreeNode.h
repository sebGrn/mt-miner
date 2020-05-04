#pragma once
#include <string>
#include <vector>

class TreeNode
{
private:
	std::string name;
	std::vector<TreeNode> children;
public:
	TreeNode(const std::string& _name);
	~TreeNode();
	void addChild(TreeNode& _child);
	std::string getName();
	std::string getTreeStructure();
};

