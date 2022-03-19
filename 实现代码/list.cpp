#include<stdio.h>
#include<windows.h>
#include<process.h>
#include<stdlib.h>//ͷ�ļ�����rand��srand��������Щ�����������������
#include<time.h>//C/C++�е����ں�ʱ��ͷ�ļ���������Ҫʱ�䷽��ĺ���
#include<iostream>
#include <conio.h>


using namespace std;
HANDLE hMutex;
int delete_err_count = 0;//����ͳ��ɾ���ڵ�ʧ�ܵĸ���
int g_count=0;

/*-----------------------------------------*/
/* ------------�������ݽṹ----------------*/
/*-----------------------------------------*/
//�涨�����������һ����ֵԪ���±�ֵΪ0


//�����������ݽṹ
typedef struct MyLinkedList{
	int val;
	struct MyLinkedList* next;
} MyLinkedList;


//��������
MyLinkedList* myLinkedListCreate() {
	MyLinkedList* node = (MyLinkedList*)malloc(sizeof(MyLinkedList));
	node->next = NULL;
	return node;
}


//���ͷ�ڵ�
void myLinkedListAddAtHead(MyLinkedList* obj, int val) {
	MyLinkedList* head = (MyLinkedList*)malloc(sizeof(MyLinkedList));
	head->val = val;
	head->next = obj->next;
	obj->next = head;
}


//���β�ڵ�
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


//����ڵ�
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


//ɾ���ڵ�
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


//ɾ��β�ڵ�
void myLinkedListDeleteAtTail(MyLinkedList* obj) {
	MyLinkedList* node = obj;
	if (node->next == NULL){ delete_err_count++;return; }//ɾ��ʧ�ܣ���¼ɾ��ʧ�ܵĴ���
	while (node->next->next != NULL){
		node = node->next;
	}
	node->next = NULL;
	return;
}


//ɾ������
void myLinkedListFree(MyLinkedList* obj) {
	MyLinkedList* node = obj;
	MyLinkedList* tmp;
	while (node){
		tmp = node->next;
		free(node);
		node = tmp;
	}
}


//���ҽڵ�
int myLinkedListGet(MyLinkedList* obj, int index) {
	if (index<0){
		//printf("��������ȷ������ֵ��\n");
		return false;
		}
	MyLinkedList* node = obj;
	for (int i = 0; i <= index; i++){
		if (node->next == NULL){
			//printf("����ʧ�ܣ����������ֵ������ǰ�����ȣ�\n")��
			return false;
			}
		else node = node->next;
	}
	return node->val;
}


//�޸Ľڵ�
void myLinkedListChange(MyLinkedList* obj, int index, int val){
	if (index<0)return;
	MyLinkedList* node = obj->next;
	for (int i = 0; i<index; i++){
		if (node == NULL)return;
		else node = node->next;
	}
	node->val = val;
}


//��ӡ����
void printlist(MyLinkedList* obj)
{
	printf("��ʼ��ӡ~~\n");
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
	printf("\n��ӡ���\n");
	return;
}


//ͳ�Ƶ�ǰ����ڵ�����
void countnodes(MyLinkedList* obj)
{
	int count = 0;
	MyLinkedList* cur = obj;
	while (cur->next != NULL){
		count++;
		cur = cur->next;
	}
	g_count=count;
	printf("��--count--����ǰ�ڵ�����:%d\n", count);
}


   struct Arg
	{
		MyLinkedList* my_list;
		int num;
	};

/*-----------------------------------------*/
/*---------------����д��ģ��----------------*/
/*-----------------------------------------*/


int   readcount = 0;//��ʼ����������Ϊ0��ȫ�ֱ���ʹ������߹���
HANDLE RP_Write;//�������߾��
int find_count=0;//����ͳ�Ʋ��ҳɹ�����

//����ģ��_ͳ�ƽڵ�����
void countnodes_s(MyLinkedList* obj) {
	
	WaitForSingleObject(hMutex, -1);//ʹ�û���������֤���������ı�ʱ����
	readcount++;//������������
	if (readcount == 1)
	{
		WaitForSingleObject(RP_Write, INFINITE);//�������ڶ�������д�߲���д
	}
	ReleaseMutex(hMutex);//�ͷŻ����ź�
	countnodes(obj);//������
	WaitForSingleObject(hMutex, -1);//���߶��꣬����������һ��ʹ�û����������������Ŀ��һʱ����
	readcount--;
	if (readcount == 0)
	{
		ReleaseSemaphore(RP_Write, 1, NULL);//��������Ϊ���ͷ��ź�����ʹ��д�߿��Խ���д����
	}
	ReleaseMutex(hMutex);   //�ͷŻ����ź�
}

//����ģ��_���ҽڵ�
void myLinkedListGet_s(MyLinkedList* obj, int index){
	
	WaitForSingleObject(hMutex, -1);
	readcount++;
	if (readcount == 1)
	{
		WaitForSingleObject(RP_Write, INFINITE);
	}
	ReleaseMutex(hMutex);
	if(myLinkedListGet(obj, index)){
	    printf("��--locate--����ǰ����ֵΪ%d��ֵΪ%d\n",index,myLinkedListGet(obj, index));
		find_count++;//ͳ�Ʋ��ҳɹ�����
	}
	else{
		printf("��--locate--������ʧ��~!\n");
	}
	WaitForSingleObject(hMutex, -1);
	readcount--;
	if (readcount == 0)
	{
		ReleaseSemaphore(RP_Write, 1, NULL);
	}
	ReleaseMutex(hMutex);   //�ͷŻ����ź�
}

//д��ģ��_����β�ڵ�
void myLinkedListAddAtTail_s(MyLinkedList* obj, int val){
	WaitForSingleObject(RP_Write, INFINITE);
	myLinkedListAddAtTail(obj, val);
	ReleaseSemaphore(RP_Write, 1, NULL);
}

//д��ģ��_ɾ��β�ڵ�
void myLinkedListDeleteAtTail_s(MyLinkedList* obj){
	
	WaitForSingleObject(RP_Write, INFINITE);
	myLinkedListDeleteAtTail(obj);
	printf("��������delete������ɾ���ڵ�\n");
	ReleaseSemaphore(RP_Write, 1, NULL);
	
}




/*-----------------------------------------*/
/*-----------�����ǲ��Դ����------------------*/
/*-----------------------------------------*/


//�����ͷ�����ӽڵ�
unsigned __stdcall threadTest1(void* my_para)
{
	MyLinkedList* my_list = (MyLinkedList*)my_para;
	srand((unsigned)time(NULL));//ѡȡ�����ļ�
	int k=rand()%100000*100;//����������ķ�Χ��10000000��
	printf("Ҫ���ӵĽڵ�����:%d\n",k);
	printf("�������ӽڵ㴦���У����Եȡ�������\n");
	for (int i = 0; i<k; i++)
	{
		WaitForSingleObject(hMutex, -1);
		myLinkedListAddAtHead(my_list, i);
		ReleaseMutex(hMutex);
	}
	return 0;
}

//���ɾ��β�ڵ�
unsigned __stdcall threadTest2(void* my_para)
{
	MyLinkedList* my_list = (MyLinkedList*)my_para;
	srand((unsigned)time(NULL));//ѡȡ�����ļ�
	int s=rand()%100000*100;//%100����������ķ�Χ
	printf("Ҫɾ���Ľڵ������%d\n",s);
	printf("����ɾ���ڵ㣬���Եȡ�������\n");
	for (int j = 0; j<s; j++)
	{
		WaitForSingleObject(hMutex, -1);
		myLinkedListDeleteAtTail(my_list);//ɾ��β�ڵ�
		ReleaseMutex(hMutex);
	}
	return 0;
}

//���ӽڵ�n��
unsigned __stdcall threadTest3(void* my_para)
{
	Arg *a;//Ϊ���ö��߶�����������̴߳��Ĳ�����һ���ṹ�壬������Ҫ���Ĳ��������һ���ṹ���У�ʵ�ֶ�������Ĵ���
	a=(Arg*)my_para;//���޷��Ų���ǿ��ת��Ϊ�ṹ��ĸ�ʽ
	int T=a->num;//Ҫ���ӽڵ�ĸ���
	MyLinkedList* my_list = a->my_list;//Ҫ�޸ĵ�����
	for (int t = 0; t<T; t++)
	{
		if (t % 50 == 0) {
			printf("��start_ADD���� %d ��׼�����ӽ����\n", t + 1);
		}
		myLinkedListAddAtTail_s(my_list, t);
		if (t % 50 == 0) {
			printf("��end_ADD���� %d �����ӽ�������\n", t + 1);
		}
	}
	return 0;
}

//ͳ�ƽڵ�n��
unsigned __stdcall threadTest4(void* my_para)
{
	Arg *a;
	a=(Arg*)my_para;
	int T=a->num;
	MyLinkedList* my_list = a->my_list;
	for (int t = 0; t<(T/10); t++)
	{
		printf("��count��������ڵ�\n");
		countnodes_s(my_list);
		printf("��count��ͳ�ƽڵ����\n");
	}
	return 0;
}

//ͳ�ƽڵ�n��
unsigned __stdcall threadTest5(void* my_para)
{
	Arg *a;
	a=(Arg*)my_para;
	int T=a->num;
	MyLinkedList* my_list = a->my_list;
	for (int t = 0; t < (T/10); t++)
	{
		printf("��s_count��������ڵ�\n");
		countnodes_s(my_list);
		printf("��e_count��ͳ�ƽڵ����\n");
	}
	return 0;
}

//���ҽڵ�n��
unsigned __stdcall threadTest6(void* my_para)
{
	Arg *a;
	a=(Arg*)my_para;
	int T=a->num;
	MyLinkedList* my_list = a->my_list;
	for (int t = 0; t < (T/10); t++)
	{
		if(t%5==0){
		printf("��s_locate����%d�β��ҽڵ㿪ʼִ���ˣ�\n",t);
		}
		myLinkedListGet_s(my_list, t);
		Sleep(10);
		if(t%5==0){
		printf("��e_locate����%d�β���ִ�н�����!\n",t);
		}
	}
	return 0;
}

//ɾ���ڵ�n��
unsigned __stdcall threadTest7(void* my_para)
{
	
	Arg *a;
	a=(Arg*)my_para;
	int T=a->num;
	MyLinkedList* my_list = a->my_list;
	for (int t = 0; t < T; t++)
	{
		printf("��s_delete����%d������ɾ���ڵ�~~~\n",t+1);
		myLinkedListDeleteAtTail_s(my_list);
		printf("��e_delete����%d��ɾ���ڵ�ɹ���\n",t+1);
	}
	return 0;
}


/*-----------------------------------------*/
/*------------������------------*/
/*-----------------------------------------*/
int main()
{

	char ch;
	while ( true )                           
	{
		printf("***********************************\n");
		printf("     1:��֤�������ݽṹ����ȷ��\n") ;
		printf("     2:��֤��������\n") ;
		printf("     3:��֤����д��ģ��\n") ;
		printf("***********************************\n");
		printf( "Enter your choice (1 or 2 or 3): ");
		//���������Ϣ����ȷ����������
		do{
			ch = (char)_getch() ;
		} while(ch != '1' && ch != '2' && ch !='3');

	  system("cls") ; //�������̨��ʾ����Ϣ
	  //ѡ��1
	  if(ch == '1')
	  {
		  printf("------��֤�������ݽṹ����ȷ��------\n");
	      void TEST1();TEST1();
	  }
	  //ѡ��2
	  if(ch == '2')
	  {
		printf("2:��֤��������\n") ; 
		void TEST2();TEST2();
	  }
	  //ѡ��3
	  if(ch == '3')
	  {
		 printf("3:��֤������������ģ��\n") ;
	     void TEST3(); TEST3();
	  }
	  
	}
	return 0;
}





/*-----------------------------------------*/
/*-----------������֤ģʽ--------------*/
/*-----------------------------------------*/

//��֤����д��ģ��
void TEST3()
{
	MyLinkedList* my_list = myLinkedListCreate();
	HANDLE hThread[5];
	hMutex = CreateMutex(NULL, FALSE, NULL);
	RP_Write = CreateSemaphore(NULL, 1, 1, NULL); 
	
	int k[5];//���������
	for(int i=0;i<5;i++)
	{
		srand((unsigned)time(NULL));//ѡȡ�����ļ�
		k[i]=rand()%1000;//����������ķ�Χ��1000��
	}
    printf("�ƻ����ӽڵ�%d\n",k[0]);
    printf("�ƻ�ɾ���ڵ�%d\n",k[4]);

    Arg arg[5];
	for(int j=0;j<5;j++){
		arg[j].my_list=my_list;
		arg[j].num=k[j];
	}

	hThread[0] = (HANDLE)_beginthreadex(NULL, 0, threadTest3, (void*)&arg[0], 0, NULL);//���ӽڵ�
	hThread[1] = (HANDLE)_beginthreadex(NULL, 0, threadTest4, (void*)&arg[1], 0, NULL);//�鿴�ڵ�����
	hThread[2] = (HANDLE)_beginthreadex(NULL, 0, threadTest5, (void*)&arg[2], 0, NULL);//�鿴�ڵ�����
	hThread[3] = (HANDLE)_beginthreadex(NULL, 0, threadTest6, (void*)&arg[3], 0, NULL);//���ҽڵ�
	hThread[4] = (HANDLE)_beginthreadex(NULL, 0, threadTest7, (void*)&arg[4], 0, NULL);//ɾ���ڵ�

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
	printf("����ڵ�������%d\n",(k[4]-k[0]));
	myLinkedListFree(my_list);
	delete_err_count=0;
}

//��֤����
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
	printf("ɾ��ʧ�ܴ���:%d\n",delete_err_count);
	countnodes(my_list);
	myLinkedListFree(my_list);
	 delete_err_count=0;
}

//��֤�������ݽṹ����ȷ��
void TEST1() {
	MyLinkedList* my_list = myLinkedListCreate();//��������
	int n[5]={0};//������������
	for(int a =0;a<5;a++){
		n[a]=rand()%100;
		printf("%d\t",n[a]);
	}//���������
		
	//��֤ͷ�����뺯������ȷ��
	printf("��ͷ������ڵ�%d��.......\n",n[4]+10);
	for (int i = 0; i <n[4]+10 ; i++)
	{
		myLinkedListAddAtHead(my_list, i+1);
	}
	//��ӡ����
	printlist(my_list);
	
	//ɾ������ֵΪ1�Ľڵ�
	printf("ɾ������ֵΪ1�ڵ�%d��\n",n[4]);
	for (int j = 0; j < n[4]; j++)
	{
		myLinkedListDeleteAtIndex(my_list,1);
	}
	printlist(my_list);
	
	//��λ��1����ڵ�4
	printf("��λ��1����ֵΪ%d�Ľڵ�\n",n[1]);
	myLinkedListAddAtIndex(my_list, 1,n[1]);
	printlist(my_list);
	
	//�޸Ľڵ�
	printf("��λ��0�޸Ľڵ��ֵΪ%d\n",n[2]);
	myLinkedListChange(my_list, 0, n[2]);
	printlist(my_list);

	//��ȡ�ڵ�
	if(myLinkedListGet(my_list, 2)){
	printf("λ��2�Ľڵ�ֵΪ:%d\n", myLinkedListGet(my_list, 2));
	}
	else{
		printf("����ʧ�ܣ�\n");
	}

	//ͳ�ƽڵ���
	countnodes(my_list);
	myLinkedListFree(my_list);
	//free(&n);

}


























/*
//��ͷ�����ӽڵ�10��  (1)
for(int i=1;i<10;i++)
{
myLinkedListAddAtHead(my_list, i);
}
//��ӡ���� (2)
printf("start print:\n");
printlist(my_list);
//ɾ������ֵΪ1�Ľڵ㣻 (3)//�±��0��ʼ
for(int j=0;j<3;j++)
{
myLinkedListDeleteAtIndex(my_list, 1);
}
//����ڵ� (4)//�±��0��ʼ������ڵ�Ӧ����Ӫְ֮ǰ
myLinkedListAddAtIndex(my_list,1, 4);

//�޸Ľڵ�//�±��0��ʼ
myLinkedListChange(my_list, 0,0);

//��ȡ�ڵ� (5)//�±��0��ʼ
printf("��ȡ�ڵ���Ϣ\n");
printf("%d\n",myLinkedListGet(my_list, 2));


//��ӡ����
printf("start print:\n");
printlist(my_list);
//ͳ�ƽڵ���
countnodes(my_list);
*/



//����д��ģ��
//����ɾ������Ϊд�ߡ����Ҵ�ӡΪ����ģ��
//����void myLinkedListAddAtTail(MyLinkedList* obj, int val)
//ɾ��void myLinkedListDeleteAtTail(MyLinkedList* obj) 
//����void myLinkedListAddAtIndex(MyLinkedList* obj, int index, int val) 
//����int myLinkedListGet(MyLinkedList* obj, int index) 
//��ӡvoid printlist(MyLinkedList* obj)
//���߶�ʱ��д�߲��ܸģ�
//������߿���ͬʱ��
//д�߸ģ����߲��ܶ�
/*
HANDLE hThread[5];//����5���߳�
unsigned threadID[5];
HANDLE rmutex=1,wmutex=1,
int readcount=0;

����д��ģ��
void readr(){
do{
WaitForSingleObject(rmutex,infinite);
if(readcount==0)WaitForSingleObject(wmutex,infinite);
readcount++;
ReleaseMutex(rmutex,infinite);
....
perform read operation;
...

printlist(MyLinkedList* obj);//��ӡ
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
