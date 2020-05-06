#include "Tree.h"

std::shared_ptr<Tree> Tree::tree(nullptr);

Tree::Tree()
{
    root = std::make_shared<TreeNode>("");
}

Tree::~Tree()
{
}

std::shared_ptr<Tree> Tree::getTree()
{
    if (tree == nullptr)
    {
        tree = std::shared_ptr<Tree>(new Tree());
    }
    return tree;
}

void Tree::addRoot(TreeNode& _root)
{
    root->addRoot(_root);
}

void Tree::addChild(TreeNode& _child)
{
    root->addChild(_child);
}

void Tree::saveJSONTree(std::string& _filename)
{
    std::cout << "saving JSON tree in : " << _filename << std::endl;
    std::ofstream outputStream;
    outputStream.open(_filename);
    outputStream << root->getTreeStructure();
    outputStream.close();
}