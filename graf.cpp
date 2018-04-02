#include <iostream>
#include <cstdio>
#include <cstring>
#include <fstream>

#define MAXN (64)
#define MAXL (256)
#define BIG (1024)
#define min(a,b) (a<b?a:b)
#define max(a,b) (a>b?a:b)

#define WHITE 0
#define GRAY 1
#define BLACK 2

using namespace std;

ifstream fin;

class Graf
{
private:
	int list_ad[MAXN][MAXN]; // ������ ���������
	int matr_ad[MAXN][MAXN]; // ������� ���������
	int matr_ad_2[MAXN][MAXN];
	int matr_in[MAXN][MAXN]; // ������� �������������
	int matr_dist[MAXN][MAXN]; // ������� ���������� ����� ���������
	int matr_weight[MAXN][MAXN]; // ������� �����/��������� ������������
	int deleted[MAXN];
	int deleted_2[MAXN];
	int v; // ����� ������
	int v_2;
	int e; // ����� ����
	int org; // = 1, ���� ���� ���������������
	int rad; // ������ �����
	int diam; // ������� �����
	int del; // ���������� �������� ������
public:
	Graf::Graf()
	{
		memset(list_ad, 0, sizeof(list_ad));
		memset(matr_ad, 0, sizeof(matr_ad));
		memset(matr_in, 0, sizeof(matr_in));
		memset(matr_dist, 0, sizeof(matr_dist));
		memset(deleted, 0, sizeof(deleted));
		e = v = org = del = 0;
		diam = -1;
		rad = BIG;
		return;
	}
	Graf::~Graf()
	{

		return;
	}
	int read_list_ad() // ������ ������ ���������
	{
		char st[MAXL], t[MAXL];
		int i = 1, j = 0, k = 0, m = 0;
		fin.getline(st, MAXL);
		while (st[k++] == '{') {
			++k;
			if (st[k] != '}') {
				while (st[++k] != '}') {
					if (st[k] >= '0' && st[k] <= '9')
						t[m++] = st[k];
					else {
						t[m] = 0;
						list_ad[i][j++] = atoi(t);
						++e;
						m = 0;
					}
				}
				t[m] = 0;
				list_ad[i][j++] = atoi(t);
				++e;
				m = j = 0;
				++i;
				++k;
			}
			else {
				++i;
				++k;
			}
		}
		v = i - 1;
		list_ad_to_matr_ad();
		degrees();
		org_or_neorg();
		if (!org) e /= 2;
		list_ad_to_matr_in();
		make_matr_distance();
		write_list_ad(0);
		return 0;
	}
	int read_matr_ad() // ������ ������� ���������
	{
		char st[MAXN], t[MAXN];
		int i = 1, j = 1, k = 0, m = 0;
		while (!fin.eof()) {
			fin.getline(st, MAXN);
			while (st[k]) {
				while (st[k] && st[k++] != ' ')
					t[m++] = st[k - 1];
				t[m] = 0;
				if (!strcmp(t, "-")) {
					++deleted[i];
					++deleted[j];
					++j;
				}
				else {
					matr_ad[i][j] = atoi(t);
					if (matr_ad[i][j] < 0 || i == j && matr_ad[i][j] % 2) // ������� ������� ����������� ��� �� ��������� ����� �������� �����
						return 1;
					if (i == j)
						e += (matr_ad[i][j++] / 2);
					else
						e += matr_ad[i][j++];
				}
				m = 0;
			}
			++i;
			j = 1;
			k = 0;
		}
		v = i - 1;
		for (i = 1; i <= v; ++i)
			if (deleted[i] == 2 * v)
				deleted[i] = 1;
			else
				deleted[i] = 0;
		org_or_neorg();
		if (!org)
			e /= 2;
		degrees();
		matr_ad_to_list_ad();
		list_ad_to_matr_in();
		make_matr_distance();
		write_matr_ad(0);
		return 0;
	}
	int read_matr_in() // ������ ������� �������������
	{
		char st[MAXN], t[MAXN];
		int i = 1, j = 1, k = 0, m = 0, n = 0;
		while (!fin.eof()) {
			fin.getline(st, MAXN);
			while (st[k]) {
				while (st[k] && st[k++] != ' ')
					t[m++] = st[k - 1];
				t[m] = 0;
				matr_in[i][j] = atoi(t);
				if (matr_in[i][j++] < 0)
					org = 1;
				m = 0;
			}
			++i;
			e = j - 1;
			j = 1;
			k = 0;
		}
		v = i - 1;

		// �������� ������������ �������� ������� �������������
		for (i = 1; i <= e; ++i) {
			k = 0; // ����� 1 � �������
			m = 0; // ����� -1 � �������
			n = 0; // ����� 2 � �������
			for (j = 1; j <= v; ++j) {
				switch (matr_in[j][i]) {
				case 0: break;
				case 1: ++k; break;
				case -1: ++m; break;
				case 2: ++n; break;
				default: return 1;
				}
			}
			if (!((org && (k == 1 && m == 1 && !n || !k && !m && n == 1)) || (!org && (k == 2 && !m && !n || !k && !m && n == 1))))
				return 1;
		}

		matr_in_to_list_ad();
		list_ad_to_matr_ad();
		degrees();
		make_matr_distance();
		write_matr_in(0);
		return 0;
	}
	int read_matr_w() // ������ ������� �����
	{
		char st[MAXN], t[MAXN];
		int i = 1, j = 1, k = 0, m = 0;
		while (!fin.eof()) {
			fin.getline(st, MAXN);
			while (st[k]) {
				while (st[k] && st[k++] != ' ')
					t[m++] = st[k - 1];
				t[m] = 0;
				if (!strcmp(t, "-")) {
					++deleted[i];
					++deleted[j];
					j++;
				}
				else if (!strcmp(t, "b")) {
					matr_weight[i][j] = BIG;
					matr_ad[i][j] = 0;
					++j;
				}
				else {
					matr_weight[i][j] = atoi(t);
					if (matr_weight[i][j])
						matr_ad[i][j] = 1;
					if (i == j && matr_weight[i][j]) // �� ��������� ����� �� ����
						return 1;
					++j;
				}
				m = 0;
			}
			++i;
			j = 1;
			k = 0;
		}
		v = i - 1;
		for (i = 1; i <= v; ++i)
			if (deleted[i] == 2 * v)
				deleted[i] = 1;
			else
				deleted[i] = 0;
		if (!org)
			e /= 2;
		org_or_neorg();
		degrees();
		matr_ad_to_list_ad();
		list_ad_to_matr_in();
		make_matr_distance();
		write_matr_w();
		return 0;
	}
	void org_or_neorg() // �����������, �������� �� ���� ���������������
	{
		int i, j;
		for (i = 1; i <= v; ++i) {
			for (j = 1; j <= v; ++j)
				if (matr_ad[i][j] != matr_ad[j][i])
					org = 1;
			if (org) break;
		}
		return;
	}
	void write_list_ad(int f) // ����� ������ ���������
	{
		int i, j;
		streambuf *bak = NULL;
		ofstream file;

		if (f) {
			file.open("output.txt", ios::app);
			bak = cout.rdbuf();  // ���������
			cout.rdbuf(file.rdbuf()); // ������������� � ����
		}

		cout << "-------------------------------------------------" << endl;
		cout << "������ ���������:" << endl;
		for (i = 1; i <= v; ++i) {
			if (!deleted[i]) {
				cout << "{" << i;
				for (j = 0; j < matr_ad[i][0]; ++j) {
					if (list_ad[i][j])
						cout << " " << list_ad[i][j];
					else
						break;
				}
				cout << "}";
			}
		}
		cout << "\n-------------------------------------------------" << endl;

		if (f)
			cout.rdbuf(bak); // ���������������

		return;
	}
	void write_matr_ad(int f) // ����� ������� ���������
	{
		int i, j;
		streambuf *bak = NULL;
		ofstream file;

		if (f) {
			file.open("output.txt", ios::app);
			bak = cout.rdbuf();  // ���������
			cout.rdbuf(file.rdbuf()); // ������������� � ����
		}

		cout << "-------------------------------------------------" << endl;
		cout << "������� ���������:" << endl;
		for (i = 1; i <= v; ++i) {
			if (deleted[i])
				for (j = 1; j <= v; ++j)
					cout << "- ";
			else
				for (j = 1; j <= v; ++j)
					if (deleted[j])
						cout << "- ";
					else
						cout << matr_ad[i][j] << " ";
			cout << endl;
		}
		cout << "-------------------------------------------------" << endl;

		if (f)
			cout.rdbuf(bak); // ���������������

		return;
	}
	void write_matr_in(int f) // ����� ������� �������������
	{
		int i, j;
		streambuf *bak = NULL;
		ofstream file;

		if (f) {
			file.open("output.txt", ios::app);
			bak = cout.rdbuf();  // ���������
			cout.rdbuf(file.rdbuf()); // ������������� � ����
		}

		cout << "-------------------------------------------------" << endl;
		cout << "������� �������������:" << endl;
		for (i = 1; i <= v; ++i) {
			for (j = 1; j <= e; ++j)
				if (deleted[i])
					cout << "- ";
				else
					cout << matr_in[i][j] << " ";
			cout << endl;
		}
		cout << "-------------------------------------------------" << endl;

		if (f)
			cout.rdbuf(bak); // ���������������

		return;
	}
	void write_matr_w() // ����� ������� �����
	{
		int i, j;
		cout << "-------------------------------------------------" << endl;
		cout << "������� �����:" << endl;
		for (i = 1; i <= v; ++i) {
			if (deleted[i])
				for (j = 1; j <= v; ++j)
					cout << "-	";
			else
				for (j = 1; j <= v; ++j)
					if (deleted[j])
						cout << "-	";
					else
						cout << matr_weight[i][j] << "	";
			cout << endl;
		}
		cout << "-------------------------------------------------" << endl;

		return;
	}
	void write_v() // ����� ����� ������
	{
		cout << "v = " << v - del << endl;
		return;
	}
	void write_e() // ����� ����� ���� �����
	{
		cout << "e = " << e << endl;
		return;
	}
	void list_ad_to_matr_ad() // ������ ��������� � ������� ���������
	{
		int i, j;
		memset(matr_ad, 0, sizeof(matr_ad));
		for (i = 1; i <= v; ++i)
			for (j = 0; j < e; ++j)
				if (list_ad[i][j]) {
					++matr_ad[i][list_ad[i][j]];
					if (i == list_ad[i][j])
						++matr_ad[i][list_ad[i][j]];
				}
				else
					break;
		return;
	}
	void list_ad_to_matr_in() // ������ ��������� � ������� �������������
	{
		int i, j, k = 1;
		memset(matr_in, 0, sizeof(matr_in));
		for (i = 1; i <= v; ++i)
			for (j = 0; j < matr_ad[i][0]; ++j)
				if (i < list_ad[i][j]) {
					if (i != list_ad[i][j]) {
						++matr_in[i][k];
						if (org)
							--matr_in[list_ad[i][j]][k];
						else
							++matr_in[list_ad[i][j]][k];
					}
					else
						matr_in[i][k] = 2;
					++k;
				}
		return;
	}
	void matr_ad_to_list_ad() // ������� ��������� � ������ ���������
	{
		int i, j, k, m, l;
		memset(list_ad, 0, sizeof(list_ad));
		for (i = 1; i <= v; ++i) {
			m = 0;
			for (j = 1; j <= v; ++j) {
				l = matr_ad[i][j];
				if (i == j)
					l /= 2;
				for (k = 0; k < l; ++k)
					list_ad[i][m++] = j;
			}
		}
		return;
	}
	void matr_in_to_list_ad() // ������� ������������� � ������ ���������
	{
		int i, j, from, to;
		memset(list_ad, 0, sizeof(list_ad));
		int h[MAXN] = { 0 };
		if (org)
			for (i = 1; i <= e; ++i) {
				for (j = 1; j <= v; ++j)
					switch (matr_in[j][i]) {
					case 1: from = j; break;
					case -1: to = j; break;
					case 2: from = to = j; break;
					}
				list_ad[from][h[from]++] = to;
			}
		else {
			for (i = 1; i <= e; ++i) {
				from = 0;
				for (j = 1; j <= v; ++j)
					switch (matr_in[j][i]) {
					case 1:
						if (!from) from = j;
						else {
							list_ad[j][h[j]++] = from;
							list_ad[from][h[from]++] = j;
						}
						break;
					case 2: list_ad[j][h[j]++] = j; break;
					}
			}
		}
		return;
	}
	void degrees() // ������� �������� ������ �����
	{
		int i, j;
		for (i = 1; i <= v; ++i) {
			matr_ad[i][0] = 0;
			matr_ad[0][i] = 0;
			for (j = 1; j <= v; ++j) {
				matr_ad[i][0] += matr_ad[i][j];
				matr_ad[0][i] += matr_ad[j][i];
			}
		}
		return;
	}
	void degree_of_vertex() // ����������� �������/������������ ������� �����
	{
		int vn;
		int d[2];
		cout << "�������: ";
		cin >> vn;
		if (vn < 1 || vn > v || deleted[vn]) {
			cout << "�������� �������!" << endl;
			return;
		}
		d[0] = matr_ad[vn][0];
		if (org) d[1] = matr_ad[0][vn];
		else d[1] = -1;
		if (d[1] == -1)
			cout << "������� = " << d[0] << endl;
		else
			cout << "������� ������ = " << d[0] << endl << "����������� ������ = " << d[1] << endl;
		return;
	}
	void write_deg_seq() // ����� ��������� ������������������
	{
		int i;
		if (org) cout << "������������������ ������������ ������:	";
		else cout << "��������� ������������������:	";
		for (i = 1; i <= v; ++i)
			if (!deleted[i])
				cout << matr_ad[i][0] << "   ";
		cout << endl;
		if (org) {
			cout << "������������������ ������������ ������:	";
			for (i = 1; i <= v; ++i)
				if (!deleted[i])
					cout << matr_ad[0][i] << "   ";
			cout << endl;
		}
		return;
	}
	void write_0_and_1_vertexes() // ����� ������������� ������ � ������� (������� � ������)
	{
		int i;
		if (!org) {
			cout << "������������� ������� -	";
			for (i = 1; i <= v; ++i)
				if (matr_ad[i][0] == 0 && !deleted[i]) cout << i << "  ";
			cout << endl;
			cout << "������� ������� -	";
			for (i = 1; i <= v; ++i)
				if (matr_ad[i][0] == 1) cout << i << "  ";
			cout << endl;
		}
		else {
			cout << "������������� ������� -	";
			for (i = 1; i <= v; ++i)
				if (matr_ad[i][0] == 0 && matr_ad[0][i] == 0)
					cout << i << "  ";
			cout << endl;
			cout << "������ -	";
			for (i = 1; i <= v; ++i)
				if (matr_ad[0][i] == 0 && matr_ad[i][0] > 0)
					cout << i << "  ";
			cout << endl;
			cout << "����� -	";
			for (i = 1; i <= v; ++i)
				if (matr_ad[i][0] == 0 && matr_ad[0][i] > 0)
					cout << i << "  ";
			cout << endl;
		}
	}
	void make_matr_distance() // ���������� ������� ���������� ����� ���������
	{
		int i, j, k;
		for (i = 1; i <= v; ++i)
			for (j = 1; j <= v; ++j) {
				if (i == j) matr_dist[i][j] = 0;
				else {
					if (!matr_ad[i][j])
						matr_dist[i][j] = BIG;
					else
						matr_dist[i][j] = 1;
				}
			}
		for (k = 1; k <= v; ++k)
			for (i = 1; i <= v; ++i)
				for (j = 1; j <= v; ++j)
					matr_dist[i][j] = min(matr_dist[i][j], matr_dist[i][k] + matr_dist[k][j]);

		for (i = 1; i <= v; ++i) { // ����������� ���������������� ������
			matr_dist[i][0] = -1;
			for (j = 1; j <= v; ++j)
				if (matr_dist[i][j] != BIG && i != j)
					matr_dist[i][0] = max(matr_dist[i][j], matr_dist[i][0]);
			if (matr_dist[i][0] == -1)
				matr_dist[i][0] = BIG;
		}

		/*cout << "matr_dist:" << endl;
		for (i = 1; i <= v; ++i) {
			for (j = 0; j <= v; ++j)
				cout << matr_dist[i][j] << "	";
			cout << endl;
		}*/

		find_radius_and_diameter();
		return;
	}
	void write_distance_between_vertexes() // ����� ���������� ����� ����� ���������
	{
		int from, to;
		cout << "��������� �������: ";
		cin >> from;
		cout << "�������� �������: ";
		cin >> to;
		if (from < 1 || from > v || to < 1 || to > v || deleted[from] || deleted[to])
			cout << "�������� �������!" << endl;
		else if (matr_dist[from][to] != BIG)
			cout << "���������� = " << matr_dist[from][to] << endl;
		else
			cout << "��� ���� ����� ���������!" << endl;
		return;
	}
	void write_eccentricity_of_vertex() // ����� ��������������� �������
	{
		int vn;
		cout << "�������: ";
		cin >> vn;
		if (vn < 1 || vn > v || deleted[vn])
			cout << "������� �� ����������!" << endl;
		else if (matr_dist[vn][0] == BIG)
			cout << "� ������� ��� ���������������!" << endl;
		else
			cout << "�������������� ������� = " << matr_dist[vn][0] << endl;
		return;
	}
	void find_radius_and_diameter() // ����������� ������� � �������� �����
	{
		int i;
		diam = -1;
		rad = BIG;
		for (i = 1; i <= v; ++i) {
			if (matr_dist[i][0] != BIG && !deleted[i]) {
				rad = min(rad, matr_dist[i][0]);
				diam = max(diam, matr_dist[i][0]);
			}
		}
		return;
	}
	void write_radius_and_diameter() // ����� ������� � �������� �����
	{
		if (rad == BIG)
			cout << "������� ���!" << endl;
		else
			cout << "������ = " << rad << endl;
		if
			(diam == -1) cout << "�������� ���!" << endl;
		else
			cout << "������� = " << diam << endl;
		return;
	}
	void write_central_and_peripheral_vertexes() // ����� ������  � ������������ ������ �����
	{
		int i;
		if (rad == BIG)
			cout << "����������� ������ ���!" << endl;
		else {
			cout << "����������� �������:	";
			for (i = 1; i <= v; ++i)
				if (matr_dist[i][0] == rad)
					cout << i << " ";
			cout << endl;
		}
		if (diam == -1)
			cout << "������������ ������ ���!" << endl;
		else {
			cout << "������������ �������:	";
			for (i = 1; i <= v; ++i)
				if (matr_dist[i][0] == diam)
					cout << i << " ";
			cout << endl;
		}
		return;
	}
	void add_vertex(int from) // ���������� �������
	{
		if (from > v)
			v = from;
		else if (from >= 1 && from <= v) {
			if (deleted[from])
				deleted[from] = 0;
			else
				cout << "������� ��� ����������" << endl;
		}
		else
			cout << "�������� �������" << endl;
		return;
	}
	void delete_vertex(int vn) // �������� �������
	{
		int i;
		if (vn < 1 || vn > v || deleted[vn])
			cout << "������� �� ����������!" << endl;
		else {
			++del;
			deleted[vn] = 1;
			for (i = 1; i <= v; ++i) {
				e -= matr_ad[vn][i];
				if (org)
					e -= matr_ad[i][vn];
				matr_ad[vn][i] = 0;
				matr_ad[i][vn] = 0;
			}
			degrees();
			matr_ad_to_list_ad();
			list_ad_to_matr_in();
			make_matr_distance();
		}
		return;
	}
	void add_edge(int from, int to) // ���������� �����
	{
		if (from < 1 || to < 1)
			cout << "�������� �������!" << endl;
		else {
			if (from > v || to > v)
				do {
					add_vertex(v + 1);
				} while (from > v || to > v);
				++matr_ad[from][to];
				if (!org)
					++matr_ad[to][from];
				++e;
				degrees();
				matr_ad_to_list_ad();
				list_ad_to_matr_in();
				make_matr_distance();
		}
		return;
	}
	void delete_edge(int from, int to) // �������� �����
	{
		if (from < 1 || from > v || to < 1 || to > v)
			cout << "�������� �������!" << endl;
		if (!matr_ad[from][to])
			cout << "����� �� ����������!" << endl;
		else {
			if (from == to)
				matr_ad[from][to] -= 2;
			else {
				--matr_ad[from][to];
				if (!org)
					--matr_ad[to][from];
			}
			--e;
			degrees();
			matr_ad_to_list_ad();
			list_ad_to_matr_in();
			make_matr_distance();
		}
		return;
	}
	void complement_graph() // ����������� ���������� �����
	{
		int i, j;
		for (i = 1; i <= v; ++i) {
			if (!deleted[i]) {
				for (j = 1; j <= v; ++j)
					if (!deleted[j])
						if (i != j) {
							if (!matr_ad[i][j])
								matr_ad[i][j] = 1;
							else {
								e = e - matr_ad[i][j] + 1;
								matr_ad[i][j] = 0;
							}
						}
						else {
							e -= matr_ad[i][j] / 2;
							matr_ad[i][j] = 0;
						}
			}
		}
		e = v * (v - 1) / 2 - e;
		degrees();
		matr_ad_to_list_ad();
		list_ad_to_matr_in();
		make_matr_distance();
		write_matr_ad(0);
		return;
	}
	void subdivision_of_edge(int from, int to) // ������������ �����
	{
		if (from < 1 || from > v || to < 1 || to > v || deleted[from] || deleted[to])
			cout << "�������� �������!" << endl;
		if (!matr_ad[from][to])
			cout << "����� �� ����������!" << endl;
		else {
			delete_edge(from, to);
			add_vertex(v + 1);
			add_edge(from, v);
			add_edge(v, to);
		}
		degrees();
		matr_ad_to_list_ad();
		list_ad_to_matr_in();
		make_matr_distance();
		return;
	}
	void contraction_of_edge(int from, int to) // ���������� �����
	{
		if (from < 1 || from > v || to < 1 || to > v || deleted[from] || deleted[to])
			cout << "�������� �������!" << endl;
		if (!matr_ad[from][to])
			cout << "����� �� ����������!" << endl;
		else {
			delete_edge(from, to);
			identification_of_vertexes(from, to);
		}
	}
	void identification_of_vertexes(int from, int to) // �������������� ������
	{
		int i;
		if (from < 1 || from > v || to < 1 || to > v || deleted[from] || deleted[to])
			cout << "�������� �������!" << endl;
		else {
			for (i = 1; i <= v; ++i)
				if (!deleted[i]) {
					matr_ad[i][from] |= matr_ad[i][to];
					matr_ad[from][i] |= matr_ad[to][i];
				}
			delete_vertex(to);
		}
		degrees();
		matr_ad_to_list_ad();
		list_ad_to_matr_in();
		make_matr_distance();
	}
	void duplication_of_vertex(int vn) // ������������ �������
	{
		int i;
		if (vn < 1 || vn > v || deleted[vn])
			cout << "������� �� ����������!" << endl;
		else {
			add_vertex(v + 1);
			for (i = 1; i <= v; ++i) {
				matr_ad[i][v] = matr_ad[i][vn];
				matr_ad[v][i] = matr_ad[vn][i];
			}
			degrees();
			matr_ad_to_list_ad();
			list_ad_to_matr_in();
			make_matr_distance();
		}
	}
	void multiplication_of_vertex(int vn) // ����������� �������
	{
		if (vn < 1 || vn > v || deleted[vn])
			cout << "������� �� ����������!" << endl;
		else {
			duplication_of_vertex(vn);
			add_edge(vn, v);
		}
	}
	int load_additional_graf()
	{
		ifstream f;
		int view = 4;
		char filename[128];
		char st[MAXN], t[MAXN];
		int i = 1, j = 1, k = 0, m = 0;
		memset((void*)filename, 0, sizeof(filename));
		cout << "������� ��� �����, ����������� ������� ��������� �����: ";
		cin >> filename;
		f.open(filename);
		if (!f.is_open()) { // ���� ���� �� ������
			cout << "���� �� ������!" << endl;
			return 0;
		}
		memset(matr_ad_2, 0, sizeof(matr_ad_2));
		memset(deleted_2, 0, sizeof(deleted_2));
		while (!f.eof()) {
			f.getline(st, MAXN);
			while (st[k]) {
				while (st[k] && st[k++] != ' ')
					t[m++] = st[k - 1];
				t[m] = 0;
				if (!strcmp(t, "-")) {
					++deleted_2[i];
					++deleted_2[j];
					matr_ad_2[i][j++] = 0;
				}
				else {
					matr_ad_2[i][j] = atoi(t);
					if (matr_ad_2[i][j] < 0 || i == j && matr_ad_2[i][j] % 2) { // ������� ������� ����������� ��� �� ��������� ����� �������� �����
						cout << "������� �����������!" << endl;
						return 0;
					}
					++j;
				}
				m = 0;
			}
			++i;
			j = 1;
			k = 0;
		}
		v_2 = i - 1;
		for (i = 1; i <= v_2; ++i)
			if (deleted_2[i] == 2 * v_2)
				deleted_2[i] = 1;
			else
				deleted_2[i] = 0;
		cout << "-------------------------------------------------" << endl;
		cout << "������� ���������:" << endl;
		for (i = 1; i <= v_2; ++i) {
			if (deleted[i])
				for (j = 1; j <= v_2; ++j)
					cout << "- ";
			else {
				for (j = 1; j <= v_2; ++j)
					if (deleted[j])
						cout << "- ";
					else
						cout << matr_ad_2[i][j] << " ";
			}
			cout << endl;
		}
		cout << "-------------------------------------------------" << endl;
		f.close();
		return 1;
	}
	void union_of_grafs() // ����������� ������
	{
		int i, j;
		if (load_additional_graf()) {
			v = (v > v_2) ? v : v_2;
			for (i = 1; i <= v; ++i) {
				deleted[i] |= deleted_2[i];
				if (!deleted[i])
					for (j = 1; j <= v; ++j)
						matr_ad[i][j] |= matr_ad_2[i][j];
			}
			write_matr_ad(0);
			degrees();
			matr_ad_to_list_ad();
			list_ad_to_matr_in();
			make_matr_distance();
		}
		return;
	}
	void connection_of_grafs() // ���������� ������
	{
		int i, j;
		if (load_additional_graf()) {
			for (i = v + 1; i <= v + v_2; ++i)
				if (!deleted_2[i - v]) {
					for (j = v + 1; j <= v + v_2; ++j)
						matr_ad[i][j] = matr_ad_2[i - v][j - v];
					for (j = 1; j <= v; ++j)
						matr_ad[i][j] = matr_ad[j][i] = 1;
				}
				else
					deleted[i] = 1;
			v += v_2;
			write_matr_ad(0);
			degrees();
			matr_ad_to_list_ad();
			list_ad_to_matr_in();
			make_matr_distance();
		}
		return;
	}
	void composition_of_grafs() // ������������ ������
	{
		int i, j, k, t;
		if (load_additional_graf()) {
			for (i = 1; i <= v; ++i)
				for (j = 1; j <= v; ++j)
					for (k = 1; k < v_2; ++k) {
						if (deleted[i])
							deleted[i + v * k] = 1;
						matr_ad[i + v * k][j + v * k] = matr_ad[i][j];
					}
			for (i = 0; i < v_2; ++i)
				for (j = 0; j < v_2; ++j)
					if (deleted_2[i + 1])
						for (k = 1; k <= v; ++k)
							deleted[k + v * i] = 1;
					else if (t = matr_ad_2[i + 1][j + 1])
						for (k = 1; k <= v; ++k)
							matr_ad[k + v * i][k + v * j] += t;
			v *= v_2;
			write_matr_ad(0);
			degrees();
			matr_ad_to_list_ad();
			list_ad_to_matr_in();
			make_matr_distance();
		}
		return;
	}
	void deep_search(int T[MAXN][MAXN], int num[MAXN], int vn)
	{
		int i;
		num[vn] = 1;
		for (i = 1; i <= v; ++i)
			if (matr_ad[vn][i] && !num[i] && !deleted[i]) {
				T[vn][i] = 1;
				if (!org)
					T[i][vn] = 1;
				deep_search(T, num, i);
			}
		return;
	}
	void DFS() // ����� � �������
	{
		int T[MAXN][MAXN] = { 0 };
		int num[MAXN] = { 0 };
		int vn = 1, i, j;
		for (i = 1; i <= v; ++i)
			if (!num[i] && !deleted[i])
				deep_search(T, num, i);
		cout << "-------------------------------------------------" << endl;
		cout << "����� �����, ��������� ������� � �������:" << endl;
		for (i = 1; i <= v; ++i) {
			if (deleted[i])
				for (j = 1; j <= v; ++j)
					cout << "- ";
			else
				for (j = 1; j <= v; ++j)
					if (deleted[j])
						cout << "- ";
					else
						cout << T[i][j] << " ";
			cout << endl;
		}
		cout << "-------------------------------------------------" << endl;
		return;
	}
	void BFS() // ����� � ������
	{
		int T[MAXN][MAXN] = { 0 }; // �����
		int num[MAXN] = { 0 };
		int Q[MAXN] = { 0 };
		int q = 0, l = 0, i, j;
		for (j = 1; j <= v; ++j) {
			if (!deleted[j] && !num[j]) {
				Q[l++] = j;
				num[j] = j * v;
				while (q != l) {
					for (i = 0; i < matr_ad[Q[q]][0]; ++i)
						if (!num[list_ad[Q[q]][i]]) {
							Q[l++] = list_ad[Q[q]][i];
							num[list_ad[Q[q]][i]] = num[Q[q]] + 1;
							T[Q[q]][list_ad[Q[q]][i]] = 1;
							if (!org)
								T[list_ad[Q[q]][i]][Q[q]] = 1;
						}
					++q;
				}
			}
		}
		cout << "-------------------------------------------------" << endl;
		cout << "����� �����, ��������� ������� � ������:" << endl;
		for (i = 1; i <= v; ++i) {
			if (deleted[i])
				for (j = 1; j <= v; ++j)
					cout << "- ";
			else
				for (j = 1; j <= v; ++j)
					if (deleted[j])
						cout << "- ";
					else
						cout << T[i][j] << " ";
			cout << endl;
		}
		cout << "-------------------------------------------------" << endl;
		return;
	}
	void Prim()
	{
		int i, j, n = 0, d, from, to, weight = 0;;
		int T[MAXN][MAXN] = { 0 };
		int flag[MAXN] = { 0 };
		for (i = 1; i <= v; ++i)
			if (!deleted[i])
				break;
		if (i > v)
			return;
		flag[i] = 1;
		n = 1;
		while (n < v) {
			d = BIG;
			for (i = 1; i <= v; ++i)
				if (flag[i])
					for (j = 1; j <= v; ++j)
						if (!flag[j] && !deleted[j] && matr_weight[i][j] && matr_weight[i][j] < d) {
							d = matr_weight[i][j];
							from = i;
							to = j;
						}
			T[from][to] = 1;
			if (!org)
				T[to][from] = 1;
			cout << from << "	" << to << endl;
			weight += d;
			flag[to] = 1;
			++n;
		}
		cout << "-------------------------------------------------" << endl;
		cout << "����� ������������ ����, ��������� ���������� �����:" << endl;
		for (i = 1; i <= v; ++i) {
			if (deleted[i])
				for (j = 1; j <= v; ++j)
					cout << "-	";
			else
				for (j = 1; j <= v; ++j)
					if (deleted[j])
						cout << "-	";
					else
						cout << T[i][j] << "	";
			cout << endl;
		}
		cout << "��� ������:" << weight << endl;
		cout << "-------------------------------------------------" << endl;
	}
	void Bellman_Ford(int from, int to)
	{
		int i, j, k;
		int d[MAXN], p[MAXN];
		if (from < 1 || from > v || deleted[from] || to < 1 || to > v || deleted[to])
			cout << "�������� �������!" << endl;
		else {
			for (i = 1; i <= v; ++i)
				d[i] = BIG;
			d[from] = 0;
			for (i = 1; i < v; ++i)
				for (j = 1; j <= v; ++j)
					if (!deleted[j])
						for (k = 1; k <= v; ++k)
							if (!deleted[k] && matr_ad[j][k])
								if (d[k] > d[j] + matr_weight[j][k]) {
									d[k] = d[j] + matr_weight[j][k];
									p[k] = j;
								}
			if (d[to] == BIG) {
				cout << "���� ����� ��������� ���!" << endl;
				return;
			}
			cout << "����� ����: " << d[to] << endl;
			j = 0;
			d[j] = to;
			while (d[j] != from) {
				d[j + 1] = p[d[j]];
				i = d[++j];
			}
			cout << d[j];
			for (i = j -1; i >= 0; --i)
				cout << "->" << d[i];
			cout << endl;
		}
		return;
	}
	void maxmin(int from, int to)
	{
		int c[MAXN], p[MAXN] = { 0 }, f[MAXN] = { 0 };
		int vn, i, j;
		if (from < 1 || from > v || deleted[from] || to < 1 || to > v || deleted[to])
			cout << "�������� �������!" << endl;
		else {
			for (i = 1; i <= v; ++i)
				c[i] = BIG;
			vn = from;
			while (vn != to) {
				f[vn] = 1;
				for (i = 1; i <= v; ++i)
					if (matr_ad[vn][i] || c[i] < c[vn] || c[i] < matr_weight[vn][i]) {
						if (c[i] == BIG) {
							c[i] = min(c[vn], matr_weight[vn][i]);
							p[i] = vn;
						}
					}
				j = -BIG;
				for (i = 1; i <= v; ++i)
					if (c[i] != BIG && !f[i] && c[i] > j) {
						vn = i;
						j = c[i];
					}
			}
			cout << "��� ����: " << c[to] << endl;
			j = 0;
			c[j] = to;
			while (c[j] != from) {
				c[j + 1] = p[c[j]];
				i = c[++j];
			}
			cout << c[j];
			for (i = j - 1; i >= 0; --i)
				cout << "->" << c[i];
			cout << endl;
		}
	}
	int bfs(int start, int end, int flow[MAXN][MAXN], int* color, int* pred, int* head, int* tail, int* q)
	{
		int u, w;
		for (u = 1; u <= v; ++u) // ������� �������� ��� ������� �� �����������
			color[u] = WHITE;
		*head = 0; // ������ ������� 0
		*tail = 0; // ����� 0
		q[*tail] = start;
		++(*tail); // ������� ���������� ��������� �������
		color[start] = GRAY;
		pred[start] = -1;
		while (*head != *tail) { // ���� ����� �� ������� � �������
			u = q[*head]; // �������� � u �������� ������
			++(*head);
			color[u] = BLACK; // ������� u ���������� ��� �����������
			for (w = 1; w <= v; ++w) { // ������� ������� �������
				if (color[w] == WHITE && matr_weight[u][w] - flow[u][w] > 0) { // ���� �� �������� � �� ���������
					q[*tail] = w; // �������� � �����
					++(*tail); // ������� ���������� ��������� �������
					color[w] = GRAY; // ���� �����
					pred[w] = u; // ���� ���������
				}
			}
		}
		if (color[end] == BLACK)
			return 0;
		else
			return 1;
	}
	int max_flow(int source, int stock, int flow[MAXN][MAXN], int* color, int* pred, int* head, int* tail, int* q)
	{
		int i, j, u;
		int maxflow = 0;
		int delta;
		memset(flow, 0, MAXN * MAXN);
		while (!bfs(source, stock, flow, color, pred, head, tail, q)) {
			delta = BIG;
			for (u = stock; pred[u] > 0; u = pred[u]) // ����� ����������� ����� � ����
				delta = min(delta, matr_weight[pred[u]][u] - flow[pred[u]][u]);
			for (u = stock; pred[u] > 0; u = pred[u]) { // �� ��������� �����-����������
				flow[pred[u]][u] += delta;
				flow[u][pred[u]] -= delta;
			}
			maxflow += delta; // �������� ������������ �����
		}
		return maxflow;
	}
	void Ford_Falkerson()
	{
		int i, j;
		int flow[MAXN][MAXN]; // ������� ������� ����� ����
		int color[MAXN]; // ����� ��� ������
		int pred[MAXN]; // ������ ����
		int head, tail;
		int q[MAXN]; // �������
		for (i = 1; i <= v; ++i) // �����
			if (matr_ad[0][i] == 0 && matr_ad[i][0] > 0)
				break;
		for (j = 1; j <= v; ++j) // ����
			if (matr_ad[j][0] == 0 && matr_ad[0][j] > 0)
				break;
		cout << "������������ ����� �� " << i << " � " << j << ": " << max_flow(i, j, flow, color, pred, &head, &tail, q) << endl;
		cout << "������� ������� ����� ����: " << endl;
		for (i = 1; i <= v; ++i) {
			for (j = 1; j <= v; ++j)
				cout << flow[i][j] << "	";
			cout << endl;
		}
	}
};

void choose(Graf G)
{
	int f, from, to;
	cout << endl << "��������� ��������:" << endl;
	cout << "-------------------------------------------------" << endl;
	cout << "1 - ����������� ����� ������" << endl;
	cout << "2 - ����������� ����� ������" << endl;
	cout << "3 - ����������� ������� �������" << endl;
	cout << "4 - ����������� ��������� ������������������" << endl;
	cout << "-------------------------------------------------" << endl;
	cout << "5 - ����� ������ ���������" << endl;
	cout << "6 - ����� ������� ���������" << endl;
	cout << "7 - ����� ������� �������������" << endl;
	cout << "-------------------------------------------------" << endl;
	cout << "8 - ����������� ������ �� �������� 0 � 1" << endl;
	cout << "9 - ����������� ���������� ����� ����� ���������" << endl;
	cout << "10 - ����������� ��������������� �������" << endl;
	cout << "11 - ����������� ������� � �������� �����" << endl;
	cout << "12 - ����������� ������ � ������������ ������ �����" << endl;
	cout << "-------------------------------------------------" << endl;
	cout << "13 - ���������� �������" << endl;
	cout << "14 - �������� �������" << endl;
	cout << "15 - ���������� �����" << endl;
	cout << "16 - �������� �����" << endl;
	cout << "-------------------------------------------------" << endl;
	cout << "17 - ����������� ���������� �����" << endl;
	cout << "-------------------------------------------------" << endl;
	cout << "18 - ������������ �����" << endl;
	cout << "19 - ���������� �����" << endl;
	cout << "20 - �������������� ������" << endl;
	cout << "21 - ������������ �������" << endl;
	cout << "22 - ����������� �������" << endl;
	cout << "-------------------------------------------------" << endl;
	cout << "23 - ����������� ������" << endl;
	cout << "24 - ���������� ������" << endl;
	cout << "25 - ������������ ������" << endl;
	cout << "-------------------------------------------------" << endl;
	cout << "26 - ����� ������ ��������� � ����" << endl;
	cout << "27 - ����� ������� ��������� � ����" << endl;
	cout << "28 - ����� ������� ������������� � ����" << endl;
	cout << "-------------------------------------------------" << endl;
	cout << "29 - ���������� ������ ������� � �������" << endl;
	cout << "30 - ���������� ������ ������� � ������" << endl;
	cout << "31 - ����� ������ ������������ ���� ���������� �����" << endl;
	cout << "-------------------------------------------------" << endl;
	cout << "32 - ����������� ���������� ����� ���������� ��������-�����" << endl;
	cout << "33 - �������� ������ maxmin-����" << endl;
	cout << "-------------------------------------------------" << endl;
	cout << "34 - ����� ������������� ������ ���������� �����-����������" << endl;
	cout << "-------------------------------------------------" << endl;
	cout << "0 - �����" << endl;
	while (1) {
		cin >> f;
		switch (f) {
		case 0: return;
		case 1: G.write_v(); break;
		case 2: G.write_e(); break;
		case 3: G.degree_of_vertex(); break;
		case 4: G.write_deg_seq(); break;
		case 5: G.write_list_ad(0); break;
		case 6: G.write_matr_ad(0); break;
		case 7: G.write_matr_in(0); break;
		case 8: G.write_0_and_1_vertexes(); break;
		case 9: G.write_distance_between_vertexes(); break;
		case 10: G.write_eccentricity_of_vertex(); break;
		case 11: G.write_radius_and_diameter(); break;
		case 12: G.write_central_and_peripheral_vertexes(); break;
		case 13: cout << "���������� �������" << endl;
			cout << "����� �������: "; cin >> from;
			G.add_vertex(from); break;
		case 14: cout << "�������� �������" << endl;
			cout << "����� �������: "; cin >> from;
			G.delete_vertex(from); break;
		case 15: cout << "���������� �����" << endl;
			cout << "������ �����: "; cin >> from;
			cout << "����� �����: "; cin >> to;
			G.add_edge(from, to); break;
		case 16: cout << "�������� �����" << endl;
			cout << "������ �����: "; cin >> from;
			cout << "����� �����: "; cin >> to;
			G.delete_edge(from, to); break;
		case 17: G.complement_graph(); break;
		case 18: cout << "������������ �����" << endl;
			cout << "������ �����: "; cin >> from;
			cout << "����� �����: "; cin >> to;
			G.subdivision_of_edge(from, to); break;
		case 19: cout << "���������� �����" << endl;
			cout << "������ �����: "; cin >> from;
			cout << "����� �����: "; cin >> to;
			G.contraction_of_edge(from, to); break;
		case 20: cout << "�������������� ������" << endl;
			cout << "������ �������: "; cin >> from;
			cout << "������ �������: "; cin >> to;
			G.identification_of_vertexes(from, to); break;
		case 21: cout << "������������ �������" << endl;
			cout << "����� �������: "; cin >> from;
			G.duplication_of_vertex(from); break;
		case 22: cout << "����������� �������" << endl;
			cout << "����� �������: "; cin >> from;
			G.multiplication_of_vertex(from); break;
		case 23: G.union_of_grafs(); break;
		case 24: G.connection_of_grafs(); break;
		case 25: G.composition_of_grafs(); break;
		case 26: G.write_list_ad(1); break;
		case 27: G.write_matr_ad(1); break;
		case 28: G.write_matr_in(1); break;
		case 29: G.DFS(); break;
		case 30: G.BFS(); break;
		case 31: G.Prim(); break;
		case 32: cout << "����� ���������� ����� ����� ����� ��������� ���������� ��������-�����" << endl;
			cout << "������ �������: "; cin >> from;
			cout << "������ �������: "; cin >> to;
			G.Bellman_Ford(from, to); break;
		case 33: cout << "����� maxmin-���� ����� ����� ���������" << endl;
			cout << "������ �������: "; cin >> from;
			cout << "������ �������: "; cin >> to;
			G.maxmin(from, to); break;
		case 34: G.Ford_Falkerson(); break;
		default: cout << "�������� ��������" << endl;
		}
	}
	return;
}

int open_file()
{
	char filename[128];
	while (1) {
		memset((void*)filename, 0, sizeof(filename));
		cout << "������� ��� �����, ����������� ����, ��� 0 ��� ������: ";
		cin >> filename;
		if (!strcmp(filename, "0"))
			return 0;
		fin.open(filename);
		if (!fin.is_open()) // ���� ���� �� ������
			cout << "���� �� ������!" << endl;
		else
			return 1;
	}
}

int main()
{
	Graf G;
	int view = 4, f = 0;
	setlocale(LC_ALL, "Russian");

	if (open_file()) {
		cout << "��� ����������� �����:" << endl;
		cout << "1 - ������ ���������" << endl;
		cout << "2 - ������� ���������" << endl;
		cout << "3 - ������� �������������" << endl;
		cout << "4 - ������� �����" << endl;
		cout << "0 - �����" << endl;
		cin >> view;
		switch (view) {
		case 0: return 0;
		case 1: f = G.read_list_ad(); break;
		case 2: f = G.read_matr_ad(); break;
		case 3: f = G.read_matr_in(); break;
		case 4: f = G.read_matr_w(); break;
		default: cout << "�������� ���!" << endl; fin.close(); return 0;
		}
		fin.close();
		if (f) { // ��������� ������� �����������
			cout << "������� �����������!" << endl;
			return 0;
		}
		choose(G);
		cout << endl;
	}

	return 0;
}