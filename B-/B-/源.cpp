#include<iostream>
#include<conio.h>
#define m 3
#define max_queue 40//��������Ϊ20
char file_name[20];//����ַ��ļ��ļ���
using namespace std;

typedef struct node {
	int n = 0; //��ǰ���Ĺؼ�����
	int K[m + 1];
	struct node* parent = NULL;
	struct node* A[m + 1] = { NULL };
}BSub_Node,*BSub_Tree; //B-���Ľṹ��

typedef struct {
	int n=max_queue; 
	int f, r; 
	BSub_Tree *elem;
}Queue;  //���нṹ��
 
void initQueue(Queue &q){ //���г�ʼ��
	q.f = q.r = -1;
	q.elem = new BSub_Tree[q.n];
}

int empty(Queue q) {  //�������Ƿ�Ϊ��
	return q.f == q.r;
}

int full(Queue q) { //�������Ƿ�����
	return (q.r + 1) % q.n == (q.f + q.n) % q.n;
}

void enQueue(Queue& q,BSub_Tree e) { //��Ӳ���
	if (full(q))  return; 
	q.r = (q.r + 1) % q.n;
	q.elem[q.r] = e;
}

void delQueue(Queue& q, BSub_Tree& e) { //���Ӳ���
	if (empty(q)) return;
	q.f = (q.f + 1) % q.n;
	e = q.elem[q.f];
}

int func_search(BSub_Tree R, int key) {  //��һ������в��ҹؼ����±�
	int i;
	for (i = 1; i <= R->n; i++) if (R->K[i] > key) break; //�ҵ��ý���е�һ������key�Ĺؼ����±�
	return i - 1; //��������ǰһ���ؼ����±�
}

void BSub_search(BSub_Tree R, int key, int mode) {  //���ҹؼ����㷨
	BSub_Tree p, parent = NULL;
	p = R;
	bool flag = false;
	int i = 0;
	char path[20]; //�������浱ǰ����·��
	int count_path = 0;
	while (p && !flag) {  //��p����ΪNULL�һ�û�ҵ�key֮ǰѭ��
		i = func_search(p, key);
		path[count_path++] = i + '0';
		if (i > 0 && p->K[i] == key) {
			flag = true;  //�Ѿ��ҵ�key�ؼ���ʱ��Ϊtrue
			break;
		}
		else {
			parent = p; p = p->A[i]; //key��K[i]�󣬱�K[i+1]С����������ǰ����A[i]��ָ���
		}
	}
	//����ҵ��Ľ�����Ϣ
	if (flag) {
		path[count_path - 1] = '\0';
		if (mode == 1) {  //�ǲ��ҹ���ʱ�������������
			cout << "���ҳɹ���" << endl;
			cout << "�ùؼ������ڽ�����Ϣ��" << endl;
		}
		cout << "(R" << path << "," << p->n;
		for (int j = 1; j <= p->n; j++) cout << "," << p->K[j];
		cout << ")" << endl;
		if (mode == -1) { //ͬʱ������ַ��ļ�ĩβ
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
		cout << "����ʧ�ܣ�" << endl;
		cout << "�ùؼ��ֲ������ڴ�B-����..." << endl;
	}
}

BSub_Tree BSub_insert(BSub_Tree &R,int key) { //����ؼ����㷨
	BSub_Tree p, parent = NULL, q;
	p = R;
	int i = 0, j;
	char path[20];
	int count_path = 0;
	while (p) { //�ҵ�Ӧ�ò���Ľ�㣬pΪNULLʱ�˳�ѭ����parent��Ҫ�ҵĽ��
		i = func_search(p, key);
		path[count_path++] = i + '0';
		parent = p; p = p->A[i];
	}
	if (!parent) { //���RΪ�����ͳ�ʼ��
		R = new BSub_Node;
		p = R;
		parent = p;
	}
	else  //���ǿ�����ʱ��
		p = parent;
	for (j = p->n + 1; j >= i + 2; j--)
		p->K[j] = p->K[j - 1];
	p->K[i + 1] = key;
	p->n++;
	int t = 0;//tΪ��ǰ�������ĸ߶ȼ�һ
	while (p->n > m - 1) { //��Ҫ����
		int mid = (p->n % 2 == 0 ? (p->n / 2) : (p->n / 2 + 1)); //��ǰ�����м�һ��Ϊ��
		int temp = p->K[mid]; //��ǰ����K[mid]
		BSub_Tree p1, p2;
		p1 = new BSub_Node;
		p2 = new BSub_Node;
		//���ɷ���ʱ�󲿷�p1
		p1->n = mid - 1;
		for (j = 0; j < mid; j++) p1->A[j] = p->A[j];
		for (j = 0; j < mid; j++) 
			if(p1->A[j]) p1->A[j]->parent = p1;
		for (j = 1; j < mid; j++) p1->K[j] = p->K[j];
		//���ɷ���ʱ�Ҳ���p2
		p2->n = p->n - mid;
		for (j = mid; j <= p->n; j++) p2->A[j - mid] = p->A[j];
		for (j = mid; j <= p->n; j++)
			if (p2->A[j - mid]) p2->A[j - mid]->parent = p2;
		for (j = mid+1; j <= p->n; j++) p2->K[j - mid] = p->K[j];
		//��p1��p2���ӵ������A[i]��A[i+1]��K[mid]�ŵ������K[i+1]λ��
		if (!p->parent) { //���ѵ������Ļ�
			R->n = 1;
			R->K[1] = temp;
			R->A[0] = p1;
			R->A[1] = p2;
			p1->parent = R;
			p2->parent = R;
			break;//������ֱ���˳�����ѭ��
		}
		int parent_num = path[count_path - 2 - t] - '0'; //������i
		//����K[mid]�������K[parent_num]
		for (j = p->parent->n + 1;j >= parent_num + 2; j--)
			p->parent->K[j] = p->parent->K[j - 1];
		p->parent->n++;
		p->parent->K[parent_num + 1] = temp;
		//���������������������
		for (j = p->parent->n + 1; j >= parent_num + 2; j--)
			p->parent->A[j] = p->parent->A[j - 1];
		p->parent->A[parent_num] = p1;
		p->parent->A[parent_num+1] = p2;
		//����˫��ָ��
		p1->parent = p->parent;
		p2->parent = p->parent;

		q = p;
		p = p->parent;//���������ж��Ƿ���Ҫ����
		delete q;
		t++;
	}
	return R;
}

BSub_Tree BSub_delete(BSub_Tree R,int key,bool &flag) {   //ɾ���ؼ����㷨
	BSub_Tree p, q, parent;//q������һ�����������������С�ؼ������ڽ��
	p = R;
	int i = 0, j;
	int parent_num;//��¼˫�׽������һ��ָ��ָ��ɾ�����,A[i]
	while (p) { //�ҵ�Ҫɾ���Ĺؼ������ڵĽ�㣬pΪ���ҽ�㣬iΪ�ùؼ����±�
		i = func_search(p, key);
		if (p->K[i] == key) {
			flag = true;
			break;
		}
		p = p->A[i];
	}
	if (!flag) return R;//û���ҵ����ϵĽ��ĵĻ�flag��Ȼ��false��ɾ��ʧ��
	//ɾ���ؼ���
	if (!p->A[0]) { //�ý������ײ���ն˽��,ֱ��ɾ��
		for (j = i; j <= p->n - 1; j++)
			p->K[j] = p->K[j + 1];
		p->n--;
	}
	else {//��������ײ�ɾ��
		q = p->A[i];
		while (q->A[0]) {
			q = q->A[0];
		}
		p->K[i] = q->K[1];
		for (j = 1; j <= q->n - 1; j++)
			q->K[j] = q->K[j + 1];
		q->n--;
		p = q; //��p����ָ����ײ�ɾ���ؼ��ֵĽ��
	}

	int min = (m % 2 == 0 ? (m / 2) : (m / 2 + 1)); //Ӧ�е����ٹؼ�����+1
	while (p->n == min - 2) { //��Ҫ�ϲ�
		if (!p->parent) {//pΪ�����
			if (p->A[0] && p->n == 0) { //�����Ϊ��ʱ
				R = p->A[0]; //���¸����
				R->parent = NULL;
			}
			else if (p->n == 0) R = NULL; //B-��ֻ��һ�����ѱ�ɾ����Ļ�
			break;//ֻ��һ�㵫��ûɾ�겻������
		}
		parent = p->parent;  //��¼p��˫�׽��
		for (j = 0; j <= p->parent->n; j++)
			if (p->parent->A[j] == p) {
				parent_num = j; break;
			}
		//���a          ���ֵܿ���ɾ��һ���ؼ���
		if (p->parent->A[parent_num - 1] && p->parent->A[parent_num - 1]->n >= min) {
			//�����ֵ����������Ĺؼ������ڽ��
			q = p->parent->A[parent_num - 1];
			//��˫�׽��K[pareent_num]����p�������
			for (j = 2; j <= p->n + 1; j++) {
				p->K[j] = p->K[j - 1];
				p->A[j] = p->A[j - 1];
			}
			p->A[1] = p->A[0];
			p->K[1] = p->parent->K[parent_num];
			p->A[0] = q->A[q->n];
			if (p->A[0])
				p->A[0]->parent = p;//�����ƶ�����������˫�׽��ָ��
			p->n++;
			//�滻��˫�׽���еĹؼ���
			p->parent->K[parent_num] = q->K[q->n];
			//ɾ�����ֵ����������ؼ��ֺ����ұ�ָ��
			q->n--;
		}
		//���b          ���ֵܿ���ɾ��һ���ؼ���
		else if (p->parent->A[parent_num + 1] && p->parent->A[parent_num + 1]->n >= min) {
			//�����ֵ���������С�Ĺؼ������ڽ��
			q = p->parent->A[parent_num + 1];
			//��˫�׽��K[pareent_num+1]����p�����Ҷ�
			p->K[p->n + 1] = p->parent->K[parent_num + 1];
			p->A[p->n + 1] = q->A[0];
			if (p->A[p->n + 1])
				p->A[p->n + 1]->parent = p;//�����ƶ�����������˫�׽��ָ��
			p->n++;
			//�滻��˫�׽���еĹؼ���
			p->parent->K[parent_num + 1] = q->K[1];
			//ɾ�����ֵ���������С�ؼ���
			q->A[0] = q->A[1];
			for (j = 1; j <= q->n - 1; j++) {
				q->K[j] = q->K[j + 1];
				q->A[j] = q->A[j + 1];
			}
			q->n--;
		}
		//���c          ���ֵܸպ���������Ƹ����Ĺؼ���
		else if (parent_num - 1 >= 0 && p->parent->A[parent_num - 1] && p->parent->A[parent_num - 1]->n == min - 1) {
			q = p->parent->A[parent_num - 1]; //qָ��p���ֵ�
			//��˫�׽���йؼ��ֺ�pʣ�µĹؼ��ֲ���q�Ҷ�
			q->K[q->n + 1] = p->parent->K[parent_num];
			for (j = 1; j <= p->n; j++)
				q->K[q->n + 1 + j] = p->K[j];
			for (j = 1; j <= p->n + 1; j++) {
				q->A[q->n + j] = p->A[j - 1];
				if (q->A[q->n + j])
					q->A[q->n + j]->parent = q;//����˫�׽��ָ��
			}
			q->n += (1 + p->n);
			//ɾ��˫�׽���е�K[parent_num]��A[parent_num]
			for (j = parent_num; j <= p->parent->n - 1; j++) {
				p->parent->K[j] = p->parent->K[j + 1];
				p->parent->A[j] = p->parent->A[j + 1];
			}
			p->parent->n--;
			delete p;
			p = parent;//���������ж��Ƿ���Ҫ����
		}
		//���d          ���ֵܸպ���������Ƹ����Ĺؼ���
		else if (parent_num + 1 <= p->parent->n && p->parent->A[parent_num + 1] && p->parent->A[parent_num + 1]->n == min - 1) {
			q = p->parent->A[parent_num + 1]; //qָ��p���ֵ�
			//��˫�׽���йؼ��ֺ�pʣ�µĹؼ��ֲ���q���
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
					q->A[j - 1]->parent = q;//����˫�׽��ָ��
			}
			q->n += (1 + p->n);
			//ɾ��˫�׽���е�K[parent_num]��A[parent_num+1]
			for (j = parent_num; j <= p->parent->n - 1; j++) {
				if (j != p->parent->n - 1)
					p->parent->K[j + 1] = p->parent->K[j + 2];
				p->parent->A[j] = p->parent->A[j + 1];
			}
			p->parent->n--;
			delete p;
			p = parent;//���������ж��Ƿ���Ҫ����
		}
	}
	return R;
}

BSub_Tree insert(BSub_Tree R) { //�����ؼ��ֲ���
	cout << "====================================================" << endl;
	int key;
	cout << "����Ҫ����Ĺؼ��֣�";
	cin >> key;
	R = BSub_insert(R, key);
	//�������Ľ����Ϣ
	cout << "����ɹ���" << endl;
	cout << "�ؼ��������������Ϣ��";
	BSub_search(R, key, 0);
	cout << "====================================================" << endl;
	cout << "��������������˵�...";
	char ch;
	ch = _getch();
	return R;
}

BSub_Tree multi_insert(BSub_Tree R) {  //����ؼ��ֲ���
	int n;
	int* key;
	cout << "====================================================" << endl;
	cout << "����Ҫ����Ĺؼ�����Ŀ��";
	cin >> n;
	key = new int[n];
	cout << "����" << n << "���ؼ��֣�ע�ⲻ�����ظ��Ĺؼ��֣�" << endl;
	for (int i = 0; i < n; i++)
		cin >> key[i];
	for (int i = 0; i < n; i++) {
		R = BSub_insert(R, key[i]);
	}
	cout << n << "���ؼ����Ѳ������..." << endl;
	cout << "====================================================" << endl;
	cout << "��������������˵�...";
	char ch;
	ch = _getch();
	return R;
}

BSub_Tree _delete(BSub_Tree R) {  //ɾ���ؼ���
	cout << "====================================================" << endl;
	int key;
	bool flag = false;
	cout << "����Ҫɾ���Ĺؼ��֣�";
	cin >> key;
	R = BSub_delete(R, key, flag);
	if (flag) cout << "�ؼ��� " << key << " ɾ���ɹ���" << endl;
	else cout << "ɾ��ʧ�ܣ�" << endl;
	cout << "====================================================" << endl;
	cout << "��������������˵�...";
	char ch;
	ch = _getch();
	return R;
}

void search(BSub_Tree R){  //���ҹؼ���
	cout << "====================================================" << endl;
	int key;
	cout << "����Ҫ���ҵĹؼ��֣�";
	cin >> key;
	BSub_search(R, key, 1);
	cout << "====================================================" << endl;
	cout << "��������������˵�...";
	char ch;
	ch = _getch();
}

void layer_print(BSub_Tree R) {  //��α������н��
	BSub_Tree p;
	Queue Q;
	FILE* fp;
	initQueue(Q);
	enQueue(Q, R);
	cout << "====================================================" << endl;
	cout << "��������ַ��ļ����ļ���:" << endl;
	cin >> file_name;
	//����ļ����������
	fp = fopen(file_name, "w+");//������ļ�
	if (!R) cout << "��B-��Ϊ�գ�" << endl;
	else {
		cout << "����α��������B-�����н����Ϣ��" << endl;
		fprintf(fp, "����α��������B-�����н����Ϣ��\n");
		fclose(fp);
		//�ö���ʵ�ֲ�α���
		while (!empty(Q)) {
			delQueue(Q, p); 
			BSub_search(R, p->K[1], -1);//����һ����㲢��������Ϣ������ļ�
			for (int j = 0; j <= p->n; j++)
				if (p->A[j])
					enQueue(Q, p->A[j]); //�ѵ�ǰ������в�Ϊ�յ�����������
		}
	}
	cout << "====================================================" << endl;
	cout << "��������������˵�...";
	char ch;
	ch = _getch();
}

void menu() {
	cout << "=============�˵�����==============" << endl;
	cout << "|1.����ؼ���                     |" << endl;
	cout << "|2.�������ؼ���                 |" << endl;
	cout << "|3.ɾ���ؼ���                     |" << endl;
	cout << "|4.���ҹؼ���                     |" << endl;
	cout << "|5.��α������B-�����н��       |" << endl;
	cout << "|6.��������                       |" << endl;
	cout << "===================================" << endl;
	cout << "�������ѡ��..." << endl;
}

int main() {
	char ch;
	BSub_Tree R = NULL;
	menu();
	ch = _getch();
	while (1) { //ѭ��ѡ�����˵�����
		switch (ch) {
		case '1': {system("cls"); R = insert(R); break; }
		case '2': {system("cls"); R = multi_insert(R); break; }
		case '3': {system("cls"); R = _delete(R); break; }
		case '4': {system("cls"); search(R); break; }
		case '5': {system("cls"); layer_print(R); break; }
		case '6': {cout << "���������" << endl; exit(0); }
		default: ;
		}
		system("cls");
		menu();
		ch = _getch();
	}
	delete R;
	return 0;
}