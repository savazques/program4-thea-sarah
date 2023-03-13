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
    
    Height(currentNode); 
    BalanceFactor(currentNode);
    rightrotation(currentNode); 
    leftrotation(currentNode); 
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
int AVL :: Height (std::shared_ptr<AVLNode> node)
{
	int height; 
	while (node != nullptr)
	{
	if(node->IsLeaf()) //if node is a leaf
	{
		height = 0; 
	}
	if(node -> HasRightChild() && node -> HasLeftChild()) //if node as both right and left child
	{
		//then the height is the max height of the right and left child + 1
		height = 1 + std::max(Height(node -> left_), Height(node -> right_));
		
	}
	if(node -> HasLeftChild() && !(node -> HasRightChild())) //if node has a left child but not a right child
	{
		height = 1 + Height(node -> left_); 
		//then the height is the 1 + the left child height
	}
	if(node -> HasRightChild() && !(node -> HasLeftChild())) //if node has a right child but not a left child
	{
		//then the height is the 1 + right child height
		height = 1 + Height(node -> right_); 
	}
 	}
	
	return height;
}

int AVL :: BalanceFactor (std::shared_ptr<AVLNode> node)
{
	int balanceFactor;
	if (node -> IsLeaf())
	{
		balanceFactor = 0;
	}
	if (node -> HasLeftChild() && node -> HasRightChild()) //if node has left and right child
	{
		//the balance factor is the right child height - left child height
		balanceFactor = Height(node -> right_) - Height(node -> left_); 
	}
	if (node -> HasLeftChild() && !(node -> HasRightChild())) // if node has only left child
	{
		//the balance factor is = (-1) - leftchild height
		balanceFactor = (-1) - Height(node -> left_); 
	}
	if (node -> HasRightChild() && !(node -> HasLeftChild())) //if node has only right child
	{
		//the balancefactor is - rightchild height - (-1) or rightchild + 1
		balanceFactor = Height(node -> right_) + 1; 
	}
	return balanceFactor; 
}

void AVL :: rightrotation (std::shared_ptr<AVLNode> node)
{
	if(BalanceFactor(node) < -1) //if the node is left heavy we have to do a right right rotation
	{
		std::shared_ptr<AVLNode> y = node -> left_; 
		std::shared_ptr<AVLNode> y_ = node -> left_-> right_; 

		std::shared_ptr<AVLNode> parent = node -> parent_.lock(); 

		node->left_ = y_; 
		y -> right_ = node; 
		parent -> left_ = y; 

		node -> parent_ = y; 
		y -> parent_ = parent; 
		y_ -> parent_ = node; 

		Height(node); 
		BalanceFactor(node); 
	}
}

void AVL :: leftrotation (std::shared_ptr<AVLNode> node)
{
	if(BalanceFactor(node) > 1)
	{
		std::shared_ptr<AVLNode> y = node -> right_; 
		std::shared_ptr<AVLNode> y_ = node -> right_ -> left_; 
		std::shared_ptr<AVLNode> parent = node -> parent_.lock(); 

		node -> right_ = y_; 
		y -> left_ = node; 
		parent -> right_ = y; 

		node -> parent_ = y; 
		y -> parent_ = parent; 
		y_ -> parent_ = node; 

		Height(node); 
		BalanceFactor(node); 		
	}

}