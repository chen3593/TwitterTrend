/*Queue - Linked List implementation
Source: https://gist.github.com/mycodeschool/7510222
*/
#include<stdio.h>
#include<stdlib.h>

struct Node {
	int clientFD;
 	struct sockaddr_in clientAddr;
	struct Node* next;
};
// Two glboal variables to store address of front and rear nodes. 
struct Node* front = NULL;
struct Node* rear = NULL;

// To Enqueue an str
void Enqueue(struct Node * x) {
	struct Node* temp = (struct Node*)malloc(sizeof(struct Node));
	temp->clientFD = x->clientFD; 
    memcpy (&(temp->clientAddr), &(x->clientAddr), sizeof (struct sockaddr_in));
	temp->next = NULL;
	if(front == NULL && rear == NULL){
		front = rear = temp;
		return;
	}
	rear->next = temp;
	rear = temp;
}

int Size() {
	struct Node* temp = front;
    int count = 0;
    while(temp != NULL) {
        temp = temp->next;
        count++;
    }
    return count;
}

// To Dequeue an element
struct Node* Dequeue() {
	struct Node* temp = front;
	if(front == NULL) {
		printf("Queue is Empty\n");
		return;
	}
	if(front == rear) {
		front = rear = NULL;
	}
	else {
		front = front->next;
	}
	return temp;
}



