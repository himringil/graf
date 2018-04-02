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
	int list_ad[MAXN][MAXN]; // список смежности
	int matr_ad[MAXN][MAXN]; // матрица смежности
	int matr_ad_2[MAXN][MAXN];
	int matr_in[MAXN][MAXN]; // матрица инцидентности
	int matr_dist[MAXN][MAXN]; // матрица расстояний между вершинами
	int matr_weight[MAXN][MAXN]; // матрица весов/пропскных способностей
	int deleted[MAXN];
	int deleted_2[MAXN];
	int v; // число вершин
	int v_2;
	int e; // число рёбер
	int org; // = 1, если граф ориентированный
	int rad; // радиус графа
	int diam; // диаметр графа
	int del; // количество удалённых вершин
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
	int read_list_ad() // чтение списка смежности
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
	int read_matr_ad() // чтение матрицы смежности
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
					if (matr_ad[i][j] < 0 || i == j && matr_ad[i][j] % 2) // элемент матрицы отрицателен или на диагонали стоит нечётное число
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
	int read_matr_in() // чтение матрицы инцидентности
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

		// проверка корректности введённой матрицы инцидентности
		for (i = 1; i <= e; ++i) {
			k = 0; // число 1 в столбце
			m = 0; // число -1 в столбце
			n = 0; // число 2 в столбце
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
	int read_matr_w() // чтение матрицы весов
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
					if (i == j && matr_weight[i][j]) // на диагонали стоит не ноль
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
	void org_or_neorg() // определение, является ли граф ориентированным
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
	void write_list_ad(int f) // вывод списка смежности
	{
		int i, j;
		streambuf *bak = NULL;
		ofstream file;

		if (f) {
			file.open("output.txt", ios::app);
			bak = cout.rdbuf();  // сохраняем
			cout.rdbuf(file.rdbuf()); // перенапраляем в файл
		}

		cout << "-------------------------------------------------" << endl;
		cout << "Список смежности:" << endl;
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
			cout.rdbuf(bak); // восстанавливаем

		return;
	}
	void write_matr_ad(int f) // вывод матрицы смежности
	{
		int i, j;
		streambuf *bak = NULL;
		ofstream file;

		if (f) {
			file.open("output.txt", ios::app);
			bak = cout.rdbuf();  // сохраняем
			cout.rdbuf(file.rdbuf()); // перенапраляем в файл
		}

		cout << "-------------------------------------------------" << endl;
		cout << "Матрица смежности:" << endl;
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
			cout.rdbuf(bak); // восстанавливаем

		return;
	}
	void write_matr_in(int f) // вывод матрицы инцидентности
	{
		int i, j;
		streambuf *bak = NULL;
		ofstream file;

		if (f) {
			file.open("output.txt", ios::app);
			bak = cout.rdbuf();  // сохраняем
			cout.rdbuf(file.rdbuf()); // перенапраляем в файл
		}

		cout << "-------------------------------------------------" << endl;
		cout << "Матрица инцидентности:" << endl;
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
			cout.rdbuf(bak); // восстанавливаем

		return;
	}
	void write_matr_w() // вывод матрицы весов
	{
		int i, j;
		cout << "-------------------------------------------------" << endl;
		cout << "Матрица весов:" << endl;
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
	void write_v() // вывод числа вершин
	{
		cout << "v = " << v - del << endl;
		return;
	}
	void write_e() // вывод числа рёбер графа
	{
		cout << "e = " << e << endl;
		return;
	}
	void list_ad_to_matr_ad() // список смежности в матрицу смежности
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
	void list_ad_to_matr_in() // список смежности в матрицу инцидентности
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
	void matr_ad_to_list_ad() // матрица смежности в список смежности
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
	void matr_in_to_list_ad() // матрица инцидентности в список смежности
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
	void degrees() // подсчёт степеней вершин графа
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
	void degree_of_vertex() // определение степени/полустепеней вершины графа
	{
		int vn;
		int d[2];
		cout << "Вершина: ";
		cin >> vn;
		if (vn < 1 || vn > v || deleted[vn]) {
			cout << "Неверная вершина!" << endl;
			return;
		}
		d[0] = matr_ad[vn][0];
		if (org) d[1] = matr_ad[0][vn];
		else d[1] = -1;
		if (d[1] == -1)
			cout << "Степень = " << d[0] << endl;
		else
			cout << "Степень исхода = " << d[0] << endl << "Полустепень захода = " << d[1] << endl;
		return;
	}
	void write_deg_seq() // вывод степенной последовательности
	{
		int i;
		if (org) cout << "Последовательность полустепеней исхода:	";
		else cout << "Степенная последовательность:	";
		for (i = 1; i <= v; ++i)
			if (!deleted[i])
				cout << matr_ad[i][0] << "   ";
		cout << endl;
		if (org) {
			cout << "Последовательность полустепеней захода:	";
			for (i = 1; i <= v; ++i)
				if (!deleted[i])
					cout << matr_ad[0][i] << "   ";
			cout << endl;
		}
		return;
	}
	void write_0_and_1_vertexes() // вывод изолированных вершин и листьев (истоков и стоков)
	{
		int i;
		if (!org) {
			cout << "Изолированные вершины -	";
			for (i = 1; i <= v; ++i)
				if (matr_ad[i][0] == 0 && !deleted[i]) cout << i << "  ";
			cout << endl;
			cout << "Висячие вершины -	";
			for (i = 1; i <= v; ++i)
				if (matr_ad[i][0] == 1) cout << i << "  ";
			cout << endl;
		}
		else {
			cout << "Изолированные вершины -	";
			for (i = 1; i <= v; ++i)
				if (matr_ad[i][0] == 0 && matr_ad[0][i] == 0)
					cout << i << "  ";
			cout << endl;
			cout << "Истоки -	";
			for (i = 1; i <= v; ++i)
				if (matr_ad[0][i] == 0 && matr_ad[i][0] > 0)
					cout << i << "  ";
			cout << endl;
			cout << "Стоки -	";
			for (i = 1; i <= v; ++i)
				if (matr_ad[i][0] == 0 && matr_ad[0][i] > 0)
					cout << i << "  ";
			cout << endl;
		}
	}
	void make_matr_distance() // построение матрицы расстояний между вершинами
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

		for (i = 1; i <= v; ++i) { // определение эксцентриситетов вершин
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
	void write_distance_between_vertexes() // вывод расстояния между двумя вершинами
	{
		int from, to;
		cout << "Начальная вершина: ";
		cin >> from;
		cout << "Конечная вершина: ";
		cin >> to;
		if (from < 1 || from > v || to < 1 || to > v || deleted[from] || deleted[to])
			cout << "Неверные вершины!" << endl;
		else if (matr_dist[from][to] != BIG)
			cout << "Расстояние = " << matr_dist[from][to] << endl;
		else
			cout << "Нет пути между вершинами!" << endl;
		return;
	}
	void write_eccentricity_of_vertex() // вывод эксцентриситета вершины
	{
		int vn;
		cout << "Вершина: ";
		cin >> vn;
		if (vn < 1 || vn > v || deleted[vn])
			cout << "Вершина не существует!" << endl;
		else if (matr_dist[vn][0] == BIG)
			cout << "У вершины нет эксцентриситета!" << endl;
		else
			cout << "Эксцентриситет вершины = " << matr_dist[vn][0] << endl;
		return;
	}
	void find_radius_and_diameter() // определение радиуса и диаметра графа
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
	void write_radius_and_diameter() // вывод радиуса и диаметра графа
	{
		if (rad == BIG)
			cout << "Радиуса нет!" << endl;
		else
			cout << "Радиус = " << rad << endl;
		if
			(diam == -1) cout << "Диаметра нет!" << endl;
		else
			cout << "Диаметр = " << diam << endl;
		return;
	}
	void write_central_and_peripheral_vertexes() // вывод центра  и периферийных вершин графа
	{
		int i;
		if (rad == BIG)
			cout << "Центральных вершин нет!" << endl;
		else {
			cout << "Центральные вершины:	";
			for (i = 1; i <= v; ++i)
				if (matr_dist[i][0] == rad)
					cout << i << " ";
			cout << endl;
		}
		if (diam == -1)
			cout << "Периферийных вершин нет!" << endl;
		else {
			cout << "Периферийные вершины:	";
			for (i = 1; i <= v; ++i)
				if (matr_dist[i][0] == diam)
					cout << i << " ";
			cout << endl;
		}
		return;
	}
	void add_vertex(int from) // добавление вершины
	{
		if (from > v)
			v = from;
		else if (from >= 1 && from <= v) {
			if (deleted[from])
				deleted[from] = 0;
			else
				cout << "Вершина уже существует" << endl;
		}
		else
			cout << "Неверная вершина" << endl;
		return;
	}
	void delete_vertex(int vn) // удаление вершины
	{
		int i;
		if (vn < 1 || vn > v || deleted[vn])
			cout << "Вершина не существует!" << endl;
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
	void add_edge(int from, int to) // добавление ребра
	{
		if (from < 1 || to < 1)
			cout << "Неверные вершины!" << endl;
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
	void delete_edge(int from, int to) // удаление ребра
	{
		if (from < 1 || from > v || to < 1 || to > v)
			cout << "Неверные вершины!" << endl;
		if (!matr_ad[from][to])
			cout << "Ребро не существует!" << endl;
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
	void complement_graph() // определение дополнения графа
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
	void subdivision_of_edge(int from, int to) // подразбиение ребра
	{
		if (from < 1 || from > v || to < 1 || to > v || deleted[from] || deleted[to])
			cout << "Неверные вершины!" << endl;
		if (!matr_ad[from][to])
			cout << "Ребро не существует!" << endl;
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
	void contraction_of_edge(int from, int to) // стягивание ребра
	{
		if (from < 1 || from > v || to < 1 || to > v || deleted[from] || deleted[to])
			cout << "Неверные вершины!" << endl;
		if (!matr_ad[from][to])
			cout << "Ребро не существует!" << endl;
		else {
			delete_edge(from, to);
			identification_of_vertexes(from, to);
		}
	}
	void identification_of_vertexes(int from, int to) // отождествление вершин
	{
		int i;
		if (from < 1 || from > v || to < 1 || to > v || deleted[from] || deleted[to])
			cout << "Неверные вершины!" << endl;
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
	void duplication_of_vertex(int vn) // дублирование вершины
	{
		int i;
		if (vn < 1 || vn > v || deleted[vn])
			cout << "Вершина не существует!" << endl;
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
	void multiplication_of_vertex(int vn) // размножение вершины
	{
		if (vn < 1 || vn > v || deleted[vn])
			cout << "Вершина не существует!" << endl;
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
		cout << "Введите имя файла, содержащего матрицу смежности графа: ";
		cin >> filename;
		f.open(filename);
		if (!f.is_open()) { // если файл не открыт
			cout << "Файл не найден!" << endl;
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
					if (matr_ad_2[i][j] < 0 || i == j && matr_ad_2[i][j] % 2) { // элемент матрицы отрицателен или на диагонали стоит нечётное число
						cout << "Матрица некорректна!" << endl;
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
		cout << "Матрица смежности:" << endl;
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
	void union_of_grafs() // объединение графов
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
	void connection_of_grafs() // соединение графов
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
	void composition_of_grafs() // произведение графов
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
	void DFS() // поиск в глубину
	{
		int T[MAXN][MAXN] = { 0 };
		int num[MAXN] = { 0 };
		int vn = 1, i, j;
		for (i = 1; i <= v; ++i)
			if (!num[i] && !deleted[i])
				deep_search(T, num, i);
		cout << "-------------------------------------------------" << endl;
		cout << "Остов графа, найденный поиском в глубину:" << endl;
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
	void BFS() // поиск в ширину
	{
		int T[MAXN][MAXN] = { 0 }; // остов
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
		cout << "Остов графа, найденный поиском в ширину:" << endl;
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
		cout << "Остов минимального веса, найденный алгоритмом Прима:" << endl;
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
		cout << "Вес остова:" << weight << endl;
		cout << "-------------------------------------------------" << endl;
	}
	void Bellman_Ford(int from, int to)
	{
		int i, j, k;
		int d[MAXN], p[MAXN];
		if (from < 1 || from > v || deleted[from] || to < 1 || to > v || deleted[to])
			cout << "Неверные вершины!" << endl;
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
				cout << "пути между вершинами нет!" << endl;
				return;
			}
			cout << "длина пути: " << d[to] << endl;
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
			cout << "Неверные вершины!" << endl;
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
			cout << "вес пути: " << c[to] << endl;
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
		for (u = 1; u <= v; ++u) // сначала отмечаем все вершины не пройденными
			color[u] = WHITE;
		*head = 0; // начало очереди 0
		*tail = 0; // хвост 0
		q[*tail] = start;
		++(*tail); // хвостом становится следующий элемент
		color[start] = GRAY;
		pred[start] = -1;
		while (*head != *tail) { // пока хвост не совпадёт с головой
			u = q[*head]; // записать в u значение головы
			++(*head);
			color[u] = BLACK; // вершина u отмечается как прочитанная
			for (w = 1; w <= v; ++w) { // смотрим смежные вершины
				if (color[w] == WHITE && matr_weight[u][w] - flow[u][w] > 0) { // если не пройдена и не заполнена
					q[*tail] = w; // записать в хвост
					++(*tail); // хвостом становится следующий элемент
					color[w] = GRAY; // цвет серый
					pred[w] = u; // путь обновляем
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
			for (u = stock; pred[u] > 0; u = pred[u]) // найти минимальный поток в сети
				delta = min(delta, matr_weight[pred[u]][u] - flow[pred[u]][u]);
			for (u = stock; pred[u] > 0; u = pred[u]) { // по алгоритму Форда-Фалкерсона
				flow[pred[u]][u] += delta;
				flow[u][pred[u]] -= delta;
			}
			maxflow += delta; // повышаем максимальный поток
		}
		return maxflow;
	}
	void Ford_Falkerson()
	{
		int i, j;
		int flow[MAXN][MAXN]; // матрица потоков через рёбра
		int color[MAXN]; // цвета для вершин
		int pred[MAXN]; // массив пути
		int head, tail;
		int q[MAXN]; // очередь
		for (i = 1; i <= v; ++i) // исток
			if (matr_ad[0][i] == 0 && matr_ad[i][0] > 0)
				break;
		for (j = 1; j <= v; ++j) // сток
			if (matr_ad[j][0] == 0 && matr_ad[0][j] > 0)
				break;
		cout << "Максимальный поток из " << i << " в " << j << ": " << max_flow(i, j, flow, color, pred, &head, &tail, q) << endl;
		cout << "Матрица потоков через рёбра: " << endl;
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
	cout << endl << "Возможные действия:" << endl;
	cout << "-------------------------------------------------" << endl;
	cout << "1 - определение числа вершин" << endl;
	cout << "2 - определение числа граней" << endl;
	cout << "3 - определение степени вершины" << endl;
	cout << "4 - определение степенной последовательности" << endl;
	cout << "-------------------------------------------------" << endl;
	cout << "5 - вывод списка смежности" << endl;
	cout << "6 - вывод матрицы смежности" << endl;
	cout << "7 - вывод матрицы инцидентности" << endl;
	cout << "-------------------------------------------------" << endl;
	cout << "8 - определение вершин со степенью 0 и 1" << endl;
	cout << "9 - определение расстояния между двумя вершинами" << endl;
	cout << "10 - определение эксцентриситета вершины" << endl;
	cout << "11 - определение радиуса и диаметра графа" << endl;
	cout << "12 - определение центра и периферийных вершин графа" << endl;
	cout << "-------------------------------------------------" << endl;
	cout << "13 - добавление вершины" << endl;
	cout << "14 - удаление вершины" << endl;
	cout << "15 - добавление ребра" << endl;
	cout << "16 - удаление ребра" << endl;
	cout << "-------------------------------------------------" << endl;
	cout << "17 - определение дополнения графа" << endl;
	cout << "-------------------------------------------------" << endl;
	cout << "18 - подразбиение ребра" << endl;
	cout << "19 - стягивание ребра" << endl;
	cout << "20 - отождествление вершин" << endl;
	cout << "21 - дублирование вершины" << endl;
	cout << "22 - размножение вершины" << endl;
	cout << "-------------------------------------------------" << endl;
	cout << "23 - объединение графов" << endl;
	cout << "24 - соединение графов" << endl;
	cout << "25 - произведение графов" << endl;
	cout << "-------------------------------------------------" << endl;
	cout << "26 - вывод списка смежности в файл" << endl;
	cout << "27 - вывод матрицы смежности в файл" << endl;
	cout << "28 - вывод матрицы инцидентности в файл" << endl;
	cout << "-------------------------------------------------" << endl;
	cout << "29 - построение остова поиском в глубину" << endl;
	cout << "30 - построение остова поиском в ширину" << endl;
	cout << "31 - поиск остова минимального веса алгоритмом Прима" << endl;
	cout << "-------------------------------------------------" << endl;
	cout << "32 - определение кратчайших путей алгоритмом Беллмана-Форда" << endl;
	cout << "33 - алгоритм поиска maxmin-пути" << endl;
	cout << "-------------------------------------------------" << endl;
	cout << "34 - поиск максимального потока алгоритмом Форда-Фалкерсона" << endl;
	cout << "-------------------------------------------------" << endl;
	cout << "0 - выход" << endl;
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
		case 13: cout << "добавление вершины" << endl;
			cout << "номер вершины: "; cin >> from;
			G.add_vertex(from); break;
		case 14: cout << "удаление вершины" << endl;
			cout << "номер вершины: "; cin >> from;
			G.delete_vertex(from); break;
		case 15: cout << "добавление ребра" << endl;
			cout << "начало ребра: "; cin >> from;
			cout << "конец ребра: "; cin >> to;
			G.add_edge(from, to); break;
		case 16: cout << "удаление ребра" << endl;
			cout << "начало ребра: "; cin >> from;
			cout << "конец ребра: "; cin >> to;
			G.delete_edge(from, to); break;
		case 17: G.complement_graph(); break;
		case 18: cout << "подразбиение ребра" << endl;
			cout << "начало ребра: "; cin >> from;
			cout << "конец ребра: "; cin >> to;
			G.subdivision_of_edge(from, to); break;
		case 19: cout << "стягивание ребра" << endl;
			cout << "начало ребра: "; cin >> from;
			cout << "конец ребра: "; cin >> to;
			G.contraction_of_edge(from, to); break;
		case 20: cout << "отождествление вершин" << endl;
			cout << "первая вершина: "; cin >> from;
			cout << "вторая вершина: "; cin >> to;
			G.identification_of_vertexes(from, to); break;
		case 21: cout << "дублирование вершины" << endl;
			cout << "номер вершины: "; cin >> from;
			G.duplication_of_vertex(from); break;
		case 22: cout << "размножение вершины" << endl;
			cout << "номер вершины: "; cin >> from;
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
		case 32: cout << "поиск кратчайших путей между двумя вершинами алгоритмом Беллмана-Форда" << endl;
			cout << "первая вершина: "; cin >> from;
			cout << "вторая вершина: "; cin >> to;
			G.Bellman_Ford(from, to); break;
		case 33: cout << "поиск maxmin-пути между двумя вершинами" << endl;
			cout << "первая вершина: "; cin >> from;
			cout << "вторая вершина: "; cin >> to;
			G.maxmin(from, to); break;
		case 34: G.Ford_Falkerson(); break;
		default: cout << "Неверное действие" << endl;
		}
	}
	return;
}

int open_file()
{
	char filename[128];
	while (1) {
		memset((void*)filename, 0, sizeof(filename));
		cout << "Введите имя файла, содержащего граф, или 0 для выхода: ";
		cin >> filename;
		if (!strcmp(filename, "0"))
			return 0;
		fin.open(filename);
		if (!fin.is_open()) // если файл не открыт
			cout << "Файл не найден!" << endl;
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
		cout << "Вид задаваемого графа:" << endl;
		cout << "1 - список смежности" << endl;
		cout << "2 - матрица смежности" << endl;
		cout << "3 - матрица инцидентности" << endl;
		cout << "4 - матрица весов" << endl;
		cout << "0 - выход" << endl;
		cin >> view;
		switch (view) {
		case 0: return 0;
		case 1: f = G.read_list_ad(); break;
		case 2: f = G.read_matr_ad(); break;
		case 3: f = G.read_matr_in(); break;
		case 4: f = G.read_matr_w(); break;
		default: cout << "Неверный вид!" << endl; fin.close(); return 0;
		}
		fin.close();
		if (f) { // считанная матрица некорректна
			cout << "Матрица некорректна!" << endl;
			return 0;
		}
		choose(G);
		cout << endl;
	}

	return 0;
}