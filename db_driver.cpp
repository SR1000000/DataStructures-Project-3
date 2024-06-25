#include "AVL_Database.hpp"
#include <cassert>
#include <iostream> //needed for cout
using namespace std;


int main() {
    int i=1;
    IndexedDatabase db;
    auto r1=new Record("a", 10);
    auto r2=new Record("b", 20);
    auto r3=new Record("d", 40);
    auto r4=new Record("e", 50);
    auto r5=new Record("fg", 70);
    auto r6=new Record("r", 88);

    // std::cout << "Inserting records...\n";
    db.insert(r1);
    db.insert(r2);
    db.insert(r3);
    db.insert(r4);
    db.insert(r5);
    db.insert(r6);

    vector<Record*> inorder_vector=db.inorderTraversal();
    vector<Record*> inorder_vector_ref={r1,r2,r3,r4,r5,r6};
    assert(inorder_vector==inorder_vector_ref);
    cout<<"Test "<<i++ <<" passed"<<endl;
   
    Record* searchResult = db.search(r2->key, r2->value);
    assert(searchResult->key==r2->key);
    assert(searchResult->value==r2->value);
    cout<<"Test "<<i++ <<" passed"<<endl;

    Record* searchResult2 = db.search("who are you", 56);
    assert(searchResult2->key=="");
    assert(searchResult2->value==0);
    cout<<"Test "<<i++ <<" passed"<<endl;

   
    db.deleteRecord(r4->key, r4->value);
    vector<Record*> ref_vector = {r1, r2, r3, r5, r6};
    vector<Record*> rec_vector=db.inorderTraversal();
    assert(ref_vector==rec_vector);
    cout<<"Test "<<i++ <<" passed"<<endl;

   
    vector<Record*> rangeQueryResult = db.rangeQuery(10, 25);
    vector<Record*> rangeQueryRef={r1,r2};
    assert(rangeQueryRef==rangeQueryResult);
    cout<<"Test "<<i++ <<" passed"<<endl;

    vector<Record*> rangeQueryResult2 = db.rangeQuery(101,250);
    vector<Record*> rangeQueryRef2={};
    assert(rangeQueryRef2==rangeQueryResult2);
    cout<<"Test "<<i++ <<" passed"<<endl;

    
    std::vector<Record*> nearestKeys = db.findKNearestKeys(12, 3);
    std::vector<Record*> nearestKeysRef = {r3,r2,r1};
    assert(nearestKeys==nearestKeysRef);
    cout<<"Test "<<i++ <<" passed"<<endl;


    std::vector<Record*> nearestKeys2 = db.findKNearestKeys(100, 2);
    std::vector<Record*> nearestKeysRef2 = {r5,r6};
    assert(nearestKeys2==nearestKeysRef2);
    cout<<"Test "<<i++ <<" passed"<<endl;
   
    // for (auto record : nearestKeys2) {
    //     std::cout << record->key << ": " << record->value << "\n";
    // }

    // std::cout << "\nClearing the database...\n";
    db.clearDatabase();
    assert(db.countRecords()==0);
    cout<<"Test "<<i++ <<" passed"<<endl;
    

    // std::cout << "\nTotal records in the database: " << db.countRecords() << "\n";

    return 0;
}
