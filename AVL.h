#include <string>
#include "BST.h"

class AVL : public BST
{
    void updateBalanceFactor ();
    void updateHeight ();
    void rightRotation ();
    void leftRotation ();
};
