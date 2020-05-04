#include "TreeNode.h"
#include <algorithm>

TreeNode::TreeNode(const std::string& _name)
{
	name = _name;
}

TreeNode::~TreeNode()
{
}

void TreeNode::addChild(TreeNode& _child)
{
	std::string _name = _child.getName();
	if (_name.substr(0, _name.find_last_of(",")) == this->getName())
	{
		children.push_back(_child);
	}
	else
	{
		for_each(children.begin(), children.end(), [&](TreeNode& node) { node.addChild(_child); });
	}
}

std::string TreeNode::getName()
{
	return name;
}

std::string TreeNode::getTreeStructure()
{
	std::string structure = "";
	structure.append("{ \"name\": \"" + this->getName() + "\"");
	if (!children.empty())
	{
		structure.append(", \"children\": [");
		auto it = children.begin();
		for_each(children.begin(), children.end(), [&](TreeNode& _node)
		{ 
			structure.append(_node.getTreeStructure());
			it++;
			if (it != children.end())
			{
				structure.append(",");
			}
		});
		structure.append("]");
	}
	structure.append("}");

	return structure;
}