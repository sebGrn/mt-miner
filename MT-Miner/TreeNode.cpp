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
	// add child if the parent name is the current node name
	if (_name == name)
	{
		children.push_back(_child);
	}
	else
	{
		// try to add child for each child in children
		for_each(children.begin(), children.end(), [&](TreeNode& node) { node.addChild(_child); });
	}
}

bool TreeNode::findTreeNode(std::string _name, TreeNode& _node)
{
	// return true when the names are corresponding
	if (_node.getName() == _name)
	{
		return true;
	}
	// try to find node in the children 
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

void TreeNode::addLeaf(TreeNode& _leaf)
{
	std::string _name = _leaf.getParentName();
	// add child if the parent node already exist
	if (findTreeNode(_name, *this))
	{
		addChild(_leaf);
	}
	// create parent node, add _leaf to his children and try to add parent node as leaf
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
		for_each(children.begin(), children.end(), [&](TreeNode& _node)
		{ 
			structure.append(_node.getTreeStructure());
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