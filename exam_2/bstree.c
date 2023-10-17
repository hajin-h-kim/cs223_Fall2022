#include <stdio.h>
#include <stdlib.h>
#include "treehelpers.h"
  
// A utility function to create a new BST node
node* newNode(int item)
{
    node* temp = malloc(sizeof(node));
    temp->key = item;
    temp->left = temp->right = NULL;
    return temp;
}
  
// A utility function to do inorder traversal of BST
void inorder(node* root)
{
    if (root != NULL) {
        inorder(root->left);
        printf("%d ", root->key);
        inorder(root->right);
    }
}

void preorder(node* root)
{
    if(root != NULL){
        printf("%d ", root->key);
        preorder(root->left);
        preorder(root->right);
    }
}
void postorder(node* root)
{
    if(root != NULL){
        postorder(root->left);
        postorder(root->right);
        printf("%d ", root->key);
    }
}

// A utility function to insert
// a new node with given key in BST
node* insert(node* node, int key)
{
    // If the tree is empty, return a new node
    if (node == NULL)
        return newNode(key);
  
    // Otherwise, recur down the tree
    if (key < node->key)
        node->left = insert(node->left, key);
    else if (key > node->key)
        node->right = insert(node->right, key);
  
    // Return the (unchanged) node pointer
    return node;
}

/* Given a non-empty binary search tree, return the node
with minimum key value found in that tree. Note that the
entire tree does not need to be searched. */
node* minValueNode(node* root)
{
    node* current = root;
 
    /* loop down to find the leftmost leaf */
    while (current && current->left != NULL)
        current = current->left;
 
    return current;
}

node* deleteNode(node* root, int key)
{
    if(root == NULL)
        return root;
    if(root->key != key)
    {
        if(key < root->key)
        {
            root->left = deleteNode(root->left, key);
            return root;
        }
        else
        {
            root->right = deleteNode(root->right, key);
            return root;
        }
    }

    // found key
    // If one of the children is empty
    // if both are empty, we'll be returning NULL anyway
    if (root->left == NULL) {
        node* temp = root->right;
        free(root);
        return temp;
    }
    else if (root->right == NULL)
    {
        node* temp = root->left;
        free(root);
        return temp;
    }
    // If both children exist
    else {
 
        node* succParent = root;
 
        // Find successor
        node* succ = root->right;
        while (succ->left != NULL) {
            succParent = succ;
            succ = succ->left;
        }
 
        // Delete successor.  Since successor
        // is always left child of its parent
        // we can safely make successor's right
        // right child as left of its parent.
        // If there is no succ, then assign
        // succ->right to succParent->right
        if(succ->right != NULL)
        {
            succParent->right = succ->right;
        }
        succParent->left = NULL;
        root->key = succ->key;
        free(succ);
        return root;
    }
}

// Driver Code
int main()
{
    /* Let us create following BST
              50
           /     \
          30      70
         /  \    /  \
       20   40  60   80 */
    node* root = NULL;
    root = insert(root, 50);
    insert(root, 30);
    insert(root, 20);
    insert(root, 40);
    insert(root, 70);
    insert(root, 60);
    insert(root, 80);
  
    // Print inoder traversal of the BST
    // inorder(root);
    // preorder(root);
    // postorder(root);

    // Delete
    printf("Inorder traversal of the given tree \n");
    inorder(root);
 
    printf("\n\nDelete 60\n");
    root = deleteNode(root, 60);
    printf("Inorder traversal of the modified tree \n");
    inorder(root);

    printf("\n");
    return 0;
}