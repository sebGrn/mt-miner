#include "Tree.h"
#include <iostream>
#include <fstream>

Tree* Tree::tree = nullptr;

Tree::Tree()
{
    root = nullptr;
}

Tree::~Tree()
{
}

Tree* Tree::getTree()
{
    if (tree == nullptr)
    {
        tree = new Tree();
    }
    return tree;
}

void Tree::setRoot(TreeNode* _root)
{
    root = _root;
}

void Tree::addChild(TreeNode& _child)
{
    root->addChild(_child);
}

void Tree::saveJSONTree(std::string& _filename)
{
    if (!_filename.empty())
    {
        std::cout << "saving JSON tree in : " << _filename << std::endl;
        std::ofstream outputStream;
        outputStream.open(_filename);
        outputStream << root->getTreeStructure();
        outputStream.close();
    }
}