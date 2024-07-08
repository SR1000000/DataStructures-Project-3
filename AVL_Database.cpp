#include "AVL_Database.hpp"
#include <cstdlib>   // for abs() in doBalance()
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

/*  Check balance of node, balancing it with rotations if required.  Assumes node exists (would be simple to change so this assumption is not necessary).
    Recursive function, gets passed node to balance and returns balanced node.
*/
AVLNode* AVLTree::doBalance(AVLNode* a) {
    //recursively propagates down whichever side is unbalanced until it fails (can't go further), at which point it is at a the source of the unbalance
    //recursive case 1: unbalanced left or right child
    if(abs(balance(a->left)) > 1) { 
        a->left = doBalance(a->left);
    }
    if(abs(balance(a->right)) > 1) {
        a->right = doBalance(a->right);
    }
    
    //base case 2: at the source of the unbalance, rotate to eliminate unbalance
    //recursive case 1a: if/when rotation of children from recursive calls are done, check if this node is still unbalanced and balance if necessary.
    if(balance(a) > 1) {    //unbalanced to the left
        if(balance(a->left) >= 0)   //two if cases filter to unbalanced on single side, only one rotation necessary
            return rotateRight(a);
        else {  //else is unbalanced to the left but left child is unbalanced to the right, which requires two rotations
            a->left = rotateLeft(a->left);
            return rotateRight(a);
        }
    } else if(balance(a) < -1) {    //unbalanced to the right
        if(balance(a->right) <=0)   //two if cases filter to unbalanced on signel side, only one rotation necessary
            return rotateLeft(a);
        else {  //else is unbalanced to the right but right child is unbalanced to the left, which requires two rotations
            a->right = rotateRight(a->right);
            return rotateLeft(a);
        }
    } else {    //else not unbalanced (perfectly balanced -Thanos)
        updateHeight(a);    //necessary to propagate height changes from rebalance up the tree during Recursive case 1 doing balance down the tree but node itself is still balanced
        return a;
    }
}

//Assumes node exists, designed to be used in recursive functions to propagate in post-order fashion.  Could be changed so assumption not necessary and be solidly recursive-friendly
void AVLTree::updateHeight(AVLNode* node) {
    if(height(node->left) >= height(node->right))   //correctly calculates height even with null nodes in left or right or both
        node->height = height(node->left) + 1;
    else
        node->height = height(node->right) + 1;
}

//Assumes y->left exists (a good assumption since this is (only?) called when y is unbalanced to the left)
AVLNode* AVLTree::rotateRight(AVLNode* y) {
    //setup temp pointers prior to performing rotation
    AVLNode* yLeft = y->left;
    AVLNode* yLeftRight;
    if(y->left)
        yLeftRight = yLeft->right;
    else    
        std::cout << "Error: Expected y->left exist but not.";
    //perform rotation with temp pointer assistance
    yLeft->right = y;
    y->left = yLeftRight;

    //update the two heights necessary (yLeftRight not necessary because its children aren't modified)
    updateHeight(y);
    updateHeight(yLeft);

    return yLeft;   //was passed node, now return correctly rotated node
}

//Assumes x->right exists (a good assumption since this is (only?) called when y is unbalanced to the right)
AVLNode* AVLTree::rotateLeft(AVLNode* x) {
    //setup temp pointers prior to performing rotation
    AVLNode* xRight = x->right;
    AVLNode* xRightLeft;
    if(x->right)
        xRightLeft = xRight->left;
    else    
        std::cout << "Error: Expected x->right exist but not.";
    //perform rotation with temp pointer assistance
    xRight->left = x;
    x->right = xRightLeft;

    //update the two heights necessary (xRightLeft not necessary because its children aren't modified)
    updateHeight(x);
    updateHeight(xRight);

    return xRight;  //was passed node, now return correctly rotated node
}



void AVLTree::insert(Record* record) {
    //you can make a private memeber function to handle insertion.
    //e.g root = insertHelper(root, record);
    root = insertHelper(root, record);  
    root = doBalance(root); //balance with recursive function
}

//Recursively travels down the tree based on value until it reaches a null (and thus available) node and places new node there.  Also updates height up the tree as part of recursion.
AVLNode* AVLTree::insertHelper(AVLNode* node, Record* r) {
    if(!node) { //base case 1: called on empty node
        AVLNode* newNode = new AVLNode(r);  //create the new node with passed record
        return newNode; //return the new node for parent to attach
    } else {    //recursive case 2 & 3
        if(node->record->value > r->value) {    //recursive case 2: r's value is less than node's record's value
            node->left = insertHelper(node->left, r);   //propagate insertion down node's left side
            if(height(node->left) >= height(node))  //updates height up the entire chain if necessary (doesn't update height if height was added to the lesser side)
                node->height++; //(by adding to self node's height if insertion down the side added to height)
        } else {    //recursive case 3: r's value is larger than (or equal to, but that doesn't happen) node's record's value
            node->right = insertHelper(node->right, r); //propagate insertion down node's right side
            if(height(node->right) >= height(node)) //updates height up the entire chain if necessary
                node->height++;
        }
        return node;    //return self node (with possibly updated height) for recursion
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

//Recursive helper for deleteNode.  Note: returns the node/subtree that should be there after deletion, as expected by caller
AVLNode* AVLTree::deleteHelper(AVLNode* node, const std::string& key, int value) {
    if(!node)   //base case 1: recursion led us to the end of tree without match 
        return nullptr; //node is nullptr, return also nullptr
    if(node->record->value == value && node->record->key == key) {  //base case 2: matching node found
        if(node->left && node->right) { //base case 2a: node has both left and right subtrees
            AVLNode *curr = node->right;
            while(curr->left) {
                //prev = curr;  //don't need prev actually
                curr = curr->left;
            }   //at the end of the while, curr points to node's successor (by value) //and prev points to curr's parent
            node->record = curr->record;  //point/replace node record to record of successor (curr)
            node->right = deleteHelper(node->right, curr->record->key, curr->record->value);    //delete original successor
            updateHeight(node); //update height after successor node deleted
            return node;    //return node that has been replaced by successor values
        } else if(node->left || node->right) {  //base case 2b: node has only left subtree, or only right subtree
            AVLNode* temp;  //use temp to skip node and point to singular child
            if(node->left)
                temp = node->left;
            else if(node->right)
                temp = node->right;
            delete node;
            //no updateHeight necessary, temp's height does not change and caller will propagate height changes
            return temp;    //return skipped-to node 
        } else {    //base case 2c: node has neither left or right, and is a leaf node
            delete node;
            return nullptr;
        }
    } else {    //else node is not matching case
        if(value < node->record->value) //recursive case 3: not matching node, value is less than node
            node->left = deleteHelper(node->left, key, value);
        else if(value > node->record->value)    //recursive case 4: not matching node, value is greater than node
            node->right = deleteHelper(node->right, key, value);
        updateHeight(node); //in case delete worked further down the recursion chain, propagate height changes post-order
        return node;
    }
}

void AVLTree::deleteAll() {
    deleteAllHelper(root);
    root = nullptr;
}

//Helper function that lets public deleteAll function access private root
void AVLTree::deleteAllHelper(AVLNode* node) {
    if(node) {  //recursive case: node exists, propagate to left and right branches
        deleteAllHelper(node->left);
        deleteAllHelper(node->right);
        delete node;    //delete in post-order fashion
    }
    //base case: node does not exist/nullptr, do nothing
}

std::vector<Record*> AVLTree::inorderTraversal() const {
    return iotHelper(root);
}

//Recursive helper function that lets public function access private root
std::vector<Record*> AVLTree::iotHelper(AVLNode* a) const {
    std::vector<Record*> out = {};  //need initalize to empty vector for base recursion cases
    
    if(!a)  //base case 1: a is nullptr, also happens when root is nullptr
        return out; //return empty vector

    //else recursive case 2: a exists, then propagate recursively down left and right sides (in in-order traversal)
    if(a->left)
        out = iotHelper(a->left);   //first set output vector to returned left-side vector
    
    out.push_back(a->record);   //second add a's record to output vector
    
    if(a->right) {
        std::vector<Record*> temp = iotHelper(a->right); //need temp as second vector to allow insert() to combine two vectors
        out.insert(out.end(), temp.begin(), temp.end());    //third add returned right-side vector (temp) to output vector
    }

    return out;
}

std::vector<Record*> AVLTree::rangeQuery(int start, int end) const {
    return rangeQueryHelper(root, start, end);
}

//Recursive helper function that lets public function access private root
//basically copied from iotHelper() and modified to only work in given range
std::vector<Record*> AVLTree::rangeQueryHelper(AVLNode* a, int start, int end) const {
    std::vector<Record*> out = {};
    
    if(!a)  //base case 1: a is nullptr
        return out; //return empty vector

    //recursive case 2: a exists, then rangequery further down either, both, or neither path as necessary 
    if(a->record->value > start)
        out = rangeQueryHelper(a->left, start, end);    //first set output vector to returned left-side vector
    
    if(a->record->value >= start && a->record->value <= end)
        out.push_back(a->record);   //second add a's record to output vector (if a's record falls within range)

    if(a->record->value < end) {
        std::vector<Record*> temp = rangeQueryHelper(a->right, start, end); //need temp as second vector to allow insert() to combine two vectors
        out.insert(out.end(), temp.begin(), temp.end());    //third add returned right-side vector (temp) to output vector
    }

    return out;
}

//Returns a vector with k elements, of the k nearest records to the value of 'key'
std::vector<Record*> AVLTree::findKNearestKeys(int key, int k) const {
    std::priority_queue<Record*,std::vector<Record*>, RecordPtrCmp1> below; //maxheap because RecordPtrCmp1 is greater than operator function pointer
    std::priority_queue<Record*,std::vector<Record*>, RecordPtrCmp2> above; //minheap because RecordPtrCmp2 is less than operator function pointer
    //tbh, heaps aren't really necessary and simple queue would work just as well
    //since inorderTraversal() already gives a sorted vector of records.  If it didn't, though...

    std::vector<Record*> temp = inorderTraversal(), out = {};   //initialize temp to all records in tree
    
    for (auto i : temp) {
        if(i->value < key)
            below.push(i);  //all records lower than key pushed into below
        if(i->value > key)
            above.push(i);  //all records higher than key pushed into above
    }
    
    while(!below.empty() && !above.empty() && k>0) {    //while both below and above non-empty (and k not exhausted (== 0))
        if( (above.top()->value - key) < (key - below.top()->value)) {  //if above's top is closer than below's top to key
            out.push_back(above.top());
            above.pop();
        } else {
            out.push_back(below.top());
            below.pop();
        }
        k--;    //remember to count only k records
    }
    while(!below.empty() && k>0) {  //one of above/below is empty, thus fill from the non-empty one until k==0
        out.push_back(below.top());
        below.pop();
        k--;
    }
    while(!above.empty() && k>0) {  //one of above/below is empty, thus fill from the non-empty one until k==0
        out.push_back(above.top());
        above.pop();
        k--;
    }

    std::reverse(out.begin(), out.end());   //db_driver wants vector of records in reverse order of pushed order
    return out;
}

void IndexedDatabase::insert(Record* record) {
    index.insert(record);   //call insert on db's tree
    //std::cout << countRecords() << " ";  //DEBUG
}

Record* IndexedDatabase::search(const std::string& key, int value) const {
    return index.search(key, value);    //call search on db's tree
}

void IndexedDatabase::deleteRecord(const std::string& key, int value) {
    index.deleteNode(key, value);   //call delete on db's tree
    //std::cout << countRecords() << " ";  //DEBUG
}

std::vector<Record*> IndexedDatabase::rangeQuery(int start, int end) const {
    return index.rangeQuery(start, end);    //call on db's tree
}

std::vector<Record*> IndexedDatabase::findKNearestKeys(int key, int k) const {
    return index.findKNearestKeys(key, k);  //call on db's tree
}

std::vector<Record*> IndexedDatabase::inorderTraversal() const {
    return index.inorderTraversal();    //call on db's tree
}
    

void IndexedDatabase::clearDatabase() {
    index.deleteAll();  //call on db's tree
}

//should add size variable to IndexedDatabase and update variable on delete/insert for O(1) instead of current O(n)
int IndexedDatabase::countRecords() const {
    return index.inorderTraversal().size(); //call on db's tree, using size() method of returned vector
}

