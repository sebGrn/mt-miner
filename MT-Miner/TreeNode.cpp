#include "TreeNode.h"

TreeNode::TreeNode(const std::string _name)
{
	name = _name;
}

TreeNode::~TreeNode()
{
}

void TreeNode::addRoot(TreeNode& _root)
{
	children.push_back(_root);
}

void TreeNode::addChild(TreeNode& _child)
{
	std::string _name = _child.getParentName();
	if (_name == name)
	{
		children.push_back(_child);
	}
	else
	{
		for_each(children.begin(), children.end(), [&](TreeNode& node) { node.addChild(_child); });
	}
}

bool TreeNode::findTreeNode(std::string _name, TreeNode& _node)
{
	if (_node.getName() == _name)
	{
		return true;
	}
	bool result = false;
	for (int i = 0; result == false && i < _node.getChildren().size(); i++) 
	{
		result = findTreeNode(_name, _node.getChildren().at(i));
	}
	return result;
}

std::string TreeNode::getParentName()
{
	std::string _name = name;
	if (_name.at(0) == '*')
	{
		_name = _name.substr(1, _name.length() - 2);
	}

	if (_name.find(",") == std::string::npos)
	{
		_name = "";
	}
	else
	{
		_name = _name.substr(0, _name.find_last_of(","));
	}
	return _name;
}

void TreeNode::addLeaf(TreeNode& _leaf)
{
	std::string _name = _leaf.getParentName();

	if (findTreeNode(_name, *this))
	{
		addChild(_leaf);
	}
	else
	{
		std::shared_ptr<TreeNode> parent = std::make_shared<TreeNode>(_name);
		parent->addRoot(_leaf);
		addLeaf(*parent);
	}
}

std::string TreeNode::getName()
{
	return name;
}

std::vector<TreeNode> TreeNode::getChildren()
{
	return children;
}

std::string TreeNode::getTreeStructure()
{
	std::string structure = "";
	structure.append("{ \"name\": \"" + name + "\"");
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