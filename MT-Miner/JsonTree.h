#pragma once

#include <memory>
#include <iostream>
#include <fstream>
#include <algorithm>

#include "Logger.h"
#include "JsonTreeNode.h"
#include "utils.h"

/// front-end tree
class JsonTree
{
private:
	/// unique tree
	static std::shared_ptr<JsonTree> tree;

	/// root of the tree to call TreeNode functions
	std::shared_ptr<JsonTreeNode> root;

private:
	JsonTree()
	{
		// create a new TreeNode as root 
		// /!\ name "" is important 
		this->root = std::make_shared<JsonTreeNode>("");
	};

public:
	~JsonTree()
	{};

	/// get or create the tree
	/// @return tree
	static std::shared_ptr<JsonTree> getTree()
	{
		// create a tree if it doesn't exist
		if (tree == nullptr)
		{
			tree = std::shared_ptr<JsonTree>(new JsonTree());
		}
		return tree;
	}

	// build front-end tree
	static void addJsonNode(const ItemsetList& toExplore)
	{
		std::shared_ptr<JsonTree> tree = JsonTree::getTree();
		for_each(toExplore.begin(), toExplore.end(), [&](const Itemset& _item)
		{
			std::string name = Utils::itemsetToString(_item);
			// remove {}
			name = name.substr(1, name.length() - 2);
			// create node with name
			std::shared_ptr<JsonTreeNode> node = std::make_shared<JsonTreeNode>(name);
			// add to the tree root if there is only one element
			if (_item.size() == 1)
			{
				tree->addRoot(node);
			}
			else
			{
				tree->addChild(node);
			}
		});
	}

	static void writeJsonNode(const ItemsetList& minimalTransversals)
	{
		std::shared_ptr<JsonTree> tree = JsonTree::getTree();
		//std::cout << "create front-end json tree" << std::endl;
		for_each(minimalTransversals.begin(), minimalTransversals.end(), [&](const Itemset& elt)
		{
			// add front-end tree leaves
			std::string name = Utils::itemsetToString(elt);
			// remove {}
			name = name.substr(1, name.length() - 2);
			// indicate leaf
			name = '*' + name + '*';
			// create node with name
			std::shared_ptr<JsonTreeNode> node = std::make_shared<JsonTreeNode>(name);
			// create or add to branch tree
			tree->addLeaf(node);
		});
		// save front-end tree
		std::string _filename = "../frontend/tree-data.json";
		tree->saveJSONTree(_filename);
	};

	/// add child to the root
	/// @params child node
	void addRoot(const std::shared_ptr<JsonTreeNode>& _root)
	{
		root->addRoot(_root);
	};

	/// add child to the tree 
	/// @params child node
	void addChild(const std::shared_ptr<JsonTreeNode>& _child)
	{
		root->addChild(_child);
	};

	/// add or create branch or leaf to the tree
	/// @params child node
	void addLeaf(const std::shared_ptr<JsonTreeNode>& _leaf)
	{
		root->addLeaf(_leaf);
	};

	/// save JSON tree structure into a file
	/// @params name of the file
	void saveJSONTree(const std::string& _filename)
	{
		// display 
		std::cout << GREEN << "saving json tree in : " << _filename << RESET << std::endl;
		std::ofstream outputStream;
		outputStream.open(_filename);
		// save the structure of the tree
		outputStream << root->getTreeStructure();
		outputStream.close();
	};
};
