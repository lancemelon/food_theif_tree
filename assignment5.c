
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MAX_INPUT 15

typedef struct Node Node;

struct Node{
    int location, hat, food;
    Node *parent;
    Node *left;
    Node *right;
};

Node *createNode(int location, int hat);
Node *insert(Node *root, int hat, int location);
Node *capture(Node *root, int location);
Node *closest(Node *root, int target);
Node *hat(Node *root, int hatSize, int location);
void stealFood(Node *root, int location, int amount);
Node *rotate(Node *current);

Node *createNode(int location, int hat) {
    Node *res = (Node *) calloc(1, sizeof(Node));

    res->hat = hat;
    res->location = location;

    return res;
}

Node *insert(Node *root, int hat, int location) {
    // Base case if there is no node
    if (root == NULL) {
        return createNode(location, hat);
    }

    // Check if the current node's hat is less than the new hat
    if (hat > root->hat) {
        // Swap the new node with the current root
        Node *temp = createNode(location, hat);
        temp->left = root->left;
        temp->right = root->right;
        temp->parent = root->parent;
        
        root->left = NULL;
        root->right = NULL;
        root->parent = temp;
        
        // Decide which side of the new node the current root should go
        if (location < temp->location) {
            temp->left = root;
        } else {
            temp->right = root;
        }
        
        return temp; // The new node becomes the root
    }

    // Traverse the tree to insert the new node based on location and hat size
    if (location < root->location) {
        root->left = insert(root->left, hat, location);
        root->left->parent = root;  // Update the parent pointer of the left child
    } else if (location > root->location) {
        root->right = insert(root->right, hat, location);
        root->right->parent = root;  // Update the parent pointer of the right child
    } else {
        // If the location already exists, update the hat if needed
        if (hat > root->hat) {
            root->hat = hat;
        }
    }

    return root;
}


Node *capture(Node *root, int location) {
    Node *cur = root;
    Node *parent = NULL;

    // Find the node to be captured and its parent
    while (cur != NULL && cur->location != location) {
        parent = cur;
        if (cur->location > location) {
            cur = cur->right;
        } else {
            cur = cur->left;
        }
    }

    // If the node to be captured is not found, return the root
    if (cur == NULL) {
        return root;
    }

    int capturedHat = cur->hat;  // Store the hat size of the captured node

    // If the node has only one child or no child
    if (cur->left == NULL || cur->right == NULL) {
        Node *child = (cur->left != NULL) ? cur->left : cur->right;

        // Update the parent pointer of the child
        if (child != NULL) {
            child->parent = parent;
        }

        // Update the parent's pointer to the child
        if (parent == NULL) {
            // If the node to be captured is the root
            return child;
        } else if (parent->left == cur) {
            parent->left = child;
        } else {
            parent->right = child;
        }

        // Free the memory of the captured node
        free(cur);

        // Call the hat function to update the hat size of the closest node
        root = hat(root, capturedHat, location);
    }
    // If the node has two children
    else {
        // Find the predecessor node (maximum node in the left subtree)
        Node *pred = cur->left;
        while (pred->right != NULL) {
            pred = pred->right;
        }

        // Swap values between cur and pred
        int tmp = cur->location;
        cur->location = pred->location;
        pred->location = tmp;

        // Recursively capture the node with the swapped value
        cur->left = capture(cur->left, location);
    }

    return root;
}

Node *closest(Node *root, int target) {
    // Check if the tree is empty
    if (root == NULL) {
        return NULL;
    }

    Node *res = root;  // Initialize res to the root node
    int minDiff = abs(root->location - target);  // Initialize minDiff to the difference between root value and target

    // Traverse the BST using inorder traversal
    while (root != NULL) {
        int diff = abs(root->location - target);  // Calculate the difference between current node value and target

        // Update closest if the current node is closer to the target
        if (diff < minDiff) {
            minDiff = diff;
            res = root;
        }

        // Move to the left or right subtree based on the target value
        if (target < root->location) {
            root = root->left;
        } else if (target > root->location) {
            root = root->right;
        } else {
            return root;  // If target matches current node value, return the current node
        }
    }

    return res;
}

Node *hat(Node *root, int hatSize, int location) {
    // Find the closest node to the given location
    Node *res = closest(root, location);

    // Check if closest node exists
    if (res == NULL) {
        return NULL;  // No node found, return NULL
    }

    // Check if the hat size of the closest node is greater than or equal to the given hat size
    if (res->hat >= hatSize) {
        return res;  // No need to update hat size, return the closest node
    } else {
        // Update the hat size of the closest node
        res->hat = hatSize;
        // TODO ROTATE FUNCTION
        return rotate(res);  // Return the new root of the rotated subtree
    }
}

void stealFood(Node *root, int location, int amount) {
    Node *thief = closest(root, location);

    while (thief != NULL) {
        // Share half of the amount with the thief node
        thief->food += amount / 2;

        // Update the remaining amount to share
        amount -= amount / 2;

        // Share the remaining amount with the parent
        if (thief->parent != NULL) {
            thief = thief->parent;
            thief->food += amount / 2;

            // Update the remaining amount again
            amount -= amount / 2;
        } else {
            // If thief has no parent (reached root), exit the loop
            break;
        }
    }
}

Node *rotate(Node *node) {
    // If the node is NULL or it doesn't have any children, no rotation needed
    if (node == NULL || (node->left == NULL && node->right == NULL)) {
        return node;
    }

    // Find the node with the highest hat in the subtree
    Node *maxHatNode = node;
    if (node->left != NULL && node->left->hat > maxHatNode->hat) {
        maxHatNode = node->left;
    }
    if (node->right != NULL && node->right->hat > maxHatNode->hat) {
        maxHatNode = node->right;
    }

    // If the node with the highest hat is already the root, no rotation needed
    if (maxHatNode == node) {
        return node;
    }

    // Rotate the subtree such that the node with the highest hat becomes the new root
    if (maxHatNode == node->left) {
        // Right rotation
        Node *temp = maxHatNode->right;
        maxHatNode->right = node;
        node->left = temp;
    } else {
        // Left rotation
        Node *temp = maxHatNode->left;
        maxHatNode->left = node;
        node->right = temp;
    }

    // Update parent pointers
    if (node->parent != NULL) {
        if (node->parent->left == node) {
            node->parent->left = maxHatNode;
        } else {
            node->parent->right = maxHatNode;
        }
    }

    maxHatNode->parent = node->parent;
    node->parent = maxHatNode;

    // Return the new root of the rotated subtree
    return maxHatNode;
}

void printFood(Node *root) {
    if (root == NULL) {
        return;
    }
    // Traverse the left subtree
    printFood(root->left);
    
    // Print the location and food of the current node
    printf("%d %d\n", root->location, root->food);

    // Traverse the right subtree
    printFood(root->right);
}

int main() {
    char input[100];
    char function[20];
    int inputLocation, inputHat, inputFood;
    Node *root = NULL;
    bool quit = false;

    // read input until QUIT
    while (!quit) {
        fgets(input, 100, stdin);
        input[strcspn(input, "\n")] = '\0';

        sscanf(input, "%s", function);
        function[sizeof(function) - 1] = '\0';

        if (strcmp(function, "ADD") == 0) {
            sscanf(input, "%s %d %d", function, &inputLocation, &inputHat);
            root = insert(root, inputHat, inputLocation);
        } 
        else if (strcmp(function, "CAPTURE") == 0) {
            sscanf(input, "%s %d", function, &inputLocation);
            printf("%d\n", closest(root, inputLocation)->food);
            root = capture(root, inputLocation);
        } 
        else if (strcmp(function, "HAT") == 0) {
            sscanf(input, "%s %d %d", function, &inputLocation, &inputHat);
            root = hat(root, inputHat, inputLocation);
        } 
        else if (strcmp(function, "STEAL") == 0) {
            sscanf(input, "%s %d %d", function, &inputLocation, &inputFood);
            stealFood(root, inputLocation, inputFood);
        } 
        else if (strcmp(function, "QUIT") == 0) {
            printFood(root);
            quit = true;
        }
    }

    return 0;
}