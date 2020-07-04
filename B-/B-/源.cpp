#include<iostream>
#include<conio.h>
#define m 3
#define max_queue 40//队列容量为20
char file_name[20];//输出字符文件文件名
using namespace std;

typedef struct node {
	int n = 0; //当前结点的关键字数
	int K[m + 1];
	struct node* parent = NULL;
	struct node* A[m + 1] = { NULL };
}BSub_Node,*BSub_Tree; //B-树的结构体

typedef struct {
	int n=max_queue; 
	int f, r; 
	BSub_Tree *elem;
}Queue;  //队列结构体
 
void initQueue(Queue &q){ //队列初始化
	q.f = q.r = -1;
	q.elem = new BSub_Tree[q.n];
}

int empty(Queue q) {  //检测队列是否为空
	return q.f == q.r;
}

int full(Queue q) { //检测队列是否满队
	return (q.r + 1) % q.n == (q.f + q.n) % q.n;
}

void enQueue(Queue& q,BSub_Tree e) { //入队操作
	if (full(q))  return; 
	q.r = (q.r + 1) % q.n;
	q.elem[q.r] = e;
}

void delQueue(Queue& q, BSub_Tree& e) { //出队操作
	if (empty(q)) return;
	q.f = (q.f + 1) % q.n;
	e = q.elem[q.f];
}

int func_search(BSub_Tree R, int key) {  //在一个结点中查找关键字下标
	int i;
	for (i = 1; i <= R->n; i++) if (R->K[i] > key) break; //找到该结点中第一个大于key的关键字下标
	return i - 1; //返回它的前一个关键字下标
}

void BSub_search(BSub_Tree R, int key, int mode) {  //查找关键字算法
	BSub_Tree p, parent = NULL;
	p = R;
	bool flag = false;
	int i = 0;
	char path[20]; //用来保存当前结点的路径
	int count_path = 0;
	while (p && !flag) {  //在p还不为NULL且还没找到key之前循环
		i = func_search(p, key);
		path[count_path++] = i + '0';
		if (i > 0 && p->K[i] == key) {
			flag = true;  //已经找到key关键字时置为true
			break;
		}
		else {
			parent = p; p = p->A[i]; //key比K[i]大，比K[i+1]小，所以移向当前结点的A[i]所指结点
		}
	}
	//输出找到的结点的信息
	if (flag) {
		path[count_path - 1] = '\0';
		if (mode == 1) {  //是查找功能时才输出下面两句
			cout << "查找成功！" << endl;
			cout << "该关键字所在结点的信息：" << endl;
		}
		cout << "(R" << path << "," << p->n;
		for (int j = 1; j <= p->n; j++) cout << "," << p->K[j];
		cout << ")" << endl;
		if (mode == -1) { //同时输出到字符文件末尾
			FILE *fp = fopen(file_name, "a+");
			fprintf(fp, "(R");
			fputs(path, fp);
			fprintf(fp, ",%d", p->n);
			for (int j = 1; j <= p->n; j++) fprintf(fp, ",%d",p->K[j]);
			fprintf(fp, ")\n");
			fclose(fp);
		}
	}
	else {
		path[count_path] = '\0';
		cout << "查找失败！" << endl;
		cout << "该关键字不存在于此B-树中..." << endl;
	}
}

BSub_Tree BSub_insert(BSub_Tree &R,int key) { //插入关键字算法
	BSub_Tree p, parent = NULL, q;
	p = R;
	int i = 0, j;
	char path[20];
	int count_path = 0;
	while (p) { //找到应该插入的结点，p为NULL时退出循环，parent是要找的结点
		i = func_search(p, key);
		path[count_path++] = i + '0';
		parent = p; p = p->A[i];
	}
	if (!parent) { //如果R为空树就初始化
		R = new BSub_Node;
		p = R;
		parent = p;
	}
	else  //不是空树的时候
		p = parent;
	for (j = p->n + 1; j >= i + 2; j--)
		p->K[j] = p->K[j - 1];
	p->K[i + 1] = key;
	p->n++;
	int t = 0;//t为当前结点的树的高度减一
	while (p->n > m - 1) { //需要分裂
		int mid = (p->n % 2 == 0 ? (p->n / 2) : (p->n / 2 + 1)); //当前结点从中间一分为二
		int temp = p->K[mid]; //当前结点的K[mid]
		BSub_Tree p1, p2;
		p1 = new BSub_Node;
		p2 = new BSub_Node;
		//生成分裂时左部分p1
		p1->n = mid - 1;
		for (j = 0; j < mid; j++) p1->A[j] = p->A[j];
		for (j = 0; j < mid; j++) 
			if(p1->A[j]) p1->A[j]->parent = p1;
		for (j = 1; j < mid; j++) p1->K[j] = p->K[j];
		//生成分裂时右部分p2
		p2->n = p->n - mid;
		for (j = mid; j <= p->n; j++) p2->A[j - mid] = p->A[j];
		for (j = mid; j <= p->n; j++)
			if (p2->A[j - mid]) p2->A[j - mid]->parent = p2;
		for (j = mid+1; j <= p->n; j++) p2->K[j - mid] = p->K[j];
		//把p1和p2连接到父结点A[i]和A[i+1]，K[mid]放到父结点K[i+1]位置
		if (!p->parent) { //分裂到根结点的话
			R->n = 1;
			R->K[1] = temp;
			R->A[0] = p1;
			R->A[1] = p2;
			p1->parent = R;
			p2->parent = R;
			break;//处理完直接退出分裂循环
		}
		int parent_num = path[count_path - 2 - t] - '0'; //父结点的i
		//插入K[mid]到父结点K[parent_num]
		for (j = p->parent->n + 1;j >= parent_num + 2; j--)
			p->parent->K[j] = p->parent->K[j - 1];
		p->parent->n++;
		p->parent->K[parent_num + 1] = temp;
		//插入两个新子树到父结点
		for (j = p->parent->n + 1; j >= parent_num + 2; j--)
			p->parent->A[j] = p->parent->A[j - 1];
		p->parent->A[parent_num] = p1;
		p->parent->A[parent_num+1] = p2;
		//更新双亲指针
		p1->parent = p->parent;
		p2->parent = p->parent;

		q = p;
		p = p->parent;//继续向上判断是否需要分裂
		delete q;
		t++;
	}
	return R;
}

BSub_Tree BSub_delete(BSub_Tree R,int key,bool &flag) {   //删除关键字算法
	BSub_Tree p, q, parent;//q用来找一个结点子树中最大或最小关键字所在结点
	p = R;
	int i = 0, j;
	int parent_num;//记录双亲结点是哪一个指针指向删除结点,A[i]
	while (p) { //找到要删除的关键字所在的结点，p为所找结点，i为该关键字下标
		i = func_search(p, key);
		if (p->K[i] == key) {
			flag = true;
			break;
		}
		p = p->A[i];
	}
	if (!flag) return R;//没有找到符合的结点的的话flag依然是false，删除失败
	//删除关键字
	if (!p->A[0]) { //该结点是最底层非终端结点,直接删除
		for (j = i; j <= p->n - 1; j++)
			p->K[j] = p->K[j + 1];
		p->n--;
	}
	else {//否则在最底层删除
		q = p->A[i];
		while (q->A[0]) {
			q = q->A[0];
		}
		p->K[i] = q->K[1];
		for (j = 1; j <= q->n - 1; j++)
			q->K[j] = q->K[j + 1];
		q->n--;
		p = q; //把p重新指向最底层删除关键字的结点
	}

	int min = (m % 2 == 0 ? (m / 2) : (m / 2 + 1)); //应有的最少关键字数+1
	while (p->n == min - 2) { //需要合并
		if (!p->parent) {//p为根结点
			if (p->A[0] && p->n == 0) { //根结点为空时
				R = p->A[0]; //更新根结点
				R->parent = NULL;
			}
			else if (p->n == 0) R = NULL; //B-树只有一层且已被删除完的话
			break;//只有一层但还没删完不做处理
		}
		parent = p->parent;  //记录p的双亲结点
		for (j = 0; j <= p->parent->n; j++)
			if (p->parent->A[j] == p) {
				parent_num = j; break;
			}
		//情况a          左兄弟可以删除一个关键字
		if (p->parent->A[parent_num - 1] && p->parent->A[parent_num - 1]->n >= min) {
			//找左兄弟子树中最大的关键字所在结点
			q = p->parent->A[parent_num - 1];
			//将双亲结点K[pareent_num]插入p的最左端
			for (j = 2; j <= p->n + 1; j++) {
				p->K[j] = p->K[j - 1];
				p->A[j] = p->A[j - 1];
			}
			p->A[1] = p->A[0];
			p->K[1] = p->parent->K[parent_num];
			p->A[0] = q->A[q->n];
			if (p->A[0])
				p->A[0]->parent = p;//更新移动的子树结点的双亲结点指针
			p->n++;
			//替换掉双亲结点中的关键字
			p->parent->K[parent_num] = q->K[q->n];
			//删除左兄弟子树中最大关键字和最右边指针
			q->n--;
		}
		//情况b          右兄弟可以删除一个关键字
		else if (p->parent->A[parent_num + 1] && p->parent->A[parent_num + 1]->n >= min) {
			//找右兄弟子树中最小的关键字所在结点
			q = p->parent->A[parent_num + 1];
			//将双亲结点K[pareent_num+1]插入p的最右端
			p->K[p->n + 1] = p->parent->K[parent_num + 1];
			p->A[p->n + 1] = q->A[0];
			if (p->A[p->n + 1])
				p->A[p->n + 1]->parent = p;//更新移动的子树结点的双亲结点指针
			p->n++;
			//替换掉双亲结点中的关键字
			p->parent->K[parent_num + 1] = q->K[1];
			//删除右兄弟子树中最小关键字
			q->A[0] = q->A[1];
			for (j = 1; j <= q->n - 1; j++) {
				q->K[j] = q->K[j + 1];
				q->A[j] = q->A[j + 1];
			}
			q->n--;
		}
		//情况c          左兄弟刚好有最低限制个数的关键字
		else if (parent_num - 1 >= 0 && p->parent->A[parent_num - 1] && p->parent->A[parent_num - 1]->n == min - 1) {
			q = p->parent->A[parent_num - 1]; //q指向p左兄弟
			//把双亲结点中关键字和p剩下的关键字插入q右端
			q->K[q->n + 1] = p->parent->K[parent_num];
			for (j = 1; j <= p->n; j++)
				q->K[q->n + 1 + j] = p->K[j];
			for (j = 1; j <= p->n + 1; j++) {
				q->A[q->n + j] = p->A[j - 1];
				if (q->A[q->n + j])
					q->A[q->n + j]->parent = q;//更新双亲结点指针
			}
			q->n += (1 + p->n);
			//删除双亲结点中的K[parent_num]和A[parent_num]
			for (j = parent_num; j <= p->parent->n - 1; j++) {
				p->parent->K[j] = p->parent->K[j + 1];
				p->parent->A[j] = p->parent->A[j + 1];
			}
			p->parent->n--;
			delete p;
			p = parent;//继续向上判断是否需要分裂
		}
		//情况d          右兄弟刚好有最低限制个数的关键字
		else if (parent_num + 1 <= p->parent->n && p->parent->A[parent_num + 1] && p->parent->A[parent_num + 1]->n == min - 1) {
			q = p->parent->A[parent_num + 1]; //q指向p右兄弟
			//把双亲结点中关键字和p剩下的关键字插入q左端
			for (j = q->n + 1 + p->n; j >= 1 + p->n + 1; j--) {
				q->K[j] = q->K[j - (1 + p->n)];
				q->A[j] = q->A[j - (1 + p->n)];
			}
			q->A[1 + p->n] = q->A[0];
			for (j = 1; j <= p->n + 1; j++) {
				if (j == p->n + 1)
					q->K[j] = p->parent->K[parent_num + 1];
				else
					q->K[j] = p->K[j];
				q->A[j - 1] = p->A[j - 1];
				if (q->A[j - 1])
					q->A[j - 1]->parent = q;//更新双亲结点指针
			}
			q->n += (1 + p->n);
			//删除双亲结点中的K[parent_num]和A[parent_num+1]
			for (j = parent_num; j <= p->parent->n - 1; j++) {
				if (j != p->parent->n - 1)
					p->parent->K[j + 1] = p->parent->K[j + 2];
				p->parent->A[j] = p->parent->A[j + 1];
			}
			p->parent->n--;
			delete p;
			p = parent;//继续向上判断是否需要分裂
		}
	}
	return R;
}

BSub_Tree insert(BSub_Tree R) { //单个关键字插入
	cout << "====================================================" << endl;
	int key;
	cout << "输入要插入的关键字：";
	cin >> key;
	R = BSub_insert(R, key);
	//输出插入的结点信息
	cout << "插入成功！" << endl;
	cout << "关键字所插入结点的信息：";
	BSub_search(R, key, 0);
	cout << "====================================================" << endl;
	cout << "按任意键返回主菜单...";
	char ch;
	ch = _getch();
	return R;
}

BSub_Tree multi_insert(BSub_Tree R) {  //多个关键字插入
	int n;
	int* key;
	cout << "====================================================" << endl;
	cout << "输入要插入的关键字数目：";
	cin >> n;
	key = new int[n];
	cout << "输入" << n << "个关键字，注意不能有重复的关键字！" << endl;
	for (int i = 0; i < n; i++)
		cin >> key[i];
	for (int i = 0; i < n; i++) {
		R = BSub_insert(R, key[i]);
	}
	cout << n << "个关键字已插入完成..." << endl;
	cout << "====================================================" << endl;
	cout << "按任意键返回主菜单...";
	char ch;
	ch = _getch();
	return R;
}

BSub_Tree _delete(BSub_Tree R) {  //删除关键字
	cout << "====================================================" << endl;
	int key;
	bool flag = false;
	cout << "输入要删除的关键字：";
	cin >> key;
	R = BSub_delete(R, key, flag);
	if (flag) cout << "关键字 " << key << " 删除成功！" << endl;
	else cout << "删除失败！" << endl;
	cout << "====================================================" << endl;
	cout << "按任意键返回主菜单...";
	char ch;
	ch = _getch();
	return R;
}

void search(BSub_Tree R){  //查找关键字
	cout << "====================================================" << endl;
	int key;
	cout << "输入要查找的关键字：";
	cin >> key;
	BSub_search(R, key, 1);
	cout << "====================================================" << endl;
	cout << "按任意键返回主菜单...";
	char ch;
	ch = _getch();
}

void layer_print(BSub_Tree R) {  //层次遍历所有结点
	BSub_Tree p;
	Queue Q;
	FILE* fp;
	initQueue(Q);
	enQueue(Q, R);
	cout << "====================================================" << endl;
	cout << "输入输出字符文件的文件名:" << endl;
	cin >> file_name;
	//清空文件，重新输出
	fp = fopen(file_name, "w+");//打开输出文件
	if (!R) cout << "该B-树为空！" << endl;
	else {
		cout << "按层次遍历输出该B-树所有结点信息：" << endl;
		fprintf(fp, "按层次遍历输出该B-树所有结点信息：\n");
		fclose(fp);
		//用队列实现层次遍历
		while (!empty(Q)) {
			delQueue(Q, p); 
			BSub_search(R, p->K[1], -1);//出队一个结点并输出结点信息到输出文件
			for (int j = 0; j <= p->n; j++)
				if (p->A[j])
					enQueue(Q, p->A[j]); //把当前结点所有不为空的子树结点入队
		}
	}
	cout << "====================================================" << endl;
	cout << "按任意键返回主菜单...";
	char ch;
	ch = _getch();
}

void menu() {
	cout << "=============菜单如下==============" << endl;
	cout << "|1.插入关键字                     |" << endl;
	cout << "|2.插入多个关键字                 |" << endl;
	cout << "|3.删除关键字                     |" << endl;
	cout << "|4.查找关键字                     |" << endl;
	cout << "|5.层次遍历输出B-树所有结点       |" << endl;
	cout << "|6.结束程序                       |" << endl;
	cout << "===================================" << endl;
	cout << "输入你的选择..." << endl;
}

int main() {
	char ch;
	BSub_Tree R = NULL;
	menu();
	ch = _getch();
	while (1) { //循环选择主菜单功能
		switch (ch) {
		case '1': {system("cls"); R = insert(R); break; }
		case '2': {system("cls"); R = multi_insert(R); break; }
		case '3': {system("cls"); R = _delete(R); break; }
		case '4': {system("cls"); search(R); break; }
		case '5': {system("cls"); layer_print(R); break; }
		case '6': {cout << "程序结束！" << endl; exit(0); }
		default: ;
		}
		system("cls");
		menu();
		ch = _getch();
	}
	delete R;
	return 0;
}