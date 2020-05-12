#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

class JsonTreeNode
{
private:
	/// JSON name
	std::string name;

	std::vector<std::shared_ptr<JsonTreeNode>> children;

public:
	JsonTreeNode(const std::string& _name)
	{
		name = _name;
	};

	~JsonTreeNode()
	{};

	/// add _root node to the children 
	/// @params child node
	void addRoot(const std::shared_ptr<JsonTreeNode>& _root)
	{
		this->children.push_back(_root);
	};

	/// add _child to the children if it has the good parent name
	/// @params child node
	void addChild(const std::shared_ptr<JsonTreeNode>& _child)
	{
		std::string _name = _child->getParentName();
		// add child if the parent name is the current node name
		if (_name == name)
		{
			children.push_back(_child);
		}
		else
		{
			// try to add child for each child in children
			for_each(children.begin(), children.end(), [&](std::shared_ptr<JsonTreeNode>& node) { node->addChild(_child);  });
		}
	};

	/// recursive function 
	/// @params node name, current node
	/// @return true if the name of the _node is _name
	bool findTreeNode(const std::string& _name, JsonTreeNode& _node) 
	{
		// return true when the names are corresponding
		if (_node.getName() == _name)
		{
			return true;
		}
		// try to find node in the children 
		bool result = false;
		for (int i = 0; result == false && i < _node.children.size(); i++)
		{
			std::shared_ptr<JsonTreeNode> _child = _node.children.at(i);
			JsonTreeNode* _child_ptr = _child.get();
			result = findTreeNode(_name, *_child_ptr);
		}
		return result;
	};

	/// recursive function create parent node if not exist and add it
	/// @params child node
	void addLeaf(const std::shared_ptr<JsonTreeNode>& _leaf)
	{
		std::string _name = _leaf->getParentName();
		// add child if the parent node already exist
		if (findTreeNode(_name, *this))
		{
			addChild(_leaf);
		}
		// create parent node, add _leaf to his children and try to add parent node as leaf
		else
		{
			std::shared_ptr<JsonTreeNode> parent = std::make_shared<JsonTreeNode>(_name);
			parent->addRoot(_leaf);
			addLeaf(parent);
		}
	};

	std::string getName()
	{
		return name;
	};

	/// cut the node name to get the parent name
	/// @return parent name
	std::string getParentName() const
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
	};

	std::vector<std::shared_ptr<JsonTreeNode>> getChildren() const
	{
		return this->children;
	};

	/// recursive function
	/// @return JSON structure 
	std::string getTreeStructure() 
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
			for_each(children.begin(), children.end(), [&](std::shared_ptr<JsonTreeNode>& _node)
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
	};
};

