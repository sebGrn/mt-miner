#include "Tree.h"

// initialize static tree
std::shared_ptr<Tree> Tree::tree(nullptr);

Tree::Tree()
{
    std::vector<std::shared_ptr<TreeNode>> v;
    v.push_back(std::make_shared<TreeNode>(""));
    structure.insert(std::pair<int, std::vector<std::shared_ptr<TreeNode>>>(0, v));
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

void Tree::addRoot(std::shared_ptr<TreeNode>& _root)
{
    structure.at(0).at(0)->addRoot(_root);
}

void Tree::buildStructure(std::shared_ptr<TreeNode>& _node, int _level)
{
    if (structure.size() - 1 < _level)
    {
        std::vector<std::shared_ptr<TreeNode>> c;
        c.push_back(_node);
        structure.insert(std::pair<int, std::vector<std::shared_ptr<TreeNode>>>(_level, c));
    }
    else
    {
        structure.at(_level).push_back(_node);
    }
}

void Tree::addChild(std::shared_ptr<TreeNode>& _child, int _level)
{
    std::vector<std::shared_ptr<TreeNode>> v = structure.at(_level - 1);
    for_each(v.begin(), v.end(), [&](std::shared_ptr<TreeNode>& _node)
        {
            _node->addChild(_child);
        });
    buildStructure(_child, _level);
}

void Tree::addLeaf(std::shared_ptr<TreeNode>& _leaf, int _level)
{
    std::vector<std::shared_ptr<TreeNode>> v = structure.at(_level - 1);
    bool added = false;
    for_each(v.begin(), v.end(), [&](std::shared_ptr<TreeNode>& _node)
        {
            if (_node->getName() == _leaf->getParentName())
            {
                _node->addRoot(_leaf);
                added = true;
            }
        });
    if (!added)
    {
        std::shared_ptr<TreeNode> parent = std::make_shared<TreeNode>(_leaf->getParentName());
        parent->addRoot(_leaf);
        addLeaf(parent, _level - 1);
        
    }
    buildStructure(_leaf, _level);
}

void Tree::saveJSONTree(std::string _filename)
{
    // display 
    std::cout << "saving json tree in : " << _filename << std::endl;
    std::ofstream outputStream;
    outputStream.open(_filename);
    // save the structure of the tree
    structure.at(0).at(0) = structure.at(0).at(0);
    outputStream << structure.at(0).at(0)->getTreeStructure();
    outputStream.close();
}