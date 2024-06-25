#ifndef AVL_DATABASE_HPP
#define AVL_DATABASE_HPP

#include <string>
#include <vector>
#include <queue>

class Record {
public:
    std::string key;
    int value;

    Record(const std::string& k, int v);
};

class AVLNode {
public:
    Record* record;
    AVLNode* left;
    AVLNode* right;
    int height;

    AVLNode(Record* r);
};

class AVLTree {
private:
    AVLNode* root;

    int height(AVLNode* node);
    int balance(AVLNode* node);
    
    AVLNode* rotateRight(AVLNode* y);
    AVLNode* rotateLeft(AVLNode* x);
    
    AVLNode* doBalance(AVLNode* a);
    int updateHeight(AVLNode* a);

    AVLNode* insertHelper(AVLNode* node, Record* r);

public:
    AVLTree();
    void insert(Record* record);
    Record* search(const std::string& key, int value);
   
    void deleteNode(const std::string& key, int value);

};

class IndexedDatabase {
private:
    AVLTree index;

public:
    void insert(Record* record);
    Record* search(const std::string& key, int value);
    void deleteRecord(const std::string& key, int value);
    std::vector<Record*> rangeQuery(int start, int end);
    std::vector<Record*> findKNearestKeys(int key, int k);
    std::vector<Record*> inorderTraversal();
    void clearDatabase();
    int countRecords();
};

#endif // AVL_DATABASE_HPP