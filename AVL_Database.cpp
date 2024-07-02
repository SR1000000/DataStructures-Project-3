#include "AVL_Database.hpp"
#include <cstdlib>   // for abs()
#include <iostream>
#include <queue>   //for priority_queue (heaps) in findKNearestKeys()
#include <algorithm>    //for reverse() in findKNearestKeys()

Record::Record(const std::string& k, int v) : key(k), value(v) {}

AVLNode::AVLNode(Record* r) : record(r), left(nullptr), right(nullptr), height(1) {}

AVLTree::AVLTree() : root(nullptr) {}

int AVLTree::height(AVLNode* node) const {
    return node ? node->height : 0;
}

int AVLTree::balance(AVLNode* node) const {
    return node ? height(node->left) - height(node->right) : 0;
}

//Check balance of node, balancing it with rotations if required.  Assumes node exists
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
        //updateHeight(root);
        doBalance(root);
    }

}

//Note: returns the node/subtree that should be there after deletion, as expected by caller
AVLNode* AVLTree::deleteHelper(AVLNode* node, const std::string& key, int value) {
    if(!node)   //base case 1: recursion led us to the end of tree without match 
        return nullptr; //node is nullptr, return also nullptr
    if(node->record->value == value && node->record->key == key) {  //base case 2: matching node found
        if(node->left && node->right) { //case 2a: node has both left and right subtrees
            AVLNode *curr = node->right;
            while(curr->left) {
                //prev = curr;  //don't need prev actually
                curr = curr->left;
            }   //at the end of the while, curr points to node's successor (by value) and prev points to curr's parent
            node->record = curr->record;  //point/replace node record to record of successor (curr)
            node->right = deleteHelper(node->right, curr->record->key, curr->record->value);    //delete original successor
            updateHeight(node); //update height after successor node deleted
            return node;    //return node that has been replaced by successor values
        } else if(node->left || node->right) {  //case 2b: node has only left subtree, or only right subtree
            AVLNode* temp;  //use temp to skip node and point to singular child
            if(node->left)
                temp = node->left;
            else if(node->right)
                temp = node->right;
            delete node;
            //no updateHeight necessary, temp's height does not change and caller will propagate height changes
            return temp;    //return skipped-to node 
        } else {    //case 2c: node has neither left or right, and is a leaf node
            delete node;
            return nullptr;
        }
    } else {    //else node is not matching case
        if(value < node->record->value) //recursive case 3: not matching node, value is less than node
            node->left = deleteHelper(node->left, key, value);
        else if(value > node->record->value)    //recursive case 4: not matching node, value is greater than node
            node->right = deleteHelper(node->right, key, value);
        updateHeight(node); //in case delete worked further down the recursion chain, propagate height changes upwards
        return node;
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

std::vector<Record*> AVLTree::iotHelper(AVLNode* a) const {
    std::vector<Record*> out = {}, temp;
    
    if(!a)  //if a is nullptr, happens when root is nullptr
        return out; //return empty vector

    if(a->left)
        out = iotHelper(a->left);
    
    out.push_back(a->record);
    
    if(a->right) {
        temp = iotHelper(a->right); //need temp as second vector to allow insert() to combine two vectors
        out.insert(out.end(), temp.begin(), temp.end());
    }

    return out;
}

std::vector<Record*> AVLTree::rangeQuery(int start, int end) const {
    return rangeQueryHelper(root, start, end);
}

std::vector<Record*> AVLTree::rangeQueryHelper(AVLNode* a, int start, int end) const {
    std::vector<Record*> out = {}, temp;
    
    if(!a)  //base case 1: a is nullptr
        return out; //return empty vector

    //recursive case 2: a exists, then rangequery further down either, both, or neither path as necessary 
    if(a->record->value > start)
        out = rangeQueryHelper(a->left, start, end);
    
    if(a->record->value >= start && a->record->value <= end)
        out.push_back(a->record);

    if(a->record->value < end) {
        temp = rangeQueryHelper(a->right, start, end); //need temp as second vector to allow insert() to combine two vectors
        out.insert(out.end(), temp.begin(), temp.end());
    }

    return out;
}

std::vector<Record*> AVLTree::findKNearestKeys(int key, int k) const {
    std::priority_queue<Record*,std::vector<Record*>, RecordPtrCmp1> below; //maxheap
    std::priority_queue<Record*,std::vector<Record*>, RecordPtrCmp2> above; //minheap
    //tbh, heaps aren't really necessary and simple queue would work just as well
    //since inorderTraversal() already gives a sorted vector of records.  If it didn't, though...

    std::vector<Record*> temp = inorderTraversal(), out = {};
    
    for (auto i : temp) {
        if(i->value < key)
            below.push(i);  //all records lower than key pushed into below
        if(i->value > key)
            above.push(i);  //all records higher than key pushed into above
    }
    
    while(!below.empty() && !above.empty() && k>0) {    //if both below and above non-empty
        if( (above.top()->value - key) < (key - below.top()->value)) {  //if above's top is closer than below's top to key
            out.push_back(above.top());
            above.pop();
        } else {
            out.push_back(below.top());
            below.pop();
        }
        k--;    //remember to count only k records
    }
    while(!below.empty() && k>0) {  //one of above/below is empty, thus fill from the non-empty one
        out.push_back(below.top());
        below.pop();
        k--;
    }
    while(!above.empty() && k>0) {  //one of above/below is empty, thus fill from the non-empty one
        out.push_back(above.top());
        above.pop();
        k--;
    }

    std::reverse(out.begin(), out.end());   //db_driver wants vector of records in reverse order of pushed order
    return out;
}

std::vector<Record*> AVLTree::fKNKHelper(AVLNode* a, int key, int k) const {
    return {};
}

void IndexedDatabase::insert(Record* record) {
    index.insert(record);
    //std::cout << countRecords() << " ";  //DEBUG
}

Record* IndexedDatabase::search(const std::string& key, int value) const {
    return index.search(key, value);
}

void IndexedDatabase::deleteRecord(const std::string& key, int value) {
    index.deleteNode(key, value);
    //std::cout << countRecords() << " ";  //DEBUG
}

std::vector<Record*> IndexedDatabase::rangeQuery(int start, int end) const {
    return index.rangeQuery(start, end);
}

std::vector<Record*> IndexedDatabase::findKNearestKeys(int key, int k) const {
    return index.findKNearestKeys(key, k); 
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

