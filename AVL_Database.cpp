#include "AVL_Database.hpp"

Record::Record(const std::string& k, int v) : key(k), value(v) {}

AVLNode::AVLNode(Record* r) : record(r), left(nullptr), right(nullptr), height(1) {}

AVLTree::AVLTree() : root(nullptr) {}

int AVLTree::height(AVLNode* node) {
    return node ? node->height : 0;
}

int AVLTree::balance(AVLNode* node) {
    return node ? height(node->left) - height(node->right) : 0;
}


AVLNode* AVLTree::rotateRight(AVLNode* y) {

}

AVLNode* AVLTree::rotateLeft(AVLNode* x) {

}



void AVLTree::insert(Record* record) {
    //you can make a private memeber function to handle insertion.
    //e.g root = insertHelper(root, record);

}

Record* AVLTree::search(const std::string& key, int value) {

}



void AVLTree::deleteNode(const std::string& key, int value) {
    
}





void IndexedDatabase::insert(Record* record) {
   
}

Record* IndexedDatabase::search(const std::string& key, int value) {
    
}

void IndexedDatabase::deleteRecord(const std::string& key, int value) {
    
}

std::vector<Record*> IndexedDatabase::rangeQuery(int start, int end) {
   
}

std::vector<Record*> IndexedDatabase::findKNearestKeys(int key, int k) {
    
}

std::vector<Record*> IndexedDatabase::inorderTraversal() {
   
}

void IndexedDatabase::clearDatabase() {
   
}

int IndexedDatabase::countRecords() {
   
}

