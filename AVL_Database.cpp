#include "AVL_Database.hpp"
#include <cstdlib>   // for abs()
#include <iostream>

Record::Record(const std::string& k, int v) : key(k), value(v) {}

AVLNode::AVLNode(Record* r) : record(r), left(nullptr), right(nullptr), height(1) {}

AVLTree::AVLTree() : root(nullptr) {}

int AVLTree::height(AVLNode* node) const {
    return node ? node->height : 0;
}

int AVLTree::balance(AVLNode* node) const {
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
        updateHeight(a);    //necessary to propagate height changes from rebalance up the tree
        return a;
    }
}

//Assumes node exists, usually used in recursion and designed to propagate in post-order fashion
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
            if(height(node->left) >= height(node))  //updates height up the entire chain if necessary
                node->height++;
        } else { 
            node->right = insertHelper(node->right, r);
            if(height(node->right) >= height(node)) //updates height up the entire chain if necessary
                node->height++;
        }
        return node;
    }
}

Record* AVLTree::search(const std::string& key, int value) const {
        return searchHelper(root, key, value);
}

//Recursive helpter for search, returns empty new record if not found
Record* AVLTree::searchHelper(AVLNode* node, const std::string& key, int value) const {
    if(!node) {   //base case 1: goes down the tree until the end with no match found
        Record* newRecord = new Record("", 0);
        return newRecord;
    }
    if(node->record->key == key && node->record->value == value)    //base case 2: key/value match found
        return node->record;
    else if(value < node->record->value)    //recursive case: go down left if value lower
        return searchHelper(node->left, key, value);
    else                                    //recursive case: go down right if value higher
        return searchHelper(node->right, key, value);
}

void AVLTree::deleteNode(const std::string& key, int value) {
    root = deleteHelper(root, key, value);  //assigning the returned node allows us to keep control
                                            //on the chain of modification
    if(root) {
        updateHeight(root);
        doBalance(root);
    }

}

AVLNode* AVLTree::deleteHelper(AVLNode* node, const std::string& key, int value) {
    if(!node)   //base case 1: recursion led us to the end of tree without match 
        return nullptr;
    if(node->record->value == value && node->record->key == key) {  //base case 2: matching node found
        if(node->left && node->right) { //case 2a: node has both left and right subtrees
            AVLNode *curr = node->right;
            while(curr->left) {
                //prev = curr;  //don't need prev actually
                curr = curr->left;
            }   //at the end of the while, curr points to node's successor and prev points to curr's parent
            node->record->key = curr->record->key;  //copy key/value of successor to current node
            node->record->value = curr->record->value;
            node->right = deleteHelper(node->right, curr->record->key, curr->record->value);    //discard second copy of successor
        
        } else if(node->left || node->right) {  //case 2b: node has only left subtree, or only right subtree
            AVLNode* temp;
            if(node->left)
                temp = node->left;
            else if(node->right)
                temp = node->right;
            delete node;
            return temp;
        } else {    //case 2c: node has neither left or right, and is a leaf node
            delete node;
            return nullptr;
        }
    } else {
        if(value < node->record->value) //recursive case 3: not matching node, value is less than node
            node->left = deleteHelper(node->left, key, value);
        else if(value > node->record->value)    //recursive case 4: not matching node, value is greater than node
            node->right = deleteHelper(node->right, key, value);
        updateHeight(node);
    }
}

void AVLTree::deleteAll() {
    deleteAllHelper(root);
    root = nullptr;
}

void AVLTree::deleteAllHelper(AVLNode* node) {
    if(node) {  //recursive case: node exists, propagate to left and right branches
        deleteAllHelper(node->left);
        deleteAllHelper(node->right);
        delete node;
    }
    //base case: node does not exist/nullptr, do nothing
}

std::vector<Record*> AVLTree::inorderTraversal() const {
    return iotHelper(root);
}

//Assumes a exists
std::vector<Record*> AVLTree::iotHelper(AVLNode* a) const {
    std::vector<Record*> out = {}, temp;

    if(a->left)
        out = iotHelper(a->left);
    
    out.push_back(a->record);
    
    if(a->right) {
        temp = iotHelper(a->right);
        out.insert(out.end(), temp.begin(), temp.end());
    }
    
    return out;
}

void IndexedDatabase::insert(Record* record) {
    index.insert(record);
    std::cout << countRecords() << " ";  //DEBUG
}

Record* IndexedDatabase::search(const std::string& key, int value) const {
    return index.search(key, value);
}

void IndexedDatabase::deleteRecord(const std::string& key, int value) {
    index.deleteNode(key, value);
    std::cout << countRecords() << " ";  //DEBUG
}

std::vector<Record*> IndexedDatabase::rangeQuery(int start, int end) const {
    std::vector<Record*> output;

    return output;
}

std::vector<Record*> IndexedDatabase::findKNearestKeys(int key, int k) const {
    std::vector<Record*> output;
   
    return output; 
}

std::vector<Record*> IndexedDatabase::inorderTraversal() const {
    return index.inorderTraversal();
}
    

void IndexedDatabase::clearDatabase() {
    index.deleteAll();
}

int IndexedDatabase::countRecords() const {
    return index.inorderTraversal().size();
}

