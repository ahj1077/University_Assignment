#include<iostream>
#include<vector>
#include<algorithm>
#include<string>
#include<list>

using namespace std;

int n;	//������ ���� 2 <= n <= 50000 
int m; // �����嵵��(����) ���� 1 <= m <= 500000 
int q; //������ �� 1<= q <= 25000

int total_weight; // mst�� ��ü ����ġ�� ��

class city {
public:
	int num;	// ���� ��ȣ
	string name; // ���� �̸�

	city(int _num, string _name) : num(_num), name(_name) {}
};

class line {
public:
	int c1; //����� ���� ��ȣ 1
	int c2; //����� ���� ��ȣ 2
	int w; // ������ ����ġ

	line(int _c1,int _c2,int _w) : c1(_c1), c2(_c2), w(_w) {}
};

int parent[1000000]; // �� ���� ��ȣ�� index�� �ϸ�, �ش� ������ set ID�� ����
int popul[1000000]; // �� ���� ��ȣ�� index�� �ϸ�, �ش� ������ �α� ���� ����

list<city> lst[1000000]; //�� ���� ��ȣ�� set ID�� �ϴ� ���Ḯ��Ʈ ǥ�� union-find �ڷᱸ��
list<line> lines;	// ��ü �������� ����
vector<line> mst; // mst�� ���� �������� ����

//�Էµ� �������� �����ϱ� ���� ���Լ�
bool cmp(line l1, line l2) {
	//�켱 ������ ����ġ�� �������� ��
	if (l1.w < l2.w) {
		return true;
	}
	else if (l1.w > l2.w) {
		return false;
	}
	else {
		//������ ����ġ�� ���ٸ� ������ ����� ���õ��� �α��� ������ ��
		int c1 = popul[l1.c1] + popul[l1.c2];
		int c2 = popul[l2.c1] + popul[l2.c2];
		if (c1 > c2) {
			return true;
		}
		else if (c1 < c2) {
			return false;
		}
		else {
			//�α��� �ձ��� ���ٸ� ������ ����� ������ ��ȣ�� ���� ���� ���� ������ �켱������ ���� 
			int min_num1 = l1.c1;
			if (l1.c2 < min_num1) min_num1 = l1.c2;

			int min_num2 = l2.c1;
			if (l2.c2 < min_num2) min_num2 = l2.c2;

			return min_num1 < min_num2;
		}
	}
}
//�����͸� �Է¹޴� �Լ�
void getData() {
	cin >> n >> m >> q;

	for (int i = 0; i < n; i++) {
		int city_num, city_popul;
		string city_name;

		cin >> city_num >> city_name >> city_popul;
		popul[city_num] = city_popul;
		parent[city_num] = city_num;
		lst[city_num].push_back(city(city_num,city_name));
	}

	for (int i = 0; i < m; i++) {
		int city_num1, city_num2, gap;
		cin >> city_num1 >> city_num2 >> gap;
		lines.push_back(line(city_num1, city_num2, gap));
	}
}

//�� ���ø� merge�ϴ� �Լ�. �պ��� set�� id�� return ��
int merge(int set_num1, int set_num2) {
	if (lst[set_num1].size() > lst[set_num2].size()) {
		list<city>::iterator iter;

		//set ID�� ����
		for (iter = lst[set_num2].begin(); iter != lst[set_num2].end(); iter++) {
			parent[iter->num] = set_num1;
		}

		//set_num2 �� ���ҵ��� set_num1�� tail�� �߰�
		lst[set_num1].splice(lst[set_num1].end(), lst[set_num2]);
		return set_num1;
	}
	else if (lst[set_num1].size() < lst[set_num2].size()) {
		list<city>::iterator iter;

		//set ID�� ����
		for (iter = lst[set_num1].begin(); iter != lst[set_num1].end(); iter++) {
			parent[iter->num] = set_num2;
		}

		//set_num1 �� ���ҵ��� set_num2�� tail�� �߰�
		lst[set_num2].splice(lst[set_num2].end(), lst[set_num1]);
		return set_num2;
	}
	else {
		// �� ������ ũ�Ⱑ ������ set_id�� ���� ������ merge
		if (set_num1 > set_num2) {
			list<city>::iterator iter;

			//set ID�� ����
			for (iter = lst[set_num1].begin(); iter != lst[set_num1].end(); iter++) {
				parent[iter->num] = set_num2;
			}
			lst[set_num2].splice(lst[set_num2].end(), lst[set_num1]);
			return set_num2;
		}
		else {
			list<city>::iterator iter;

			//set ID�� ����
			for (iter = lst[set_num2].begin(); iter != lst[set_num2].end(); iter++) {
				parent[iter->num] = set_num1;
			}
			lst[set_num1].splice(lst[set_num1].end(), lst[set_num2]);
			return set_num1;
		}
	}
}

//�߰����� ��� : Ư�� ���ð� ���Ե� ���Ḯ��Ʈ�� ���� ���� �ľ�
void queryN() {
	int v, k;
	cin >> v >> k;

	int set_num = parent[v];
	list<city>::iterator iter;

	//���Ḯ��Ʈ���� k��° ������ ������ ã�� ���
	int cnt = 0;
	for (iter = lst[set_num].begin(); iter != lst[set_num].end(); iter++) {
		if (cnt == k) {
			cout << iter->num << " " << iter->name << "\n";
			return;
		}
		cnt++;
	}

	//���ٸ� no exist
	cout << "no exist\n";
}

//�߰����� ���: Ư�� ���ð� ���Ե� ���Ḯ��Ʈ�� ũ��
void queryL() {
	int v;
	cin >> v;
	cout << lst[parent[v]].size() << "\n";
}


//ũ�罺Į �˰��򿡼� �ϳ��� �ݺ��� ����
bool queryI() {
	
	int new_id = 0; //�պ��ǰ� ���� ������� set ID�� ������ ����
	list<line>::iterator iter;

	for (iter = lines.begin(); iter != lines.end(); iter++) {

		//������ ����� �� ������ set ID�� �ٸ��ٸ� mst�� ����
		if (parent[iter->c1] != parent[iter->c2]) {
			mst.push_back(*iter);
			total_weight += iter->w;
			new_id = merge(parent[iter->c1], parent[iter->c2]);
			lines.erase(iter);
			cout << new_id << " " << lst[new_id].size();
			break;
		}
		else {
			//�� ������ set ID�� ���ٸ� �ش� ������ mst�� ������ �����Ƿ� ����
			lines.erase(iter);
			cout << "not union";
			break;
		}
	}

	if (mst.size() == n - 1) {
		//MST�� ã�� ���. mst�� ������ ���� (��� ��) - 1 ��
		cout << " " << new_id << " " << total_weight << "\n";

		//mst�� �����Ǿ��ٸ� true�� �����Ͽ� ���� ���� ���� ���� ���α׷� ����
		return true;
	}


	//mst�� �������� �ʾҴٸ� false�� �����Ͽ� ���� ���Ǹ� ����
	cout << "\n";
	return false;
}

//�߰����� ���: �� ���ð� ���� ���տ� ���ϴ��� Ȯ��
void queryF() {
	int num1, num2;
	cin >> num1 >> num2;

	//�� ������ set ID�� Ȯ��
	if (parent[num1] == parent[num2]) {
		cout << "True " << parent[num1] << "\n";
	}
	else {
		cout << "False " << parent[num1] << " " << parent[num2] <<"\n";
	}
}

//�߰����� ���: ������ ���ø� �����ϴ� ������ ��� ���嵵���� ����
void queryW() {
	int v;
	cin >> v;

	int set_num = parent[v];
	int total = 0;

	//���嵵�δ� mst�� ���ϴ� �������̹Ƿ� mst�� ���ݱ��� ���Ե� �������� Ȯ���Ͽ�
	//������ ����� �� ������ set ID�� ������ ������ set ID�� ������ Ȯ��
	for (int i = 0; i < mst.size(); i++) {
		if (parent[mst[i].c1] == set_num && parent[mst[i].c2] == set_num) {
			total += mst[i].w;
		}
	}

	cout << total << "\n";
}

//�˰����� ������ �ܰ� ���Ӽ��� �� ���α׷� ����
//queryI�� ���� �˰���������, mst�� ������ ������ ������ �ݺ�
void queryQ() {

	int new_id = 0;

	//mst�� �����Ǿ����� �ƴ����� mst ������ ���� n-1���� �ƴ����� �Ǻ�
	while (mst.size() < n - 1) {
		list<line>::iterator iter;

		for (iter = lines.begin(); iter != lines.end(); iter++) {
			if (parent[iter->c1] != parent[iter->c2]) {
				mst.push_back(*iter);
				total_weight += iter->w;
				new_id = merge(parent[iter->c1], parent[iter->c2]);
				lines.erase(iter);
				break;
			}
			else {
				lines.erase(iter);
				break;
			}
		}
	}

	cout << new_id << " " << total_weight << "\n";

}

int main() {
	ios::sync_with_stdio(false);
	cin.tie(NULL);

	//������ �Է¹ޱ�
	getData();

	//�׷����� ��� �������� �������Ͽ� ���õ� �������� ���� ������ �� �ֵ��� ����
	lines.sort(cmp);

	char oper;

	while (q--) {
		cin >> oper;

		if (oper == 'N') {
			queryN();
		}
		else if (oper == 'L') {
			queryL();
		}
		else if (oper == 'I') {
			if (queryI()) break;
		}
		else if (oper == 'F') {
			queryF();
		}
		else if (oper == 'W') {
			queryW();
		}
		else if (oper == 'Q') {
			queryQ();
			break;
		}
	}

	return 0;
}
