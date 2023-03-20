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

    std::cout << "line 85" << std::endl; 

    UpdateHeightandBalanceFactor(root_); 

    std::shared_ptr<AVLNode> problemNode = findProblemNode(); 

    RotateTree(problemNode, problemNode -> balance_factor); 


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
	return result.dump(2) + "\n";
}


















std::shared_ptr<AVLNode> AVL::findProblemNode() {
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
            if ( v -> balance_factor > 1) { 
                return v; 
            }

            result[key]["height"] = v -> height;
		}
	}

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
		int bf = rightHeight - leftHeight; 
        node -> balance_factor = bf;
		node -> height = std::max(rightHeight , leftHeight ) + 1 ;
    
        std::cout << "the nodes balance factor right before the abs check is  " << node-> balance_factor << std::endl; 
        std::cout << "the nodes hieght right before the abs check is  " << node-> height << std::endl;
		if(std::abs(bf) > 1)
		{
            RotateTree(node, bf); 
           // UpdateHeightandBalanceFactor(root_); 

		}
	}

    std::cout << "the nodes balance factor at this time is " << node-> balance_factor << std::endl; 
    std::cout << "the nodes height at this time is " << node-> height<< std::endl;

	return node -> height;

    
}



void AVL :: RotateTree(std::shared_ptr<AVLNode> node, int bf)
{
	std::cout<< "entering rotate"<<std::endl;
    


    std::cout<< "before assinging right node"<<std::endl;
    
    std::cout<< "before assinging left balance factor" <<std::endl;

    std::cout<< "before assinging right balance facotr" <<std::endl;

    std::cout<< "entering rotate"<<std::endl;
	if (bf > 1)
	{
		std::shared_ptr<AVLNode> rightNode = node -> right_;
		int rightleftheight = UpdateHeightandBalanceFactor(rightNode-> left_); 
		int rightrightheight = UpdateHeightandBalanceFactor(rightNode -> right_);
		int Rbf = (rightrightheight - rightleftheight); 
		std::cout<<Rbf<< std::endl; 
       
		std::cout<<"we have a right heavy tree"<<std::endl;
		if (bf == 1)
		{
			std::cout<<"we have a rigt right prblm"<<std::endl; 
			LLrotation(node); 
		}
		if (bf  == -1)
		{
			std::cout<<"we have a  left right problem"<<std::endl;
			LRrotation(node);   
		}
	}
	if (bf  < -1)
	{
		std::shared_ptr<AVLNode> leftNode = node -> left_; 
		int leftleftheight = UpdateHeightandBalanceFactor(leftNode-> left_); 
		int leftrightheight = UpdateHeightandBalanceFactor(leftNode -> right_);
		int Lbf = (leftrightheight - leftleftheight); 
		std::cout<<Lbf<<std::endl; 
		if(bf == 1)
		{
            std::cout << "we have a right left problem" << std::endl; 
			RLrotation(node); 
		}
		if(bf == -1)
		{
            std::cout << "we have a rr rotation problem" << std::endl; 
			RRrotation(node); 
		}
	}
	
}

void AVL :: RRrotation (std::shared_ptr<AVLNode> node)
{
	
		std::shared_ptr<AVLNode> y = node -> left_; //left child of node 

        if (node == root_) { 
            if(y -> right_ != nullptr)
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
            
			if(y -> right_ != nullptr)
			{
				std::shared_ptr<AVLNode> y_ = y->right_; 
				y_ -> parent_.reset(); 
				y -> parent_.reset(); 
				node->left_ = y_; 
				//here 
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
UpdateHeightandBalanceFactor(y); 
	
}


void AVL :: LLrotation (std::shared_ptr<AVLNode> node)
{
	std::cout<<"left left"<<std::endl; 
	std::shared_ptr<AVLNode> y = node -> right_; 
	if(node == root_)
	{
		    if(y -> left_ != nullptr)
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
		if(y -> left_ != nullptr)
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
UpdateHeightandBalanceFactor(y); 
}

void AVL::LRrotation(std::shared_ptr<AVLNode> node)
{

std::cout<<"entering left right"<<std::endl; 
        std::shared_ptr<AVLNode> a = node; 
        std::shared_ptr<AVLNode> b = a->right_; 
        std::shared_ptr<AVLNode> c = b->left_; 


        if (c -> right_ != nullptr) { 

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

        if (c -> right_ == nullptr){ 

            b->parent_.reset(); 
            b->left_ = nullptr; 

            a->right_ = c; 
            c->parent_ = a;

            c->right_ = b; 
            b->parent_ = c; 
            
        }
std::cout<<"exiting left rihg "<<std::endl; 
		LLrotation(node); 

}

void AVL::RLrotation(std::shared_ptr<AVLNode> node)
{

        std::shared_ptr<AVLNode> a = node; 
        std::shared_ptr<AVLNode> b = a->left_; 
        std::shared_ptr<AVLNode> c = b->right_; 


        if (c -> left_ != nullptr) { 

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

        if (c -> left_ == nullptr){ 

            b->parent_.reset(); 
            b->right_ = nullptr; 

            a->left_ = c; 
            c->parent_ = a;

            c->left_ = b; 
            b->parent_ = c; 
            
        }

		RRrotation(node); 

}

