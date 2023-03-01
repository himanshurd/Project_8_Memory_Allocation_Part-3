#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>

#define ALIGNMENT 16   // Must be power of 2
#define GET_PAD(x) ((ALIGNMENT - 1) - ((x - 1) & (ALIGNMENT - 1)))
#define PADDED_SIZE(x) ((x) + GET_PAD(x))
#define PTR_OFFSET(p, offset) ((void*)((char *)(p) + (offset)))

void print_data(void);
void *myalloc(int bytes);

struct block {
    int in_use;
    int size;
    struct block *next;
};
struct block *head = NULL;

// Split_Space(current_node, requested_size):
void split_space(struct block *cur, int padded_size){
  int block_padded_size = PADDED_SIZE(sizeof(struct block)); 
  int cur_size= cur->size;
  int remaining_free_space = cur_size - padded_size - block_padded_size;
  int room_to_split = remaining_free_space>=16; 

  if (room_to_split){
    struct block *new = PTR_OFFSET(cur, padded_size + block_padded_size);           
    new ->size = cur->size - padded_size - block_padded_size;
    cur->size = padded_size;
    new->in_use = 0;
    new->next = cur->next; 
    cur->next = new; 
  }
}

void myfree(void *p) {    
  int block_padded_size = PADDED_SIZE(sizeof(struct block));
  struct block *b = PTR_OFFSET(p, -block_padded_size);    
  b -> in_use = 0;
                                                         
  struct block *cur = head;                              
  while(cur->next!=NULL){                                
    if((!cur->in_use)&&(!cur->next->in_use)){            
      cur->size += cur->next->size + block_padded_size;  
      cur->next=cur->next->next;                         
      }
    else{                                             
      cur = cur->next;                                   
    }
  }
}

void *myalloc(int bytes){
  
  if (head == NULL){
    head = mmap(NULL, 1024, PROT_READ|PROT_WRITE,
                    MAP_ANON|MAP_PRIVATE, -1, 0);;
    head->next = NULL;
    head->size = 1024 - PADDED_SIZE(sizeof(struct block));
    head->in_use = 0;
  }

  int padded_size = PADDED_SIZE(bytes);
  int padded_block_size = PADDED_SIZE(sizeof(struct block));
  struct block *cur = head;
  
  while(cur != NULL){
    if ((!cur->in_use)&&(cur->size>=padded_size)){
      split_space(cur, padded_size);
      cur->in_use = 1;
      int padded_block_size = PADDED_SIZE(sizeof(struct block));
      return PTR_OFFSET(cur, padded_block_size);
    }
    cur = cur->next;
  }
  return NULL;
}

void print_data(void)
{
  struct block *b = head;
  if (b == NULL) {
    printf("[empty]\n");
    return;
  }
  while (b != NULL) {
    // Uncomment the following line if you want to see the pointer values
    //printf("[%p:%d,%s]", b, b->size, b->in_use? "used": "free");
    printf("[%d,%s]", b->size, b->in_use? "used": "free");
    if (b->next != NULL) {
      printf(" -> ");
    }
    b = b->next;
  }
  printf("\n");
}

int main(void) {

  // void *p;  
  // p = myalloc(10); print_data();
  // myfree(p); print_data();


  // void *p, *q;
  // p = myalloc(10); print_data();
  // q = myalloc(20); print_data();
  // myfree(p); print_data();
  // myfree(q); print_data();


  // void *p, *q;
  // p = myalloc(10); print_data();
  // q = myalloc(20); print_data();
  // myfree(q); print_data();
  // myfree(p); print_data();


  void *p, *q, *r, *s;
  p = myalloc(10); print_data();
  q = myalloc(20); print_data();
  r = myalloc(30); print_data();
  s = myalloc(40); print_data();
  myfree(q); print_data();
  myfree(p); print_data();
  myfree(s); print_data();
  myfree(r); print_data();
}
