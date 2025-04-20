#include "rbtree.h"

#include <stdlib.h>

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  // TODO: initialize struct if needed
  if (p == NULL) return NULL; // 할당 실패시 널 반환(메모리 부족 상태)
  
  node_t *n = malloc(sizeof(node_t));
  if (n == NULL) {
    free(p); // 닐 노드 실패했으니 rbtree 구조체도 반환해야함!
    return NULL;
  }

  //센티넬(닐) 노드 초기화
  n->color = RBTREE_BLACK;
  n->key = 0;
  n->left = n;
  n->parent = n;
  n->right = n;

  //닐 노드 이어주기
  p->nil = n;
  p->root = n;
  
  return p;
}

//파일 내부에서만 돌아가는 헬퍼함수
static void del_node(rbtree *t, node_t* n){ 
  if (n == t->nil){ //끝(nil)이면 그냥 리턴
    return ;
  }
  //후위 순회
  del_node(t, n->left);
  del_node(t, n->right);
  free(n);
}

void delete_rbtree(rbtree *t) {
  // TODO: reclaim the tree nodes's memory
  // 후위 순회 하면서 아래에서 위로 올라오면서 다 삭제해주기
  if (t->root == t->nil) { //빈 트리
    free(t->nil);
    free(t);
    return;
  }

  //실제 노드 해제
  del_node(t, t->root);
  
  //트리 구조체 할당 해제
  free(t->nil);
  free(t);
}

//회전 함수
void Left_Rotate(rbtree* t, node_t *x){
  node_t* y = x->right;
  x->right = y->left;
  if (y->left != t->nil){
    y->left->parent = x;
  }
  y->parent = x->parent;

  if (x->parent == t->nil){
    t->root = y;
  }
  else if( x == x->parent->left){
    x->parent->left = y;
  }
  else{
    x->parent->right = y;
  }
  y->left = x;
  x->parent = y;
}

void Right_Rotate(rbtree* t, node_t *y){
  node_t* x = y->left;
  y->left = x->right;
  if(x->right != t->nil){
    x->right->parent = y;
  }
  x->parent = y->parent;

  if (y->parent == t->nil){
    t->root = x;
  }
  else if (y == y->parent->right){
    y->parent->right = x;
  }
  else {
    y->parent->left = x;
  }
  x->right = y;
  y->parent = x;
}

//삽입 색 보정 함수
void RB_Insert_Fixup(rbtree *t, node_t *z){
  // z노드가 루트가 아니고(루트의 부모는 nil(Black)), 부모가 red 이면 반복
  while (z->parent->color == RBTREE_RED){ 
    if (z->parent == z->parent->parent->left) {
      //------------------z 부모가 왼쪽자식인가-------------------
      node_t* y = z->parent->parent->right; //y = 삼촌노드
      if (y->color == RBTREE_RED) { // 삼촌이 레드
      // case 1: 부모, 삼촌이 red -> 색 바꾸고 조부모에게 전가
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      }
      else{ //삼촌이 블랙
        if (z == z->parent->right){
        // case 2-1(2) : z가 부모의 오른쪽 자식 -> 내부(left) 회전
          z = z->parent;
          Left_Rotate(t,z);
        }
        // case 2-2(3) : z가 부모의 왼쪽 자식 -> 색 바꾸고 외부(right) 회전
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        Right_Rotate(t,z->parent->parent);
      }
    }
    else {
      // -----------------z 부모가 오른쪽 자식일때-------------
      node_t* y = z->parent->parent->left; //y = 삼촌노드
      if (y->color == RBTREE_RED) {
        // case 1
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      }
      else{
        if (z == z->parent->left) {
          // case 2-1(2)
          z = z->parent;
          Right_Rotate(t,z);
        }
        // case 2-2(3)
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        Left_Rotate(t,z->parent->parent);
      }
    }
  }
  //루트는 항상 블랙!
  t->root->color = RBTREE_BLACK;
  
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  // TODO: implement insert
  // 트리에 요소 삽입 케이스 3개 존재
  // BST 처럼 삽입 위치 찾기
  
  //삽입 노드 초기화
  node_t* z = (node_t*)malloc(sizeof(node_t)); // 삽입될 노드
  if (z == NULL) return NULL; // 할당실패시 NULL 반환
  z->key = key;

  node_t* x = t->root; // key랑 비교할 포인터
  node_t* y = t->nil; // 삽입될 노드의 parents 위치 표시 

  //위치 찾아가기
  while (x != t->nil)
  {
    y = x;
    if (z->key < x->key)
      x = x->left;
    else x = x->right;
  }
  z->parent =  y;

  //부모 자식 연결
  if (y == t->nil){ //트리가 비어있으면
    t->root = z;
  }
  else if(z->key < y->key){
    y->left = z;
  }
  else y->right = z;

  //bst 삽입 연산에서 항상 리프노드가 삽입됨 -> 자식 nil로
  z->left = t->nil;
  z->right = t->nil;
  z->color = RBTREE_RED;

  //색 보정
  RB_Insert_Fixup(t,z);

  return t->root;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  // TODO: implement find
  node_t* x = t->root;
  while (x != t->nil)
  {
    if (key == x->key){
      return x;
    }
    else if (key < x->key){
      x = x->left;
    }
    else{
      x = x->right;
    }
  }
  
  return NULL;
}

node_t *rbtree_min(const rbtree *t) {
  // TODO: implement find
  if (t->root == t->nil){ // 빈트리일때 
    return NULL;
  }
  node_t* x = t->root;
  while (x->left != t->nil){
    x = x->left;
  }
  return x;
}

node_t *rbtree_max(const rbtree *t) {
  // TODO: implement find
  if (t->root == t->nil){ // 빈트리일때 
    return NULL;
  }
  node_t* x = t->root;
  while (x->right != t->nil){
    x = x->right;
  }
  return x;
}

//서브트리의 최솟값 구하기 -> successor 구하는 함수
static node_t *subtree_min(const rbtree *t, node_t *x) {
  while (x->left != t->nil) {
    x = x->left;
  }
  return x;
}

// 노드 u를 제거하고 그 자리에 v(서브트리)를 이식하여 부모 포인터를 갱신
void RB_Transplant(rbtree* t, node_t* u, node_t* v){
  
  if (u->parent == t->nil){
    //u가 루트면, v를 새 루트로
    t->root = v;
  }
  else if(u == u->parent->left){ 
    //u가 부모의 왼쪽 자식일 때
    u->parent->left = v;
  }
  else{
    //u가 부모의 오른쪽 자식일 때
    u->parent->right = v;
  }
  //v가 이 자리에 연결되었으니, v의 부모 포인터 갱신
  v->parent = u->parent;
}

//삭제 색 보정 함수
void RB_Delete_Fixup(rbtree *t, node_t *x){
  node_t* w;
  while (x != t->root && x->color == RBTREE_BLACK){
    if (x == x->parent->left){ // x가 왼쪽 자식인가?
      w = x->parent->right; // w는 x의 형제
      if (w->color == RBTREE_RED){
        //case 1 : 형제가 red
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        Left_Rotate(t,x->parent);
        w = x->parent->right;
      }
      if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK){
      //case 2 : 형제의 두 자식 다 black
        w->color = RBTREE_RED;
        x = x->parent;
      }
      else{
        if (w->right->color == RBTREE_BLACK){
          //case 3 : 가까운 자식이 red, 먼 자식이 balck
          w->left->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          Right_Rotate(t,w);
          w = x->parent->right;
        }
        //case 4 : 가까운 자식이 black, 먼 자식이 red
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->right->color = RBTREE_BLACK;
        Left_Rotate(t,x->parent);
        x = t->root;
      }   
    }
    else{ // mirror : x가 오른쪽 자식인가?
      w = x->parent->left; // w는 x의 형제
      if (w->color == RBTREE_RED){
        //case 1 : 형제가 red
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        Right_Rotate(t,x->parent);
        w = x->parent->left;
      }
      if (w->right->color == RBTREE_BLACK && w->left->color == RBTREE_BLACK){
      //case 2 : 형제의 두 자식 다 black
        w->color = RBTREE_RED;
        x = x->parent;
      }
      else{
        if (w->left->color == RBTREE_BLACK){
          //case 3 : 가까운 자식이 red, 먼 자식이 balck
          w->right->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          Left_Rotate(t,w);
          w = x->parent->left;
        }
        //case 4 : 가까운 자식이 black, 먼 자식이 red
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->left->color = RBTREE_BLACK;
        Right_Rotate(t,x->parent);
        x = t->root;
      }   
    }
  }
  // 루트는 항상 블랙!
  x->color = RBTREE_BLACK;
}

//BST 방식 노드 삭제 진행
int rbtree_erase(rbtree *t, node_t *p) {
  // TODO: implement erase
  // p : 삭제 대상 노드, y : 실제로 트리에서 꺼내질 노드, x : y 빠진 위치에 대체될 노드
  node_t* y = p;
  node_t* x;
  int y_original_color = y->color; // 색 저장해 블랙노드 삭제 여부 판단
  if (p->left == t->nil){
    // ---case3 : 왼쪽 자식이 없을 때---
    x = p->right;
    RB_Transplant(t,p,p->right); // p를 오른쪽 자식으로 바꾼다
  }
  else if (p->right == t->nil){
    // ---case2 : 오른쪽 자식이 없을 때 ---
    x = p->left;
    RB_Transplant(t,p,p->left); // p를 왼쪽 자식으로 바꾼다
  }
  else{
    // ---case 3 : 두 자식이 모두 있을 때---
    y = subtree_min(t, p->right);
    y_original_color = y->color;
    x = y->right;

    if (y != p->right){
      RB_Transplant(t,y,y->right);
      y->right = p->right;
      y->right->parent = y;
    }
    else{
      x->parent = y;
    }

    RB_Transplant(t,p,y);
    y->left = p->left;
    y->left->parent = y;
    y->color = p->color;
  }

  // 삭제된 노드가 블랙이면 보정 진행
  if (y_original_color == RBTREE_BLACK){
    RB_Delete_Fixup(t,x);
  }

  //메모리 해제
  free(p);
  return 0;
}

static size_t insert_arr(const rbtree *t, node_t* x, key_t *arr, size_t idx, const size_t n){
  if (x == t->nil || idx >= n){ //n 넘으면 바로 리턴
    return idx;
  }
  //중위 순회
  idx = insert_arr(t,x->left, arr, idx, n);
  arr[idx] = x->key;
  idx ++;
  return insert_arr(t,x->right, arr, idx, n);
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  // 중위 순회 하면서 rbtree 키를 array에 삽입
  node_t *x = t->root;

  //빈 트리일 경우 바로 리턴
  if (x == t->nil){
    return 0;
  }

  // 실제 채워 넣을 수 있는 최대 n을 넘지 않도록
  size_t filled = insert_arr(t, x, arr, 0, n);

  return filled;
}