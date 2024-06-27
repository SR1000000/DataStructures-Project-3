#ifndef AVL_DATABASE_HPP
#define AVL_DATABASE_HPP

#include <string>
#include <vector>
//#include <queue>

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

    int height(AVLNode* node) const;
    int balance(AVLNode* node) const;
    
    AVLNode* rotateRight(AVLNode* y);
    AVLNode* rotateLeft(AVLNode* x);
    AVLNode* doBalance(AVLNode* a);

    void updateHeight(AVLNode* a);

    AVLNode* insertHelper(AVLNode* node, Record* r);
    Record* searchHelper(AVLNode* node, const std::string& key, int value) const;
    std::vector<Record*> iotHelper(AVLNode* a) const;
    AVLNode* deleteHelper(AVLNode* node, const std::string& key, int value);

public:
    AVLTree();
    void insert(Record* record);
    Record* search(const std::string& key, int value) const;
    std::vector<Record*> inorderTraversal() const;
    void deleteNode(const std::string& key, int value);
    void deleteAll();
    void deleteAllHelper(AVLNode* node);

};

class IndexedDatabase {
private:
    AVLTree index;

public:
    void insert(Record* record);
    Record* search(const std::string& key, int value) const;
    void deleteRecord(const std::string& key, int value);
    std::vector<Record*> rangeQuery(int start, int end) const;
    std::vector<Record*> findKNearestKeys(int key, int k) const;
    std::vector<Record*> inorderTraversal() const;
    void clearDatabase();
    int countRecords() const;
};

#endif // AVL_DATABASE_HPP