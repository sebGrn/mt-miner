#ifndef  _BASIC_H
#define _BASIC_H

//# define integer short
# define integer int
# define FALSE 0
# define TRUE 1
# define Boolean int
# define INFNTY_INT 100000000
# define INFNTY_DOUBLE 10E10
# define EPSILON 10E-10
# define MAXALLOC 100000

#include <iostream>

class Vector;
class Box;
class Problem;
class System;
class IndexArray;

extern Boolean debug;

extern double numNodes; // num of recursive calls
extern double numLeaves; // num of subproblems of volume <=1
extern double clk; // clock
extern double clockMax;
extern Boolean clockPos;
extern double interTimeAvg; // Avg. time between two succisevely generated points 
extern double interTimeAvg2; // Avg. time between two succisevely generated points 
extern double interTimeMax; // Max. time between two succisevely generated points 
extern double interTimeMax2; // Max. time between two succisevely generated points 
extern double interTimeMax22; // Max. time between two succisevely generated points 
extern double interTimeTot; //sum of Tot times between two succisevely generated points 
extern double interTimeTot2; //sum of Tot times between two succisevely generated points over the period for output
extern int numEssMax2,numEssMax22; // Max number of essential coordinates within a period
extern int nodesVisited; //the number of recursion-tree nodes visited since the last output
extern int nodeDepth; //the depth of the recursion-tree node at which the current output is produced
extern int nodesVisitedMax2,nodesVisitedMax22;// the max number of recursion-tree nodes visisted since the last output in a period 
extern int nodeDepthMax2,nodeDepthMax22; // the max depth in the tree at which the output is generated in a period
extern int maxNum; // Maximum number of independent elements to be generated
extern int threshold; // number of independent elements after which tree is renewed
extern double Tthreshold; // intertime in Sec after which tree is renewed
extern int numWithShift; // number of independent elements generated with shifts
extern int numColl;// number of collisions in random shifts
extern int outputPeriod; // when to output statistics
extern int orig; // original number of dual points given to the program
extern char fileName[20];
extern char logFile[20];
extern char debugFile[20];
extern FILE *fout;
extern Boolean toLog;
extern Boolean toOutput;

extern float cleanUpFactor; // the factor in [0,1] of the current number of vectors 
                     // after which cleanup is performed  
extern int numSamples1; // number of samples to be taken for finding a random solution e
extern int numSamples2; // number of samples to be taken for count estimation 
extern int numMultp;

//int min(int x,int y);
//int max(int x,int y);
integer min(integer,integer);
integer max(integer,integer);
double min(double x,double y);
double dabs(double x);
void computeProducts(int num,int* size, long* product);
double map(Vector &v,double *product);
void pam(long int map,Vector *t,int k,long *product);
//int uniform(int n1,int n2);
integer uniform(integer n1,integer n2);
void genRandom0(Box& C, System* A, int num);
void exchange (int *x, int *y);
void init();
void initExp();
void setTimer();
void updateTimes(long num);
void logData(long num,int numEss);
void report1(long num);
void report1_1(long num);
//void report1_2(long num);
void report2(long num);
//void report2_1(long num);
void report3(long num);
void report4(long num);
void report5(int num);
void fprint(char *fileName,char *s);
double chi(double v);
//int __cdecl lexCompare(const void *xx, const void *yy);
//int __cdecl revLexCompare(const void *xx, const void *yy);
int lexCompare(const void *xx, const void *yy);
int revLexCompare(const void *xx, const void *yy);

typedef struct RBNode *RBptr;	
typedef enum { red, black } Color;

typedef struct RBNode {
  Color colour;
  int index;
  RBptr left,right,parent;
} RB_Node;

class RBTree // Red-Black tree
{
  private:
    RBptr root; // pointer to the root
	void deleteTree(RBptr x);
	void left_rotate(RBptr x);
    void right_rotate(RBptr x);
  public:
	RBTree();
	~RBTree();
    void insert(int i,System &B);
	Boolean found(Vector &v,System &B);
	void print(RBptr x);
	void print(FILE *fout,RBptr x);
	void print();
	void print(FILE *fout);
	void print(RBptr x,System &B);
	void print(FILE *fout,RBptr x,System &B);
	void print(System &B);
	void print(FILE *fout,System &B);
	int height(RBptr x);
	int height();
};

class HNode {
  private:
    int index; // the index of the vector in the vector array 
  public:
    HNode& operator=(HNode &x);
  friend class Heap;
};

class Heap
{
  private:
    int num; // num of elements in the heap;
    HNode *ar; // array of heap nodes
	void swap(HNode *x,HNode *y);
    void heapify(IndexArray &a,int l); // restore the heap property
	void heapifyUp(IndexArray &a,int l); // restore the heap property moving Upward
	int numEss(IndexArray &a,int i);
  public:
	Heap(int size=MAXALLOC);
	~Heap();
	void insert(int index);// insert without maintaining the heap property
	void adjustHeap(IndexArray &a); // restore the heap property
	void heapInsert(IndexArray &a,int index); // insert maintaining the heap property
	void getMin(IndexArray &a,int *index,int *numEss);
	void deleteMin(IndexArray &a,int *index,int *numEss);
	Boolean isEmpty(){return num==0;}
	void init(){num=0;}
	void print();
	void print(FILE *fout);
	void print(IndexArray &a);
	void print(FILE *fout,IndexArray &a);
  friend class Problem;
};

#endif