#include "Tree.h"

// initialize static tree
std::shared_ptr<Tree> Tree::tree(nullptr);

Tree::Tree()
{
    // create the root node
    root = std::make_shared<TreeNode>("");
    // initialize the tree structure
    buildStructure(root);
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

void Tree::buildStructure(std::shared_ptr<TreeNode>& _node)
{
    structure.insert(std::pair<std::string, std::shared_ptr<TreeNode>>(_node->getName(), _node));
}

void Tree::addChild(std::shared_ptr<TreeNode>& _child)
{
    auto it = structure.find(_child->getParentName());
    if (it != structure.end())
    {
        it->second->addChild(_child);
    }
    else
    {
        std::shared_ptr<TreeNode> parent = std::make_shared<TreeNode>(_child->getParentName());
        parent->addChild(_child);
        addChild(parent);
    }
    buildStructure(_child);
}

void Tree::saveJSONTree(std::string _filename)
{
    // display 
    std::cout << "saving json tree in : " << _filename << std::endl;
    std::ofstream outputStream;
    outputStream.open(_filename);
    // save the structure of the tree
    outputStream << root->getTreeStructure();
    outputStream.close();
}