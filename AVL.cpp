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

    //    std::cout<< "checking if the root is null" << std::endl;
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


    UpdateHeightandBalanceFactor(lastNode); 
	RotateTree(lastNode); 

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

            result[key]["balance factor"] = v -> balance_factor; 
            result[key]["height"] = v -> height;
		}
	}
	result["size"] = size_;
	result["height"] = 3;
	return result.dump(2) + "\n";
}


void AVL :: UpdateHeightandBalanceFactor (std::shared_ptr<AVLNode> node)
{
if(node != nullptr)
{
	if(node -> IsLeaf())
	{
		node -> height = 0; 
		node -> balance_factor = 0; 
	}
	else{
		if(node -> HasLeftChild() && node -> HasRightChild())
		{
			int rheight = node -> right_ -> height; 
			int lheight = node -> left_ -> height; 
			node -> height = std::max(rheight, lheight) + 1; 
			node -> balance_factor = node -> right_ -> height - node -> left_ -> height; 
		}
		else if (node -> HasLeftChild())
		{
			node -> height = (node -> left_ -> height) +1; 
			node -> balance_factor = -1 - (node -> left_ -> height); 
		}
		else if(node -> HasRightChild())
		{
			node -> height = (node -> right_ -> height) + 1;
			node -> balance_factor = (node -> right_ -> height) + 1;  
		}
		else 
		{
			std::cerr <<"error with bf and height"<<std::endl; 
		}
	}
	UpdateHeightandBalanceFactor(node -> parent_.lock()); 
}

	return; 
	
	
}

void AVL :: RotateTree(std::shared_ptr<AVLNode> node)
{

	if(node != nullptr)
	{
		//std::cout << "the bf of right node is" << node -> right_ -> balance_factor <<std::endl; 
		//std::cout<<"the bf pf left node is "<<node ->left_ -> balance_factor <<std::endl; 
			if(std::abs(node->balance_factor) > 1)
			{
				if (node -> balance_factor == 2)
					{
						std::shared_ptr<AVLNode> rightNode = node -> right_;
						if (node -> right_ -> balance_factor == 1)
						{
							
							this -> LLrotation(node); 
							//std::cout<<"we have exited LL"<<std::endl; 
						}
						if (node -> right_ -> balance_factor  == -1)
						{
							this -> LRrotation(node);  
							 
						}
					}
				else if (node -> balance_factor == -2)
						{
							if( node -> left_ -> balance_factor == 1)
							{
								this -> RLrotation(node);  
								
							}
							if(node -> left_-> balance_factor == -1)
							{ 
								this -> RRrotation(node); 
								
							}
						}
				else
				{
					UpdateHeightandBalanceFactor(node -> parent_.lock()); 
					RotateTree(node ->parent_.lock()); 

				}
						
			}
	
	}

}

void AVL :: RRrotation (std::shared_ptr<AVLNode> node)
{
	
		std::shared_ptr<AVLNode> y = node -> left_; //left child of node 

        if (node == root_) { 
            if(y -> HasRightChild())
			{
				std::shared_ptr<AVLNode> y_ = y->right_; 
				y_ -> parent_.reset(); 
				y -> parent_.reset(); 
				node->left_ = y_; 
				root_ = y;
		
				y -> right_ = node; 
				node -> parent_ = y; 
				
				y_ -> parent_ = node; 
			}
			else
			{
				y -> parent_.reset(); 
				root_ = y; 
				y -> right_ = node;
				node -> parent_ = y; 
                node -> left_ = nullptr; 
			}
            
        }
	    if(node != root_)
		{
            std::shared_ptr<AVLNode> parent = node -> parent_.lock(); 
            
			if(y -> HasRightChild())
			{
				std::shared_ptr<AVLNode> y_ = y->right_; 
				y_ -> parent_.reset(); 
				y -> parent_.reset(); 
				node->left_ = y_; 
				y -> parent_ = parent;
				node -> parent_.reset(); 

				parent -> left_ = y; 
				y -> right_ = node; 
				node -> parent_ = y; 
				
				y_ -> parent_ = node; 
			}
			else
			{
				y -> parent_.reset(); 
				node -> parent_.reset(); 
				y -> right_ = node;
				node -> parent_ = y; 
				parent -> left_ = y; 
				y -> parent_ = parent; 
			}
		}
	//UpdateHeightandBalanceFactor(node -> parent_.lock()); 
	
}


void AVL :: LLrotation (std::shared_ptr<AVLNode> node)
{
	
	std::shared_ptr<AVLNode> y = node -> right_; 
	if(node == root_)
	{
		    if(y -> HasLeftChild())
			{
				std::shared_ptr<AVLNode> y_ = y->left_; 
				y_ -> parent_.reset(); 
				y -> parent_.reset(); 
				node->right_ = y_; 
				root_ = y;
		
				y -> left_ = node; 
				node -> parent_ = y; 
				
				y_ -> parent_ = node; 
			}
			else
			{
				y -> parent_.reset(); 
				root_ = y; 
				y -> left_ = node;
				node -> parent_ = y; 
                node -> right_ = nullptr; 
			}
	}
	
	if(node != root_)
	{
		std::shared_ptr<AVLNode> parent = node -> parent_.lock(); 
		if(y -> HasLeftChild())
		{
	
			std::shared_ptr<AVLNode> y_ = y->left_; 
            y_ -> parent_.reset(); 
            y -> parent_.reset(); 
			node->right_ = y_; 
            //here 
            y -> parent_ = parent;
            node -> parent_.reset(); 

            parent -> right_ = y; 
			y -> left_ = node; 
			node -> parent_ = y; 
            
			y_ -> parent_ = node; 
		}
		else
		{
			y -> parent_.reset(); 
			node -> parent_.reset(); 
			y -> left_ = node;
			node -> parent_ = y; 
			parent -> right_ = y; 
			y -> parent_ = parent; 
		}
	}

//UpdateHeightandBalanceFactor(node->parent_.lock()); 
}

void AVL::LRrotation(std::shared_ptr<AVLNode> node)
{


        std::shared_ptr<AVLNode> a = node; 
        std::shared_ptr<AVLNode> b = a->right_; 
        std::shared_ptr<AVLNode> c = b->left_; 

        if (c -> HasRightChild()) { 
            std::shared_ptr<AVLNode> c_ = c -> right_; 

            c_ -> parent_.reset(); 
           
            b->parent_.reset(); 
            b->left_ = c_; 
            c_ -> parent_ = b;

            a->right_ = c; 
            c->parent_ = a;

            c->right_ = b; 
            b->parent_ = c; 

        }

        else{ 
            b->parent_.reset(); 
            b->left_ = nullptr; 

            a->right_ = c; 
            c->parent_ = a;

            c->right_ = b; 
            b->parent_ = c; 
		}
        

		LLrotation(a); 

}

void AVL::RLrotation(std::shared_ptr<AVLNode> node)
{

        std::shared_ptr<AVLNode> a = node; 
        std::shared_ptr<AVLNode> b = a->left_; 
        std::shared_ptr<AVLNode> c = b->right_; 


        if (c -> HasLeftChild()) { 

            std::shared_ptr<AVLNode> c_ = c -> left_; 

            c_ -> parent_.reset(); 
           
            b->parent_.reset(); 
            b->right_ = c_; 
            c_ -> parent_ = b;

            a->left_ = c; 
            c->parent_ = a;

            c->left_ = b; 
            b->parent_ = c; 

        }

        else{ 

            b->parent_.reset(); 
            b->right_ = nullptr; 

            a->left_ = c; 
            c->parent_ = a;

            c->left_ = b; 
            b->parent_ = c; 
            
        }

		RRrotation(a); 

}

