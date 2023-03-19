#include "AVL.h"

#include <cassert>
#include <iostream>
#include <string>
#include <queue>

#include "json.hpp"


AVLNode::AVLNode(int key) :
	key_(key),
	parent_(std::weak_ptr<AVLNode>()),
	left_(nullptr),
	right_(nullptr) {}

AVLNode::AVLNode(int key, std::weak_ptr<AVLNode> parent) :
	key_(key),
	parent_(parent),
	left_(nullptr),
	right_(nullptr) {}

bool AVLNode::IsLeaf() const {
	return left_ == nullptr && right_ == nullptr;
}

bool AVLNode::HasLeftChild() const {
	return left_ != nullptr;
}

bool AVLNode::HasRightChild() const {
	return right_ != nullptr;
}

void AVLNode::DeleteChild(std::shared_ptr<AVLNode> v) {
	if (left_ == v) {
		left_ = nullptr;
	} else if (right_ == v) {
		right_ = nullptr;
	} else {
		std::cerr << "AVLNode::DeleteChild Error: non-child passed as argument\n";
		exit(EXIT_FAILURE);
	}
}

void AVLNode::ReplaceChild(std::shared_ptr<AVLNode> v, std::shared_ptr<AVLNode> u) {
	if (left_ == u || right_ == u) {
		std::cerr << "AVLNode::ReplaceChild Error: child passed as replacement\n";
	}
	if (left_ == v) {
		left_ = u;
		u->parent_ = v->parent_;
	} else if (right_ == v) {
		right_ = u;
		u->parent_ = v->parent_;
	} else {
		std::cerr << "AVLNode::ReplaceChild Error: non-child passed as argument\n";
		exit(EXIT_FAILURE);
	}
}

AVL::AVL() : root_(nullptr), size_(0) {}

void AVL::Insert(int key) {
	if (root_ == nullptr) {

        std::cout<< "checking if the root is null" << std::endl;
		root_ = std::make_shared<AVLNode>(key);
		size_++;
		return;
	}
	std::shared_ptr<AVLNode> currentNode = root_, lastNode = nullptr;
	while (currentNode != nullptr) {
		lastNode = currentNode;
		currentNode = (key < currentNode->key_) ?
			currentNode->left_ : currentNode->right_;
	}
	if (key < lastNode->key_) {
		lastNode->left_ = std::make_shared<AVLNode>(key, lastNode);
	} else {
		lastNode->right_ = std::make_shared<AVLNode>(key, lastNode);
	}
	size_++;


    std::cout<<"node in insert" << lastNode -> key_<<std::endl; 

    UpdateHeightandBalanceFactor(root_); 
	RotateTree(lastNode); 
    // rightrotation(lastNode); 
    // leftrotation(lastNode); 
	//RIGHT HERE IS WHERE WE ADD
}

bool AVL::Delete(int key) {
	std::shared_ptr<AVLNode> currentNode = root_;
	while (currentNode != nullptr) {
		if (currentNode->key_ == key) {
			if (currentNode->IsLeaf()) {
				DeleteLeaf(currentNode);
			} else if (currentNode->left_ == nullptr) {
				assert(currentNode->right_ != nullptr);
				std::shared_ptr<AVLNode> parent = currentNode->parent_.lock();
				parent->ReplaceChild(currentNode, currentNode->right_);
				size_--; assert(size_ >= 0);
			} else if (currentNode->right_ == nullptr) {
				assert(currentNode->left_ != nullptr);
				std::shared_ptr<AVLNode> parent = currentNode->parent_.lock();
				parent->ReplaceChild(currentNode, currentNode->left_);
				size_--; assert(size_ >= 0);
			} else {
				currentNode->key_ = DeleteMin(currentNode);
			}
		}
		currentNode = (key < currentNode->key_) ?
			currentNode->left_ : currentNode->right_;
	}
	return false;
}

int AVL::DeleteMin() {
	return DeleteMin(root_);
}


void AVL::DeleteLeaf(std::shared_ptr<AVLNode> currentNode) {
	std::shared_ptr<AVLNode> parent = currentNode->parent_.lock();
	if (parent == nullptr) {
		// Delete root
		root_ = nullptr;
		size_--; assert(size_ == 0);
	} else {
		if (parent->right_ == currentNode) {
			parent->right_ = nullptr;
		} else if (parent->left_ == currentNode) {
			parent->left_ = nullptr;
		} else {
			std::cerr << "AVL::DeleteLeaf Error: inconsistent state\n";
		}
		size_--; assert(size_ >= 0);
	}
}

int AVL::DeleteMin(std::shared_ptr<AVLNode> currentNode) {
	std::shared_ptr<AVLNode> lastNode = nullptr;
	while (currentNode != nullptr) {
		lastNode = currentNode;
		currentNode = currentNode->left_;
	}
	int result = lastNode->key_;
	std::shared_ptr<AVLNode> parent = lastNode->parent_.lock();
	if (parent == nullptr) {
		// lastNode is root
		if (lastNode->right_ != nullptr) {
			root_ = lastNode->right_;
			lastNode->right_->parent_.reset();
		} else {
			root_ = nullptr;
		}
	} else {
		// lastNode under the root
		if (lastNode->right_ != nullptr) {
			parent->left_ = lastNode->right_;
			lastNode->right_->parent_ = parent;
		} else {
			parent->left_ = nullptr;
		}
  }
	size_--; assert(size_ >= 0);
	return result;
}

size_t AVL::size() const {
	return size_;
}

bool AVL::empty() const {
	return size_ == 0;
}

bool AVL::Find(int key) const {
	std::shared_ptr<AVLNode> currentNode = root_;
	while (currentNode != nullptr) {
		if (currentNode->key_ == key) {
			return true;
		}
		currentNode = (key < currentNode->key_) ?
			currentNode->left_ : currentNode->right_;
	}
	return false;
}

std::string AVL::JSON() const {
	nlohmann::json result;
	std::queue< std::shared_ptr<AVLNode> > nodes;
	if (root_ != nullptr) {
		result["root"] = root_->key_;
		nodes.push(root_);
		while (!nodes.empty()) {
			auto v = nodes.front();
			nodes.pop();
			std::string key = std::to_string(v->key_);
			if (v->left_ != nullptr) {
				result[key]["left"] = v->left_->key_;
				nodes.push(v->left_);
			}
			if (v->right_ != nullptr) {
				result[key]["right"] = v->right_->key_;
				nodes.push(v->right_);
			}
			if (v->parent_.lock() != nullptr) {
				result[key]["parent"] = v->parent_.lock()->key_;
			} else {
				result[key]["root"] = true;
			}
		}
	}
	result["size"] = size_;
	return result.dump(2) + "\n";
}
int AVL :: UpdateHeightandBalanceFactor (std::shared_ptr<AVLNode> node)
{
	if(node == nullptr)
	{
		return -1; 
	}
	if(node -> IsLeaf())
	{
		return 0; 
	}
	else
	{
		int rightHeight = UpdateHeightandBalanceFactor(node->right_); 
		int leftHeight = UpdateHeightandBalanceFactor(node->left_); 
		node -> height = std::max(rightHeight, leftHeight) ; 
		node -> balance_factor = (rightHeight - leftHeight) + 1; 
	}
	return node -> height;
}

void AVL :: RotateTree(std::shared_ptr<AVLNode> node)
{
	if (node -> balance_factor == 2)
	{
		if (node -> right_ -> balance_factor == 1)
		{
			LLrotation(node); 
		}
		if (node -> right_ -> balance_factor == -1)
		{
			LRrotation(node);  
		}
	}
	if(node -> balance_factor == -2)
	{
		if(node->left_->balance_factor == 1)
		{
			RLrotation(node); 
		}
		if(node -> left_->balance_factor == -1)
		{
			RRrotation(node); 
		}
	}
	
	RotateTree(node -> parent_.lock()); 
}
void AVL :: RRrotation (std::shared_ptr<AVLNode> node)
{
	
		std::shared_ptr<AVLNode> y = node -> left_; //left child of node 
		std::shared_ptr<AVLNode> parent = node -> parent_.lock(); 

		if(y -> right_ != nullptr)
		{
			std::shared_ptr<AVLNode> y_ = y->right_; 
			node->left_ = y_; 
			y -> right_ = node; 
			parent -> left_ = y; 
			y -> parent_ = parent; 
			node -> parent_ = y; 
			y_ -> parent_ = node; 
		}
		else
		{
			y -> right_ = node;
			node -> parent_ = y; 
			parent -> left_ = y; 
			y -> parent_ = parent; 
		}

		UpdateHeightandBalanceFactor(root_); 
}


void AVL :: LLrotation (std::shared_ptr<AVLNode> node)
{
	
		std::shared_ptr<AVLNode> y = node -> right_; //right child of node 
		std::shared_ptr<AVLNode> parent = node -> parent_.lock(); 

		if(y -> left_ != nullptr)
		{
			std::shared_ptr<AVLNode> y_ = y->left_; 
			node->right_ = y_; 
			y -> left_ = node; 
			parent -> right_ = y; 
			node -> parent_ = y; 
			y -> parent_ = parent; 
			y_ -> parent_ = node; 
		}
		else
		{
			y -> left_ = node; 
			node -> parent_ = y; 
			parent -> right_ = y; 
			y -> parent_ = parent; 
		}

		UpdateHeightandBalanceFactor(root_); 
	
}

void AVL::LRrotation(std::shared_ptr<AVLNode> node)
{
	std::shared_ptr<AVLNode> z = (node -> right_) -> left_; 
	std::shared_ptr<AVLNode> parent = node -> parent_.lock(); 

	if  ( ((z -> left_) != nullptr ) && ((z -> right_)!= nullptr) ) //both left and right child
	{
		std::shared_ptr <AVLNode> z_left = z -> left_; 
		std::shared_ptr <AVLNode> z_right = z -> right_; 

		(node -> right_) -> left_ = z_right; 
		z -> right_ = node -> right_; 
		z -> left_ = node; 
		node -> right_ = z_left; 
		parent -> right_= z; 
		z -> parent_ = parent; 
		z_left -> parent_ = (node -> right_) -> left_; 
		z_right -> parent_ = node -> right_; 
		node -> parent_ = z;
	}
	if ( (z->left_) == nullptr && z->right_ != nullptr) //only right child 
	{ 
		std::shared_ptr <AVLNode> z_right = z -> right_; 

		(node -> right_) -> left_ = z_right; 
		z -> right_ = node -> right_; 
		z -> left_ = node; 
		parent -> right_= z; 
		z -> parent_ = parent; 
		z_right -> parent_ = node -> right_; 
		(node -> right_) -> parent_ = z;
		node -> parent_ = z;
	}
	if ( z -> left_ != nullptr && z-> right_ == nullptr) // only left child
	{
		std::shared_ptr <AVLNode> z_left = z -> left_; 

		z -> right_ = node -> right_; 
		z -> left_ = node; 
		node -> right_ = z_left; 
		parent -> right_= z; 
		z -> parent_ = parent; 
		z_left -> parent_ = node -> right_; 
		(node -> right_) -> parent_ = z; 
		node -> parent_ = z;
	}
	else // no children
	{
		z -> right_ = node -> right_; 
		z -> left_ = node; 
		parent -> right_= z; 
		z -> parent_ = parent; 
		(node -> right_) -> parent_ = z; 
		node -> parent_ = z;
	}
	UpdateHeightandBalanceFactor(root_); 

}

void AVL::RLrotation(std::shared_ptr<AVLNode> node)
{
	std::shared_ptr<AVLNode> z = (node -> left_) -> right_; 
	std::shared_ptr<AVLNode> parent = node -> parent_.lock(); 

	if ( (z -> left_) && (z -> right_) != nullptr) //both left and right child
	{
		std::shared_ptr <AVLNode> z_left = z -> left_; 
		std::shared_ptr <AVLNode> z_right = z -> right_; 

		(node -> left_) -> right_ = z_left; 
		z -> left_ = node -> left_; 
		node -> left_ = z_right; 
		parent -> left_ = z; 
		z -> parent_ = parent; 
		z_left -> parent_ = (node -> left_) -> right_; 
		z_right -> parent_ = node; 
		node -> parent_ = z;
	}
	if ( (z->left_) == nullptr && z->right_ != nullptr) //only right child 
	{ 
		std::shared_ptr <AVLNode> z_right = z -> right_; 

		z -> left_ = node -> left_; 
		node -> left_ = z_right; 
		parent -> left_ = z; 
		z -> parent_ = parent; 
		z_right -> parent_ = node; 
		node -> parent_ = z;
	}
	if ( z -> left_ != nullptr && z-> right_ == nullptr) // only left child
	{
		std::shared_ptr <AVLNode> z_left = z -> left_; 

		(node -> left_) -> right_ = z_left; 
		z -> left_ = node -> left_; 
		z -> right_ = node; 
		parent -> left_ = z; 
		z -> parent_ = parent; 
		z_left -> parent_ = node -> left_;
		node -> parent_ = z;
	}
	else // no children
	{
		z -> left_ = node -> left_; 
		z -> right_ = node; 
		parent -> left_ = z; 
		z -> parent_ = parent; 
		(node -> left_) -> parent_ = z; 
		node -> parent_ = z;
	}

	UpdateHeightandBalanceFactor(node); 
}

