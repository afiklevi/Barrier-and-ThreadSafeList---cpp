#ifndef THREAD_SAFE_LIST_H_
#define THREAD_SAFE_LIST_H_

#include <pthread.h>
#include <iostream>
#include <iomanip> // std::setw

using namespace std;

template <typename T>
class List
{
public:
    /**
     * Constructor
     */
    List(): head(new Node(0)), size(0){pthread_mutex_init(&general_mutex,NULL);}

    /**
     * Destructor
     */
    ~List(){
        Node* curr_node = head;
        while(curr_node){
            pthread_mutex_destroy(&curr_node->node_mutex);
            Node* next_node = curr_node->next;
            delete curr_node;
            curr_node=next_node;
        }
        pthread_mutex_destroy(&general_mutex);
    }

    class Node {
    public:
        T data;
        Node *next;
        pthread_mutex_t node_mutex;
        explicit Node(const T &data): data(data), next(nullptr){pthread_mutex_init(&node_mutex,NULL);};
        ~Node() = default;
        void updateNext(Node *next){
            this->next = next;
        }
        Node *getNext() const {
            return next;
        }
    };



    bool insert(const T& data) {
        Node* new_node = new Node (data);
        Node* pred = head;
        pthread_mutex_lock(&pred->node_mutex);
        Node* curr = pred->next;

        if (curr){
            pthread_mutex_lock(&curr->node_mutex);
        }

        while (curr){
            if (curr->data >= data){
                if (curr->data == data){
                    pthread_mutex_unlock(&pred->node_mutex);
                    pthread_mutex_unlock(&curr->node_mutex);
                    return false;
                }
                break;
            }
            Node* prev_pred = pred;
            pred = curr;
            curr = curr->next;
            if (curr){
                pthread_mutex_lock(&curr->node_mutex);
            }
            pthread_mutex_unlock(&prev_pred->node_mutex);

        }

        pthread_mutex_lock(&general_mutex);
        new_node->updateNext(curr);
        pred->updateNext(new_node);
        size++;
        pthread_mutex_unlock(&general_mutex);
        __insert_test_hook();
        pthread_mutex_unlock(&pred->node_mutex);
        if (curr){
            pthread_mutex_unlock(&curr->node_mutex);
        }
        return true;
    }

    /**
     * Remove the node that its data equals to @param value
     * @param value the data to lookup a node that has the same data to be removed
     * @return true if a matched node was found and removed and false otherwise
     */
    bool remove(const T& value) {
        Node* pred = head;
        pthread_mutex_lock(&pred->node_mutex);
        Node* curr = pred->next;

        if (curr){
            pthread_mutex_lock(&curr->node_mutex);
        }

        while (curr) {
            if (curr->data == value){
                pred->next = curr->next;
                __remove_test_hook();
                pthread_mutex_lock(&general_mutex);
                size--;
                pthread_mutex_unlock(&general_mutex);
                pthread_mutex_unlock(&pred->node_mutex);
                pthread_mutex_unlock(&curr->node_mutex);

                return true;
            }
            Node* prev_pred = pred;
            pred = curr;
            curr = curr->next;
            pthread_mutex_unlock(&prev_pred->node_mutex);
            if (curr){
                pthread_mutex_lock(&curr->node_mutex);
            }
        }
        pthread_mutex_unlock(&pred->node_mutex);
        return false;
    }

    /**
     * Returns the current size of the list
     * @return current size of the list
     */
    unsigned int getSize() {
        return size;
    }

    // Don't remove
    void print() {
        Node* temp = head->getNext();
        if (temp == NULL)
        {
            cout << "";
        }
        else if (temp->next == NULL)
        {
            cout << temp->data;
        }
        else
        {
            while (temp != NULL)
            {
                cout << right << setw(3) << temp->data;


                temp = temp->next;
                cout << " ";
            }
        }
        cout << endl;
    }

    // Don't remove
    virtual void __insert_test_hook() {}
    // Don't remove
    virtual void __remove_test_hook() {}


    

    Node* head;
    unsigned int size;
    pthread_mutex_t general_mutex;

private:

    /**
     * function to search if a node exist in the list;
     * @param data - the data of the node
     * @return reference to the node if exitst, nullptr otherwise
     */
    Node* search(const T& data){
        for (Node* iterator = head ; iterator ; iterator = iterator->next){
            if (iterator->data == data) return iterator;
        }
        return nullptr;

    }

};



#endif //THREAD_SAFE_LIST_H_