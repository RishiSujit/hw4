#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)
#include <algorithm>
#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
//function to calculate depth
int depthCalculator(Node* root)
{
  if(!root)
  {
    return 0;
  }
  //calculating the bigger of the two sides of each node, adding 1 for the first node
  return 1+ max(depthCalculator(root->left), depthCalculator(root->right));
}
bool equalPathsHelper(Node* root, int depth, int goal)
{
  if(!root)
  {
    return true;
  }
  //checking if leaf node
  if(!root -> left && !root ->right) 
  {
    return depth == goal;
  }
  //recursively calls if not leaf
  return equalPathsHelper(root->left, depth+1, goal) && equalPathsHelper(root->right, depth+1, goal);

  
}

bool equalPaths(Node * root)
{
    // Add your code below

    if(root ==NULL)
    {
      return true;
    }
    //finding ideal depth
    int goal = depthCalculator(root);
    return equalPathsHelper(root, 1, goal);
    

}



