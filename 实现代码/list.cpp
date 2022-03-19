#include<stdio.h>
#include<windows.h>
#include<process.h>
#include<stdlib.h>//头文件包含rand和srand函数，这些函数用来生成随机数
#include<time.h>//C/C++中的日期和时间头文件，用于需要时间方面的函数
#include<iostream>
#include <conio.h>


using namespace std;
HANDLE hMutex;
int delete_err_count = 0;//用于统计删除节点失败的个数
int g_count=0;

/*-----------------------------------------*/
/* ------------链表数据结构----------------*/
/*-----------------------------------------*/
//规定创建的链表第一个有值元素下标值为0


//单向链表数据结构
typedef struct MyLinkedList{
	int val;
	struct MyLinkedList* next;
} MyLinkedList;


//创建链表
MyLinkedList* myLinkedListCreate() {
	MyLinkedList* node = (MyLinkedList*)malloc(sizeof(MyLinkedList));
	node->next = NULL;
	return node;
}


//添加头节点
void myLinkedListAddAtHead(MyLinkedList* obj, int val) {
	MyLinkedList* head = (MyLinkedList*)malloc(sizeof(MyLinkedList));
	head->val = val;
	head->next = obj->next;
	obj->next = head;
}


//添加尾节点
void myLinkedListAddAtTail(MyLinkedList* obj, int val) {
	MyLinkedList* tail = (MyLinkedList*)malloc(sizeof(MyLinkedList));
	tail->val = val;
	tail->next = NULL;
	MyLinkedList* node = obj;
	while (node->next != NULL){
		node = node->next;
	}
	node->next = tail;
}


//插入节点
void myLinkedListAddAtIndex(MyLinkedList* obj, int index, int val) {
	if (index<0)return;
	MyLinkedList* node = obj;
	for (int i = 0; i<index; i++){
		if (node->next == NULL)return;
		else node = node->next;
	}
	MyLinkedList* add = (MyLinkedList*)malloc(sizeof(MyLinkedList));
	add->val = val;
	if (node->next != NULL)add->next = node->next;
	else add->next = NULL;
	node->next = add;
}


//删除节点
void myLinkedListDeleteAtIndex(MyLinkedList* obj, int index) {
	if (index<0)return;
	MyLinkedList* node = obj;
	for (int i = 0; i<index; i++){
		if (node->next == NULL)return;
		else node = node->next;
	}
	if (node->next == NULL)return;
	else node->next = node->next->next;
}


//删除尾节点
void myLinkedListDeleteAtTail(MyLinkedList* obj) {
	MyLinkedList* node = obj;
	if (node->next == NULL){ delete_err_count++;return; }//删除失败，记录删除失败的次数
	while (node->next->next != NULL){
		node = node->next;
	}
	node->next = NULL;
	return;
}


//删除链表
void myLinkedListFree(MyLinkedList* obj) {
	MyLinkedList* node = obj;
	MyLinkedList* tmp;
	while (node){
		tmp = node->next;
		free(node);
		node = tmp;
	}
}


//查找节点
int myLinkedListGet(MyLinkedList* obj, int index) {
	if (index<0){
		//printf("请输入正确的索引值！\n");
		return false;
		}
	MyLinkedList* node = obj;
	for (int i = 0; i <= index; i++){
		if (node->next == NULL){
			//printf("查找失败！输入的索引值超过当前链表长度！\n")；
			return false;
			}
		else node = node->next;
	}
	return node->val;
}


//修改节点
void myLinkedListChange(MyLinkedList* obj, int index, int val){
	if (index<0)return;
	MyLinkedList* node = obj->next;
	for (int i = 0; i<index; i++){
		if (node == NULL)return;
		else node = node->next;
	}
	node->val = val;
}


//打印链表
void printlist(MyLinkedList* obj)
{
	printf("开始打印~~\n");
	if (obj->next == NULL)
	{
		printf("hhh\t");
		return;
	}
	MyLinkedList* cur = obj->next;
	while (cur != NULL){
		printf("%d\t", cur->val);
		cur = cur->next;
	}
	printf("\n打印完毕\n");
	return;
}


//统计当前链表节点数量
void countnodes(MyLinkedList* obj)
{
	int count = 0;
	MyLinkedList* cur = obj;
	while (cur->next != NULL){
		count++;
		cur = cur->next;
	}
	g_count=count;
	printf("【--count--】当前节点数量:%d\n", count);
}


   struct Arg
	{
		MyLinkedList* my_list;
		int num;
	};

/*-----------------------------------------*/
/*---------------读者写者模型----------------*/
/*-----------------------------------------*/


int   readcount = 0;//初始化读者数量为0，全局变量使多个读者共享
HANDLE RP_Write;//创建读者句柄
int find_count=0;//用于统计查找成功次数

//读者模型_统计节点数量
void countnodes_s(MyLinkedList* obj) {
	
	WaitForSingleObject(hMutex, -1);//使用互斥量，保证读者数量改变时互斥
	readcount++;//读者数量增加
	if (readcount == 1)
	{
		WaitForSingleObject(RP_Write, INFINITE);//读者正在读，现在写者不能写
	}
	ReleaseMutex(hMutex);//释放互斥信号
	countnodes(obj);//读链表
	WaitForSingleObject(hMutex, -1);//读者读完，读者数量减一，使用互斥量，避免读者数目减一时出错
	readcount--;
	if (readcount == 0)
	{
		ReleaseSemaphore(RP_Write, 1, NULL);//读者数量为零释放信号量，使得写者可以进行写操作
	}
	ReleaseMutex(hMutex);   //释放互斥信号
}

//读者模型_查找节点
void myLinkedListGet_s(MyLinkedList* obj, int index){
	
	WaitForSingleObject(hMutex, -1);
	readcount++;
	if (readcount == 1)
	{
		WaitForSingleObject(RP_Write, INFINITE);
	}
	ReleaseMutex(hMutex);
	if(myLinkedListGet(obj, index)){
	    printf("【--locate--】当前索引值为%d的值为%d\n",index,myLinkedListGet(obj, index));
		find_count++;//统计查找成功次数
	}
	else{
		printf("【--locate--】查找失败~!\n");
	}
	WaitForSingleObject(hMutex, -1);
	readcount--;
	if (readcount == 0)
	{
		ReleaseSemaphore(RP_Write, 1, NULL);
	}
	ReleaseMutex(hMutex);   //释放互斥信号
}

//写者模型_增加尾节点
void myLinkedListAddAtTail_s(MyLinkedList* obj, int val){
	WaitForSingleObject(RP_Write, INFINITE);
	myLinkedListAddAtTail(obj, val);
	ReleaseSemaphore(RP_Write, 1, NULL);
}

//写者模型_删除尾节点
void myLinkedListDeleteAtTail_s(MyLinkedList* obj){
	
	WaitForSingleObject(RP_Write, INFINITE);
	myLinkedListDeleteAtTail(obj);
	printf("【！！！delete】正在删除节点\n");
	ReleaseSemaphore(RP_Write, 1, NULL);
	
}




/*-----------------------------------------*/
/*-----------下面是测试代码段------------------*/
/*-----------------------------------------*/


//多次在头部增加节点
unsigned __stdcall threadTest1(void* my_para)
{
	MyLinkedList* my_list = (MyLinkedList*)my_para;
	srand((unsigned)time(NULL));//选取种子文件
	int k=rand()%100000*100;//控制随机数的范围在10000000中
	printf("要增加的节点数量:%d\n",k);
	printf("正在增加节点处理中，请稍等。。。。\n");
	for (int i = 0; i<k; i++)
	{
		WaitForSingleObject(hMutex, -1);
		myLinkedListAddAtHead(my_list, i);
		ReleaseMutex(hMutex);
	}
	return 0;
}

//多次删除尾节点
unsigned __stdcall threadTest2(void* my_para)
{
	MyLinkedList* my_list = (MyLinkedList*)my_para;
	srand((unsigned)time(NULL));//选取种子文件
	int s=rand()%100000*100;//%100控制随机数的范围
	printf("要删除的节点个数：%d\n",s);
	printf("正在删除节点，请稍等。。。。\n");
	for (int j = 0; j<s; j++)
	{
		WaitForSingleObject(hMutex, -1);
		myLinkedListDeleteAtTail(my_list);//删除尾节点
		ReleaseMutex(hMutex);
	}
	return 0;
}

//增加节点n次
unsigned __stdcall threadTest3(void* my_para)
{
	Arg *a;//为了让读者读的数随机，线程传的参数是一个结构体，即所有要传的参数打包在一个结构体中，实现多个参数的传递
	a=(Arg*)my_para;//将无符号参数强制转换为结构体的格式
	int T=a->num;//要增加节点的个数
	MyLinkedList* my_list = a->my_list;//要修改的链表
	for (int t = 0; t<T; t++)
	{
		if (t % 50 == 0) {
			printf("【start_ADD】第 %d 次准备增加结点了\n", t + 1);
		}
		myLinkedListAddAtTail_s(my_list, t);
		if (t % 50 == 0) {
			printf("【end_ADD】第 %d 次增加结点已完成\n", t + 1);
		}
	}
	return 0;
}

//统计节点n次
unsigned __stdcall threadTest4(void* my_para)
{
	Arg *a;
	a=(Arg*)my_para;
	int T=a->num;
	MyLinkedList* my_list = a->my_list;
	for (int t = 0; t<(T/10); t++)
	{
		printf("【count】申请读节点\n");
		countnodes_s(my_list);
		printf("【count】统计节点结束\n");
	}
	return 0;
}

//统计节点n次
unsigned __stdcall threadTest5(void* my_para)
{
	Arg *a;
	a=(Arg*)my_para;
	int T=a->num;
	MyLinkedList* my_list = a->my_list;
	for (int t = 0; t < (T/10); t++)
	{
		printf("【s_count】申请读节点\n");
		countnodes_s(my_list);
		printf("【e_count】统计节点结束\n");
	}
	return 0;
}

//查找节点n次
unsigned __stdcall threadTest6(void* my_para)
{
	Arg *a;
	a=(Arg*)my_para;
	int T=a->num;
	MyLinkedList* my_list = a->my_list;
	for (int t = 0; t < (T/10); t++)
	{
		if(t%5==0){
		printf("【s_locate】第%d次查找节点开始执行了！\n",t);
		}
		myLinkedListGet_s(my_list, t);
		Sleep(10);
		if(t%5==0){
		printf("【e_locate】第%d次查找执行结束了!\n",t);
		}
	}
	return 0;
}

//删除节点n次
unsigned __stdcall threadTest7(void* my_para)
{
	
	Arg *a;
	a=(Arg*)my_para;
	int T=a->num;
	MyLinkedList* my_list = a->my_list;
	for (int t = 0; t < T; t++)
	{
		printf("【s_delete】第%d次申请删除节点~~~\n",t+1);
		myLinkedListDeleteAtTail_s(my_list);
		printf("【e_delete】第%d次删除节点成功！\n",t+1);
	}
	return 0;
}


/*-----------------------------------------*/
/*------------主函数------------*/
/*-----------------------------------------*/
int main()
{

	char ch;
	while ( true )                           
	{
		printf("***********************************\n");
		printf("     1:验证各个数据结构的正确性\n") ;
		printf("     2:验证并发互斥\n") ;
		printf("     3:验证读者写者模型\n") ;
		printf("***********************************\n");
		printf( "Enter your choice (1 or 2 or 3): ");
		//如果输入信息不正确，继续输入
		do{
			ch = (char)_getch() ;
		} while(ch != '1' && ch != '2' && ch !='3');

	  system("cls") ; //清除控制台显示的信息
	  //选择1
	  if(ch == '1')
	  {
		  printf("------验证各个数据结构的正确性------\n");
	      void TEST1();TEST1();
	  }
	  //选择2
	  if(ch == '2')
	  {
		printf("2:验证并发互斥\n") ; 
		void TEST2();TEST2();
	  }
	  //选择3
	  if(ch == '3')
	  {
		 printf("3:验证生产者消费者模型\n") ;
	     void TEST3(); TEST3();
	  }
	  
	}
	return 0;
}





/*-----------------------------------------*/
/*-----------三种验证模式--------------*/
/*-----------------------------------------*/

//验证读者写者模型
void TEST3()
{
	MyLinkedList* my_list = myLinkedListCreate();
	HANDLE hThread[5];
	hMutex = CreateMutex(NULL, FALSE, NULL);
	RP_Write = CreateSemaphore(NULL, 1, 1, NULL); 
	
	int k[5];//随机数数组
	for(int i=0;i<5;i++)
	{
		srand((unsigned)time(NULL));//选取种子文件
		k[i]=rand()%1000;//控制随机数的范围在1000内
	}
    printf("计划增加节点%d\n",k[0]);
    printf("计划删除节点%d\n",k[4]);

    Arg arg[5];
	for(int j=0;j<5;j++){
		arg[j].my_list=my_list;
		arg[j].num=k[j];
	}

	hThread[0] = (HANDLE)_beginthreadex(NULL, 0, threadTest3, (void*)&arg[0], 0, NULL);//增加节点
	hThread[1] = (HANDLE)_beginthreadex(NULL, 0, threadTest4, (void*)&arg[1], 0, NULL);//查看节点数量
	hThread[2] = (HANDLE)_beginthreadex(NULL, 0, threadTest5, (void*)&arg[2], 0, NULL);//查看节点数量
	hThread[3] = (HANDLE)_beginthreadex(NULL, 0, threadTest6, (void*)&arg[3], 0, NULL);//查找节点
	hThread[4] = (HANDLE)_beginthreadex(NULL, 0, threadTest7, (void*)&arg[4], 0, NULL);//删除节点

	WaitForSingleObject(hThread[0], -1);
	WaitForSingleObject(hThread[1], -1);
	WaitForSingleObject(hThread[2], -1);
	WaitForSingleObject(hThread[3], -1);
	WaitForSingleObject(hThread[4], -1);
	CloseHandle(hThread[0]);
	CloseHandle(hThread[1]);
	CloseHandle(hThread[2]);
	CloseHandle(hThread[3]);
	CloseHandle(hThread[4]);
	CloseHandle(hMutex);
	CloseHandle(RP_Write);
	printf("*****delete_err:%d********\n",delete_err_count);
	printf("*****findsuccess:%d********\n",find_count);
	countnodes(my_list);
	printf("理想节点数量：%d\n",(k[4]-k[0]));
	myLinkedListFree(my_list);
	delete_err_count=0;
}

//验证并发
void TEST2()
{
	MyLinkedList* my_list = myLinkedListCreate();
	HANDLE hThread[2];
	hMutex = CreateMutex(NULL, FALSE, NULL);
	hThread[0]=(HANDLE)_beginthreadex(NULL,0,threadTest1,my_list,0,NULL);
	hThread[1]=(HANDLE)_beginthreadex(NULL,0,threadTest2,my_list,0,NULL);
	WaitForSingleObject(hThread[0],-1);
	WaitForSingleObject(hThread[1],-1);
	CloseHandle(hThread[0]);
	CloseHandle(hThread[1]);
	CloseHandle(hMutex);
	printf("删除失败次数:%d\n",delete_err_count);
	countnodes(my_list);
	myLinkedListFree(my_list);
	 delete_err_count=0;
}

//验证各个数据结构的正确性
void TEST1() {
	MyLinkedList* my_list = myLinkedListCreate();//创建链表
	int n[5]={0};//生成五个随机数
	for(int a =0;a<5;a++){
		n[a]=rand()%100;
		printf("%d\t",n[a]);
	}//生成随机数
		
	//验证头部插入函数的正确性
	printf("在头部插入节点%d次.......\n",n[4]+10);
	for (int i = 0; i <n[4]+10 ; i++)
	{
		myLinkedListAddAtHead(my_list, i+1);
	}
	//打印链表
	printlist(my_list);
	
	//删除索引值为1的节点
	printf("删除索引值为1节点%d次\n",n[4]);
	for (int j = 0; j < n[4]; j++)
	{
		myLinkedListDeleteAtIndex(my_list,1);
	}
	printlist(my_list);
	
	//在位置1插入节点4
	printf("在位置1插入值为%d的节点\n",n[1]);
	myLinkedListAddAtIndex(my_list, 1,n[1]);
	printlist(my_list);
	
	//修改节点
	printf("在位置0修改节点的值为%d\n",n[2]);
	myLinkedListChange(my_list, 0, n[2]);
	printlist(my_list);

	//获取节点
	if(myLinkedListGet(my_list, 2)){
	printf("位置2的节点值为:%d\n", myLinkedListGet(my_list, 2));
	}
	else{
		printf("查找失败！\n");
	}

	//统计节点数
	countnodes(my_list);
	myLinkedListFree(my_list);
	//free(&n);

}


























/*
//在头部增加节点10个  (1)
for(int i=1;i<10;i++)
{
myLinkedListAddAtHead(my_list, i);
}
//打印链表 (2)
printf("start print:\n");
printlist(my_list);
//删除索引值为1的节点； (3)//下表从0开始
for(int j=0;j<3;j++)
{
myLinkedListDeleteAtIndex(my_list, 1);
}
//插入节点 (4)//下标从0开始，插入节点应在所营职之前
myLinkedListAddAtIndex(my_list,1, 4);

//修改节点//下标从0开始
myLinkedListChange(my_list, 0,0);

//获取节点 (5)//下标从0开始
printf("获取节点信息\n");
printf("%d\n",myLinkedListGet(my_list, 2));


//打印链表
printf("start print:\n");
printlist(my_list);
//统计节点数
countnodes(my_list);
*/



//读者写者模型
//增加删除插入为写者。查找打印为读者模型
//增加void myLinkedListAddAtTail(MyLinkedList* obj, int val)
//删除void myLinkedListDeleteAtTail(MyLinkedList* obj) 
//插入void myLinkedListAddAtIndex(MyLinkedList* obj, int index, int val) 
//查找int myLinkedListGet(MyLinkedList* obj, int index) 
//打印void printlist(MyLinkedList* obj)
//读者读时，写者不能改；
//多个读者可以同时读
//写者改，读者不能读
/*
HANDLE hThread[5];//创建5个线程
unsigned threadID[5];
HANDLE rmutex=1,wmutex=1,
int readcount=0;

读者写者模型
void readr(){
do{
WaitForSingleObject(rmutex,infinite);
if(readcount==0)WaitForSingleObject(wmutex,infinite);
readcount++;
ReleaseMutex(rmutex,infinite);
....
perform read operation;
...

printlist(MyLinkedList* obj);//打印
WaitForSingleObject(rmutex,infinite);
readcount--;
if(readcount == 0)ReleaseMutex(wmutex,infinite);
ReleaseMutex(rmutex,infinite);
}while(true);
}

void writer(){
do{
WaitForSingleObject(wmutex,infinite);
----perform write operation-----
ReleaseMutex(wmutex,infinite);
}while(true);

}

*/
