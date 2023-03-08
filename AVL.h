#include <string>
#include "BST.h"

class AVL : public BST
{
    void updateBalanceFactor ();
    void updateHeight ();
    void rightRotation ();
    void leftRotation ();

    int findTreeHeight(); 
    int findRootKey(); 
    int findTreeSize();
    int findNodeHeight();
    int findNodeBalanceFactor(); 
    int findNodeParent(); 
    int findNodeLeftChild(); 
    int findNodeRightChild(); 
    bool isNodeRoot(); 
};
