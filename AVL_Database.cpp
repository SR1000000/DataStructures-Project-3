#include "AVL_Database.hpp"
#include <cstdlib>   // for abs()
#include <iostream>

Record::Record(const std::string& k, int v) : key(k), value(v) {}

AVLNode::AVLNode(Record* r) : record(r), left(nullptr), right(nullptr), height(1) {}

AVLTree::AVLTree() : root(nullptr) {}

int AVLTree::height(AVLNode* node) {
    return node ? node->height : 0;
}

int AVLTree::balance(AVLNode* node) {
    return node ? height(node->left) - height(node->right) : 0;
}

//Check balance of node, balancing it with rotations if required
AVLNode* AVLTree::doBalance(AVLNode* a) {
    if(abs(balance(a->left)) > 1) {
        a->left = doBalance(a->left);
    }
    if(abs(balance(a->right)) > 1) {
        a->right = doBalance(a->right);
    }
    
    if(balance(a) > 1) {
        if(balance(a->left) >= 0)
            return rotateRight(a);
        else {
            a->left = rotateLeft(a->left);
            return rotateRight(a);
        }
    } else if(balance(a) < -1) {
        if(balance(a->right) <=0)
            return rotateLeft(a);
        else {
            a->right = rotateRight(a->right);
            return rotateLeft(a);
        }
    } else {
        updateHeight(a);
        return a;
    }
}

//Assumes node exists
void AVLTree::updateHeight(AVLNode* node) {
    if(height(node->left) >= height(node->right))
        node->height = height(node->left) + 1;
    else
        node->height = height(node->right) + 1;
}

//Assumes y->left exists
AVLNode* AVLTree::rotateRight(AVLNode* y) {
    AVLNode* yLeft = y->left;
    AVLNode* yLeftRight;
    if(y->left)
        yLeftRight = yLeft->right;
    else    
        std::cout << "Error: Expected y->left exist but not.";
    yLeft->right = y;
    y->left = yLeftRight;

    updateHeight(y);
    updateHeight(yLeft);

    return yLeft;
}

//Assumes x->right exists
AVLNode* AVLTree::rotateLeft(AVLNode* x) {
    AVLNode* xRight = x->right;
    AVLNode* xRightLeft;
    if(x->right)
        xRightLeft = xRight->left;
    else    
        std::cout << "Error: Expected x->right exist but not.";
    xRight->left = x;
    x->right = xRightLeft;

    updateHeight(x);
    updateHeight(xRight);

    return xRight;
}



void AVLTree::insert(Record* record) {
    //you can make a private memeber function to handle insertion.
    //e.g root = insertHelper(root, record);
    root = insertHelper(root, record);
    root = doBalance(root);
}

AVLNode* AVLTree::insertHelper(AVLNode* node, Record* r) {
    if(!node) {
        AVLNode* newNode = new AVLNode(r);
        return newNode;
    } else {
        if(node->record->value > r->value) {
            node->left = insertHelper(node->left, r);
            if(height(node->left) >= height(node))
                node->height++;
        } else { 
            node->right = insertHelper(node->right, r);
            if(height(node->right) >= height(node))
                node->height++;
        }
        //node->height = (height(node->left) > height(node->right)) ? height(node->left) + 1 : height(node->right) + 1;

        return node;
    }
}

Record* AVLTree::search(const std::string& key, int value) {
    return nullptr;
}



void AVLTree::deleteNode(const std::string& key, int value) {
    
}





void IndexedDatabase::insert(Record* record) {
   index.insert(record);
}

Record* IndexedDatabase::search(const std::string& key, int value) {
    return index.search(key, value);
}

void IndexedDatabase::deleteRecord(const std::string& key, int value) {
    index.deleteNode(key, value);
}

std::vector<Record*> IndexedDatabase::rangeQuery(int start, int end) {
    std::vector<Record*> output;

    return output;
}

std::vector<Record*> IndexedDatabase::findKNearestKeys(int key, int k) {
    std::vector<Record*> output;
   
    return output; 
}

std::vector<Record*> IndexedDatabase::inorderTraversal() {
    std::vector<Record*> output;
   
    return output;
}

void IndexedDatabase::clearDatabase() {
   
}

int IndexedDatabase::countRecords() {
    return 0;
}

