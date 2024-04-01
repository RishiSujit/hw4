#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);
    virtual void removeFix(AVLNode<Key, Value>* node, int8_t diff);
    virtual void insertFix(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* node);
    virtual void rotateLeft(AVLNode<Key, Value>* node);
    virtual void rotateRight(AVLNode<Key, Value>* node);
   
    // Add helper functions here


};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    
    AVLNode<Key, Value>* currNode= static_cast<AVLNode<Key, Value>*>(this->root_);
    //if tree is empty create new root
    if(BinarySearchTree<Key, Value>::empty())
    {
      AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(new_item.first, new_item.second, NULL);
      this->root_ = newNode;
      newNode->setBalance(0);
      return;
    }
    else
    {
      AVLNode<Key, Value>* parent_node = NULL;
      //find appropiate position for new node
      while (currNode != NULL)
      {
        parent_node = currNode;
        if(new_item.first == currNode->getKey())
        {
          //if key exists updates value
          currNode->setValue(new_item.second);
          return;
        }
        else if(new_item.first< currNode->getKey())
        {
          currNode = currNode->getLeft();
        }
        else if(new_item.first > currNode->getKey())
        {
          currNode = currNode->getRight();
        }
      }
      //create new node with item and set parents
      if (new_item.first < parent_node->getKey())
      {
        AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(new_item.first, new_item.second, parent_node);
        parent_node->setLeft(newNode);
      }
      else if (new_item.first > parent_node->getKey())
      {
        AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(new_item.first, new_item.second, parent_node);

        parent_node->setRight(newNode);
      }
      //perform avl tree balance
      AVLNode<Key, Value> *newNode = static_cast<AVLNode<Key, Value>*>(this->internalFind(new_item.first));
      newNode->setBalance(0);
      if(newNode->getParent()->getBalance() == 1 || newNode->getParent()->getBalance() == -1)
      {
        newNode->getParent()->setBalance(0);
        return;
      }
      else if(newNode->getParent()->getBalance() == 0)
      {
        if(newNode->getParent()->getLeft() == NULL)
        {
          newNode->getParent()->setBalance(1);
        }
        else{
          newNode->getParent()->setBalance(-1);
        }
        insertFix(newNode->getParent(), newNode);

      }
    }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::remove(const Key& key)
{
    int diff = 0;
    if(this->root_ == NULL)
    {
      return;
    }
    //find node to be removed
    AVLNode<Key, Value>* removalNode = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
    if(removalNode == NULL)
    {
      return;
    }
    AVLNode<Key, Value> *removedNodeParent = NULL;
    AVLNode<Key, Value>* nextNode = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
    //if node has no children
    if (nextNode->getLeft() == NULL && nextNode->getRight() == NULL)
    {
      nextNode = removalNode->getParent();
      if (removedNodeParent != NULL)
      {
        if(removedNodeParent->getLeft() == removalNode)
        {
          diff = 1;
        }
        else
        {
          diff = -1;
        }
      }
      //if this case remove node from tree
      if(nextNode == this->root_)
      {
        this->root_ = NULL;
      }
      else
      {
        if(nextNode->getParent()->getLeft() == nextNode)
        {
          nextNode->getParent()->setLeft(NULL);
        }
        else
        {
          nextNode->getParent()->setRight(NULL);
        }
      }
      delete nextNode;
      
    }
    //if node only has one child
    else if(nextNode->getLeft() == NULL || nextNode->getRight() == NULL)
    {
      removedNodeParent = removalNode->getParent();
      if(removedNodeParent != NULL)
      {
        if(removedNodeParent->getLeft() == removalNode)
        {
          diff = 1;
        }
        else{
          diff = -1;
        }
      }
      //find the child to be removed
      AVLNode<Key, Value>* child;
      if(nextNode->getLeft() == NULL)
      {
        child = nextNode->getRight();
      }
      else
      {
        child = nextNode->getLeft();
      }
      //remove node
      if(nextNode == this->root_)
      {
        this->root_ = child;
        child->setParent(NULL);
      }
      else
      {
        
        if(nextNode->getParent()->getLeft() == nextNode)
        {
          nextNode->getParent()->setLeft(child);
        }
        else{
          nextNode->getParent()->setRight(child);
        }
        child->setParent(nextNode->getParent());
        
      }
      delete nextNode;
      
    }

      //node has two children
    else
    {
      nodeSwap(nextNode, static_cast<AVLNode<Key, Value>*>(this->predecessor(nextNode)));
      removedNodeParent = removalNode->getParent();
      if(removedNodeParent != NULL)
      {
        if(removedNodeParent->getLeft() == removalNode)
        {
          diff = 1;
        }
        else
        {
          diff = -1;
        }
      }
      if(nextNode->getLeft() == NULL && nextNode->getRight() == NULL)
      {
        if(nextNode == this->root_)
        {
          this->root_ = NULL;
        }
        else
        {
          if(nextNode->getParent()->getLeft() == nextNode)
          {
            nextNode->getParent()->setLeft(NULL);
          }
          else{
            nextNode->getParent()->setRight(NULL);
          }
        }
        delete nextNode;
      }
      else if(nextNode->getLeft() == NULL || nextNode->getRight() == NULL)
      {
        Node<Key, Value>* child;
        if (nextNode->getLeft() == NULL)
        {
          child = nextNode->getRight();
        }
        else{
          child = nextNode->getLeft();
        }

        if(nextNode == this->root_)
        {
          this->root_ = child;
          child->setParent(NULL);
        }
        else
        {
          if(nextNode->getParent()->getLeft() == nextNode)
          {
            nextNode->getParent()->setLeft(child);
          }
          child->setParent(nextNode->getParent());
        }
        delete nextNode;
      }
    }

    //print tree
    this->print();
    //perform avl rebalancing
    removeFix(removedNodeParent, diff);


}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}



template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* node)
{
  //store child
  AVLNode<Key, Value>* m = node->getRight();
  AVLNode<Key, Value>* temp = m->getLeft();
  AVLNode<Key, Value>* parent  = node->getParent();
  //perform rotation
  m->setLeft(node);
  m->setParent(parent);
  node->setParent(m);
  //update parent
  if(parent != NULL)
  {
    if(parent->getRight() == node)
    {
      parent->setRight(m);
    }
    else
    {
      //if the given node is root, update it
      parent->setLeft(m);
    }
  }
  else
  {
    this->root_=m;
  }
  node->setRight(temp);
  //update parent
  if(temp != NULL)
  {
    temp->setParent(node);
  }
  

  
}


template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* node)
{
  //store all the nodes
  AVLNode<Key, Value>* m = node->getLeft();
  AVLNode<Key, Value>* temp = m->getRight();
  AVLNode<Key, Value>* parent  = node->getParent();

  m->setRight(node);
  m->setParent(parent);
  node->setParent(m);
  //update the parent
  if(parent != NULL)
  {
    if(parent->getLeft() == node)
    {
      parent->setLeft(m);
    }
    else
    {
      //if it is root update it
      parent->setRight(m);
    }
  }
  else
  {
    this->root_=m;
  }
  node->setLeft(temp);
  //update parent
  if(temp != NULL)
  {
    temp->setParent(node);
  }

}

template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* node) {
  if(parent == NULL)
  {
    return;
  }
  //find parent
  AVLNode<Key, Value>* grandParent = parent->getParent();
  //check if parent is a left child of grandparent
  if(grandParent == NULL)
  {
    return;
  }
  if(grandParent->getLeft() == parent)
  {
    grandParent->updateBalance(-1);

    if(grandParent->getBalance() == 0)
    {
      return;
    }
    else if(grandParent->getBalance() == -1)
    {
      insertFix(grandParent, parent);
    }
    else
    {
      //check the roation case and perform adjustments
      if(parent->getLeft() == node)
      {
        rotateRight(grandParent);
        grandParent->setBalance(0);
        parent->setBalance(0);
      }
      else{
        rotateLeft(parent);
        rotateRight(grandParent);
        //update balance after rotations
        if(node->getBalance() == -1)
        {
          parent->setBalance(0);
          grandParent->setBalance(1);
          node->setBalance(0);

        }
        else if(node->getBalance() == 0)
        {
          parent->setBalance(0);
          grandParent->setBalance(0);
          node->setBalance(0);
        }
        else if(node->getBalance() == 1)
        {
          parent->setBalance(-1);
          grandParent->setBalance(0);
          node->setBalance(0);
        }
      }
    }
  }
  else{
    //update balance factor of grandparent
    grandParent->updateBalance(1);
    if(grandParent->getBalance() == 0)
    {
      return;
    }
    else if(grandParent->getBalance() == 1)
    {
      insertFix(grandParent, parent);

    }
    else{
      if(parent->getRight() == node)
      {
        rotateLeft(grandParent);
        grandParent->setBalance(0);
        parent->setBalance(0);
      }
      //update balances 
      else{
        rotateRight(parent);
        rotateLeft(grandParent);
        if(node->getBalance() == 1)
        {
          parent->setBalance(0);
          grandParent->setBalance(-1);
          node->setBalance(0);
        }
        else if(node->getBalance() == -1)
        {
          parent->setBalance(1);
          grandParent->setBalance(0);
          node->setBalance(0);
        }
        else if(node->getBalance() == 0)
        {
          parent->setBalance(0);
          grandParent->setBalance(0);
          node->setBalance(0);
        }
        
      }
    }
  }


}

template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value>* node, int8_t diff) {
  if(node == NULL)
  {
    return;
  }
  //get parent of node
  AVLNode<Key, Value>* parent = node->getParent();
  int nDiff = 0;
  if(parent != NULL)
  {
    if(parent->getLeft() == node)
    {
      nDiff = 1;
    }
    else
    {
      nDiff = -1;
    }
  }
  if(diff == -1)
  {
    //check if rotations are needed
    if(node->getBalance() + diff == -2)
    {
      AVLNode<Key, Value>* child = node->getLeft();
      if(child->getBalance() == -1)
      {
        rotateRight(node);
        node->setBalance(0);
        child->setBalance(0);
        removeFix(parent, nDiff);
      }
      else if(child->getBalance() == 0)
      {
        rotateRight(node);
        node->setBalance(-1);
        child->setBalance(1);
      }
      else
      {
        AVLNode<Key, Value>* grandParent = child->getRight();
        rotateLeft(child);
        rotateRight(node);
        if(grandParent->getBalance() == 1)
        {
          node->setBalance(0);
          child->setBalance(-1);
          grandParent->setBalance(0);
        }
        else if(grandParent->getBalance() == 0)
        {
          node->setBalance(0);
          child->setBalance(0);
          grandParent->setBalance(0);
        }
        else
        {
          node->setBalance(1);
          child->setBalance(0);
          grandParent->setBalance(0);
        }
        removeFix(parent, nDiff);
      }
    }
    else if(node->getBalance()+diff == -1)
    {
      node->setBalance(-1);
    }
    else{
      node->setBalance(0);
      removeFix(parent, nDiff);
    }
  }
  //check balance factors
  else
  {
    if(node->getBalance() + diff == 2)
    {
      AVLNode<Key, Value>* child = node->getRight();
      if(child->getBalance() == 0)
      {
        rotateLeft(node);
        node->setBalance(1);
        child->setBalance(-1);
      }
      else if(child->getBalance() == 1)
      {
        rotateLeft(node);
        node->setBalance(0);
        child->setBalance(0);
        removeFix(parent, nDiff);
      }
      
      else
      {
        AVLNode<Key, Value>* grandchild = child->getLeft();
        rotateRight(child);
        rotateLeft(node);
        if(grandchild->getBalance() == 0)
        {
          node->setBalance(0);
          child->setBalance(0);
          grandchild->setBalance(0);
        }
        else if(grandchild->getBalance() == -1)
        {
          node->setBalance(0);
          child->setBalance(1);
          grandchild->setBalance(0);
        }
        
        else
        {
          node->setBalance(-1);
          child->setBalance(0);
          grandchild->setBalance(0);
        }
        removeFix(parent, nDiff);
      }

    }
    else if(node->getBalance()+diff == 1)
    {
      node->setBalance(1);
    }
    else
    {
      node->setBalance(0);
      removeFix(parent, nDiff);
    }
  }


}


#endif
