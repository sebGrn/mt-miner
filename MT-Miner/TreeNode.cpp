#include "TreeNode.h"

TreeNode::TreeNode(const std::string _name)
{
	name = _name;
}

TreeNode::~TreeNode()
{
}

void TreeNode::addChild(std::shared_ptr<TreeNode>& _child)
{
	children.push_back(_child);
}

std::string TreeNode::getParentName()
{
	std::string _name = name;
	// if the node is a leaf
	if (_name.at(0) == '*')
	{
		_name = _name.substr(1, _name.length() - 2);
	}

	// set name to root if name doesn't contain ','
	if (_name.find(",") == std::string::npos)
	{
		_name = "";
	}
	else
	{
		// cut the last part
		_name = _name.substr(0, _name.find_last_of(","));
	}
	return _name;
}

std::string TreeNode::getName()
{
	return name;
}

std::string TreeNode::getTreeStructure()
{
	// beginning structure
	std::string structure = "";
	// add name in JSON format 
	structure.append("{ \"name\": \"" + name + "\"");
	if (!children.empty())
	{
		// add children in JSON format
		structure.append(", \"children\": [");
		// first child
		auto it = children.begin();
		for_each(children.begin(), children.end(), [&](std::shared_ptr<TreeNode>& _node)
		{ 
			structure.append(_node->getTreeStructure());
			// next child
			it++;
			// add , if there are others children behind
			if (it != children.end())
			{
				structure.append(",");
			}
		});
		// close children
		structure.append("]");
	}
	// close name 
	structure.append("}");

	return structure;
}