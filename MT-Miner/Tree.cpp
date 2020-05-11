#include "Tree.h"

// initialize static tree
std::shared_ptr<Tree> Tree::tree(nullptr);

Tree::Tree()
{
    // create a new TreeNode as root 
    // /!\ name "" is important 
    root = std::make_shared<TreeNode>("");
}

Tree::~Tree()
{
}

std::shared_ptr<Tree> Tree::getTree()
{
    // create a tree if it doesn't exist
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

void Tree::addLeaf(TreeNode& _leaf)
{
    root->addLeaf(_leaf);
}

void Tree::saveJSONTree(std::string _filename)
{
    // display 
    std::cout << "saving JSON tree in : " << _filename << std::endl;
    std::ofstream outputStream;
    outputStream.open(_filename);
    // save the structure of the tree
    outputStream << root->getTreeStructure();
    outputStream.close();
}