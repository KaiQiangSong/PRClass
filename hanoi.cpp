#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <map>
#include <ctime>
#include <algorithm>
#include <iomanip>

using namespace std;

#define OFFSET (3)
#define BASE (1 << OFFSET)
#define BASE_MOD (BASE - 1)
#define END_STATE (7)
#define START_STATE (448)


int N,end_state;

double R[100][100];
double Q[100][100];
bool Graph[100][100];


std::vector<int> SV;
std::map<int,int> Index;

std::vector<int> Edge[27];


bool bound(int x)
{
	return ((x<BASE) && (x>=0));
}

int get_bit(int x, int k)
{
	return (x >> k) & 1;
}

bool is_state(int a, int b, int c)
{
	if (!bound(a) || !bound(b) || !bound(c)) return false;
	for (int i = 0; i < OFFSET; ++ i)
		if (get_bit(a,i) + get_bit(b,i) + get_bit(c,i) != 1) return false;
	return true;
}

bool is_state(int x)
{
	return is_state((x >> (OFFSET << 1) ) & BASE_MOD , (x >> OFFSET) & BASE_MOD , x & BASE_MOD);
}

int calc_state(int a, int b ,int c)
{
	return (((a << OFFSET) + b) << OFFSET) + c;
}

void show_state(int state)
{
	cout << " State : " << endl;
	for (int i = OFFSET-1; i >= 0 ; --i)
	{
		int col = (state >> (i*OFFSET)) & BASE_MOD;
		cout << "col No." << OFFSET-i << ":";
		for (int j = 0; j < OFFSET; ++j)
			if ((col>>j) & 1) cout << " " << OFFSET-j;
		cout << endl;
	}
}

int get_state()
{
    bool flag = true;
    char ch;
    int a,b,c,state;
    do {
	    do {
    	    cout << "Please input your state." << endl;
		    cout << "Try to input 3 integers." << endl;
    		cout << "Each of the integer represent one col." << endl;
		    cout << "state = 0 for nothing" << endl;
		    cout << "if 1 in this col state += 1" << endl;
    		cout << "if 2 in this col state += 2" << endl;
		    cout << "if 3 in this col state += 4" << endl;
		    cout << "............." << endl;
    	    cin >> a >> b >> c;
	    } while (!is_state(a,b,c));
    	state = calc_state(a,b,c);
	    show_state(state);
	    cout << "Do you mean this state?(y/n)" << endl;
	    cin >> ch;
	    if (ch=='y') flag=false;
	} while (flag);
	return state;
}

int get_top(int x)
{
	int i;
	for (i = OFFSET; i > 0; --i)
		if ((x >> (i-1)) & 1) return (1<<(i-1));
	return 0;
}

int move(int state, int S, int T, int PS, int PT)
{
	state -= (PS << (OFFSET * S));
	state += (PS << (OFFSET * T));
	return state;
}

int add_edge(int x, int y)
{
	x = Index[x]; y = Index[y];
	Graph[x][y] = true;
	Edge[x].push_back(y);
}

void pre_calc(int &N)
{
	/* get State -> Index  && Index -> State*/
	for (int a = 0; a < BASE; ++a)
		for (int b = 0; b < BASE-a; ++b)
		{
			int c = BASE_MOD - a - b;
			if (!is_state(a,b,c)) continue;
			int state = calc_state(a,b,c);
			SV.push_back(state);
			Index[state] = SV.size()-1;
		}
	N = SV.size();
	/*get Action Matrix*/	
	for (int i=0; i<N; ++i)
	{
		int state = SV[i],next_state;
		int top_A = get_top((state >> (OFFSET << 1)) & BASE_MOD);
		int top_B = get_top((state >> OFFSET) & BASE_MOD);
		int top_C = get_top(state & BASE_MOD);
		if (top_A > top_B)
		{
			next_state = move(state,2,1,top_A,top_B);
			add_edge(state,next_state);
		}
		if (top_A > top_C)
		{
			next_state = move(state,2,0,top_A,top_C);
			add_edge(state,next_state);
		}
		if (top_B > top_A)
		{
			next_state = move(state,1,2,top_B,top_A);
			add_edge(state,next_state);
		}
		if (top_B > top_C)
		{
			next_state = move(state,1,0,top_B,top_C);
			add_edge(state,next_state);
		}
		if (top_C > top_A)
		{
			next_state = move(state,0,2,top_C,top_A);
			add_edge(state,next_state);
		}
		if (top_C > top_B)
		{
			next_state = move(state,0,1,top_C,top_B);
			add_edge(state,next_state);
		}
	}
	end_state = Index[END_STATE];
}


void pre_MatrixR(double init)
{
	for (int i = 0; i < N; ++i)
	{
		for (int j = 0; j < N; ++j)
		{
			if (Graph[i][j])
			{
				if (end_state == j)
					R[i][j] = init;
				else R[i][j] = 0;
			} else
			R[i][j] = -1;
		}
	}
}

void calc_MatrixQ(double gamma,int num_episode)
{
	int x,y;
	srand(time(NULL));
	memset(Q,0,sizeof(Q));
	for (int i = 0; i < num_episode; ++i)
	{
		x = rand()%N;
		int cnt = 0;
		while (x!=end_state)
		{
			++cnt; 
			y = Edge[x][rand()%Edge[x].size()];
			double Max = *(std::max_element(Q[y],Q[y]+N));
			Q[x][y] = R[x][y] + gamma * Max;
			x = y;
		}
	}
	/* calc Q Matrix by Q Learning Algorithm */
	double g = *(std::max_element(Q[0],Q[0]+N*N));
	for (int i = 0; i < N; ++i)
		for (int j = 0; j < N; ++j)
			Q[i][j] = Q[i][j] / g;
	/* nomalize Q */
}

void show_Matrix_Q()
{
	for (int i = 0; i < N; ++i)
	{
		for (int j = 0; j < N; ++j)
			cout << setw(5) << setprecision(2)<< Q[i][j];
		cout << endl;
	}
}

void demo()
{
    int state = get_state();
    show_state(state);
    state = Index[state];
    while (state != end_state)
    {
    	double Max = *(std::max_element(Q[state],Q[state]+N));
    	int i;
    	for (i = 0; i < N; ++i)
    		if (Q[state][i] == Max)
    			break;
    	state = i;
    	show_state(SV[i]);
    }
}



int main()
{
    pre_calc(N);
    pre_MatrixR(100);
    calc_MatrixQ(0.8,500);
    show_Matrix_Q();
    demo();
    return 0;
}

