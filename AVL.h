// what we need to do 
/* 
- add the find balance factor and UpdateHeight class methods 
- bascially update all nodes' hight and balance factors 
    - and we do a method for that. (in avl) 
- for pointers: 
    to do is to look at the BST stuff to see how it works 

- the only time that we care about lock is when wee have to move up to the parent node and find 
    the balance factor or UpdateHeight of that parent node 
            - so like to search the AVL tree up basically 

*/

#include <memory>
#include <string>

class AVL;

class AVLNode {
 public:
 	AVLNode(int key);
 	AVLNode(int key, std::weak_ptr<AVLNode> parent);
 	bool IsLeaf() const;
 	bool IsMissingChild() const;
 	bool HasLeftChild() const;
 	bool HasRightChild() const;
 	void DeleteChild(std::shared_ptr<AVLNode> v);
 	void ReplaceChild(std::shared_ptr<AVLNode> v, std::shared_ptr<AVLNode> u);
	int height;
  	int balance_factor;
   


 private:
  int key_;
  std::weak_ptr<AVLNode> parent_;
  std::shared_ptr<AVLNode> left_;
  std::shared_ptr<AVLNode> right_;

  friend AVL;
}; // class AVLNode

class AVL{
 public:
 	AVL();

 	void Insert(int key);
 	bool Delete(int key);
 	bool Find(int key) const;
 	std::string JSON() const;
 	size_t size() const;
 	bool empty() const;
 	int DeleteMin();
	int UpdateHeightandBalanceFactor(std::shared_ptr<AVLNode> currentNode); 
    void RotateTree(std::shared_ptr<AVLNode> currentNode); 
    void RRrotation(std::shared_ptr<AVLNode> currentNode); 
    void RLrotation(std::shared_ptr<AVLNode>currentNode);
	void LLrotation(std::shared_ptr<AVLNode>currentNode);
	void LRrotation(std::shared_ptr<AVLNode>currentNode);
    
    std::shared_ptr<AVLNode> root_;
 	size_t size_;
    
 private:
	void DeleteLeaf(std::shared_ptr<AVLNode> currentNode);
	int DeleteMin(std::shared_ptr<AVLNode> currentNode);
    


}; // class AVL