#include<iostream>
#include<vector>
#include<algorithm>
#include<string>
#include<list>

using namespace std;

int n;	//도시의 개수 2 <= n <= 50000 
int m; // 비포장도로(간선) 개수 1 <= m <= 500000 
int q; //질의의 수 1<= q <= 25000

int total_weight; // mst의 전체 가중치의 합

class city {
public:
	int num;	// 도시 번호
	string name; // 도시 이름

	city(int _num, string _name) : num(_num), name(_name) {}
};

class line {
public:
	int c1; //연결된 도시 번호 1
	int c2; //연결된 도시 번호 2
	int w; // 간선의 가중치

	line(int _c1,int _c2,int _w) : c1(_c1), c2(_c2), w(_w) {}
};

int parent[1000000]; // 각 도시 번호를 index로 하며, 해당 도시의 set ID를 저장
int popul[1000000]; // 각 도시 번호를 index로 하며, 해당 도시의 인구 수를 저장

list<city> lst[1000000]; //각 도시 번호를 set ID로 하는 연결리스트 표현 union-find 자료구조
list<line> lines;	// 전체 간선들을 저장
vector<line> mst; // mst에 들어가는 간선들을 저장

//입력된 간선들을 정렬하기 위한 비교함수
bool cmp(line l1, line l2) {
	//우선 간선의 가중치를 기준으로 비교
	if (l1.w < l2.w) {
		return true;
	}
	else if (l1.w > l2.w) {
		return false;
	}
	else {
		//간선의 가중치가 같다면 간선과 연결된 도시들의 인구수 합으로 비교
		int c1 = popul[l1.c1] + popul[l1.c2];
		int c2 = popul[l2.c1] + popul[l2.c2];
		if (c1 > c2) {
			return true;
		}
		else if (c1 < c2) {
			return false;
		}
		else {
			//인구수 합까지 같다면 간선과 연결된 도시의 번호가 제일 작은 쪽의 간선을 우선적으로 개선 
			int min_num1 = l1.c1;
			if (l1.c2 < min_num1) min_num1 = l1.c2;

			int min_num2 = l2.c1;
			if (l2.c2 < min_num2) min_num2 = l2.c2;

			return min_num1 < min_num2;
		}
	}
}
//데이터를 입력받는 함수
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

//두 도시를 merge하는 함수. 합병된 set의 id를 return 함
int merge(int set_num1, int set_num2) {
	if (lst[set_num1].size() > lst[set_num2].size()) {
		list<city>::iterator iter;

		//set ID를 갱신
		for (iter = lst[set_num2].begin(); iter != lst[set_num2].end(); iter++) {
			parent[iter->num] = set_num1;
		}

		//set_num2 의 원소들을 set_num1의 tail에 추가
		lst[set_num1].splice(lst[set_num1].end(), lst[set_num2]);
		return set_num1;
	}
	else if (lst[set_num1].size() < lst[set_num2].size()) {
		list<city>::iterator iter;

		//set ID를 갱신
		for (iter = lst[set_num1].begin(); iter != lst[set_num1].end(); iter++) {
			parent[iter->num] = set_num2;
		}

		//set_num1 의 원소들을 set_num2의 tail에 추가
		lst[set_num2].splice(lst[set_num2].end(), lst[set_num1]);
		return set_num2;
	}
	else {
		// 두 집합의 크기가 같으면 set_id가 작은 쪽으로 merge
		if (set_num1 > set_num2) {
			list<city>::iterator iter;

			//set ID를 갱신
			for (iter = lst[set_num1].begin(); iter != lst[set_num1].end(); iter++) {
				parent[iter->num] = set_num2;
			}
			lst[set_num2].splice(lst[set_num2].end(), lst[set_num1]);
			return set_num2;
		}
		else {
			list<city>::iterator iter;

			//set ID를 갱신
			for (iter = lst[set_num2].begin(); iter != lst[set_num2].end(); iter++) {
				parent[iter->num] = set_num1;
			}
			lst[set_num1].splice(lst[set_num1].end(), lst[set_num2]);
			return set_num1;
		}
	}
}

//중간정보 출력 : 특정 도시가 포함된 연결리스트의 내부 정보 파악
void queryN() {
	int v, k;
	cin >> v >> k;

	int set_num = parent[v];
	list<city>::iterator iter;

	//연결리스트에서 k번째 도시의 정보를 찾아 출력
	int cnt = 0;
	for (iter = lst[set_num].begin(); iter != lst[set_num].end(); iter++) {
		if (cnt == k) {
			cout << iter->num << " " << iter->name << "\n";
			return;
		}
		cnt++;
	}

	//없다면 no exist
	cout << "no exist\n";
}

//중간정보 출력: 특정 도시가 포함된 연결리스트의 크기
void queryL() {
	int v;
	cin >> v;
	cout << lst[parent[v]].size() << "\n";
}


//크루스칼 알고리즘에서 하나의 반복을 진행
bool queryI() {
	
	int new_id = 0; //합병되고 새로 만들어진 set ID를 저장할 변수
	list<line>::iterator iter;

	for (iter = lines.begin(); iter != lines.end(); iter++) {

		//간선에 연결된 두 도시의 set ID가 다르다면 mst에 넣음
		if (parent[iter->c1] != parent[iter->c2]) {
			mst.push_back(*iter);
			total_weight += iter->w;
			new_id = merge(parent[iter->c1], parent[iter->c2]);
			lines.erase(iter);
			cout << new_id << " " << lst[new_id].size();
			break;
		}
		else {
			//두 도시의 set ID가 같다면 해당 간선은 mst에 속하지 않으므로 제거
			lines.erase(iter);
			cout << "not union";
			break;
		}
	}

	if (mst.size() == n - 1) {
		//MST를 찾은 경우. mst의 간선의 수는 (노드 수) - 1 임
		cout << " " << new_id << " " << total_weight << "\n";

		//mst가 형성되었다면 true를 리턴하여 다음 질의 수행 없이 프로그램 종료
		return true;
	}


	//mst가 형성되지 않았다면 false를 리턴하여 다음 질의를 수행
	cout << "\n";
	return false;
}

//중간정보 출력: 두 도시가 같은 집합에 속하는지 확인
void queryF() {
	int num1, num2;
	cin >> num1 >> num2;

	//두 도시의 set ID를 확인
	if (parent[num1] == parent[num2]) {
		cout << "True " << parent[num1] << "\n";
	}
	else {
		cout << "False " << parent[num1] << " " << parent[num2] <<"\n";
	}
}

//중간정보 출력: 임의의 도시를 포함하는 집합의 모든 포장도로의 길이
void queryW() {
	int v;
	cin >> v;

	int set_num = parent[v];
	int total = 0;

	//포장도로는 mst에 속하는 간선들이므로 mst에 지금까지 포함된 간선들을 확인하여
	//간선과 연결된 두 도시의 set ID가 임의의 도시의 set ID과 같은지 확인
	for (int i = 0; i < mst.size(); i++) {
		if (parent[mst[i].c1] == set_num && parent[mst[i].c2] == set_num) {
			total += mst[i].w;
		}
	}

	cout << total << "\n";
}

//알고리즘의 나머지 단계 연속수행 및 프로그램 종료
//queryI와 같은 알고리즘이지만, mst가 완전히 생성될 때까지 반복
void queryQ() {

	int new_id = 0;

	//mst가 생성되었는지 아닌지는 mst 간선의 수가 n-1인지 아닌지로 판별
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

	//데이터 입력받기
	getData();

	//그래프의 모든 간선들을 과제파일에 제시된 기준으로 먼저 포장할 수 있도록 정렬
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
