

#include "GeneticAl.hpp"

#include <i386/signal.h>
#include <sys/signal.h>
#include <iterator>


#define toDigit(c) (c-'0')
#define toChar(c) (c+'0')
#define ALRM 5 		//seconds
using namespace std;

sig_atomic_t volatile done = 0;
void game_over(int) { done = 1; }

void printBoard(string  s)
{
	string top = "_1_2_3_4_5_6_7_8";
	
	
	cout<<top<<"_"<<endl;
	for(int i = 0;i<8;i++)
	{
		string st = "|";
		for(int j=0;j<8;j++)
		{
			if(toDigit(s[j]) -1 == i)
			{
				
				st+='Q';
			}
			else {
				st+='_';
			}
			st +="|";
		}
		cout<<st<<endl;
	}
	
}

int getHoriHeuristic(string &sol)
{
	//check if a pair of queen is attacking each other horizontally
	int h = 0;
	for(int i=0;i<sol.size()-2;i++)
	{
		if(sol.substr(i+1).find(sol[i]) != -1)
			++h;
	}
	return h;
}
int getDiaHeuristic(string &sol)
{
	//check if a pair of queen is attacking each other diagonally
	int h = 0;
	for(int i=0;i<sol.size()-1;i++)
	{
		int x = toDigit(sol[i]);
//		cout<<"\nx : "<<x<<endl;
		int a = 1;
		for(int j=i+1;j<sol.size();j++)
		{
			
			int y = toDigit(sol[j]);
//			cout<<y;
			if( ((x+a == y)) || ((x-a == y)) )
			{
//				cout<<x<<" "<<a<<" "<<y<<endl;
//				cout<<"col "<<i+1<<" and col "<<j+1<<" diag"<<endl;
				h++;
//				break;
			}
			a++;
			
			
		}
	}
	
	return h;
}
int getHeuristic(string sol)
{
	//heuristic is the number of pair of queens attacking each other
	int h = 0;

	h += getHoriHeuristic(sol);
	h += getDiaHeuristic(sol);
	return h;	
}

vector<string> getBetterPopulation(vector<string> s)
{
	//get better population by randomly selecting two solutions,
	//comparing their heuristics and keeping the one with better heuristic

	vector<string> better_pop;
	int population_size = s.size();
	srand(time(0));
	
	for(int i=0;i<population_size;i++)
	{
		int a = rand()%population_size;
		int b = rand()%population_size;
//		cout<<" random "<<a<<" h = "<<getHeuristic(s[a])<<" vs "<<b<<" h= "<< getHeuristic(s[b]);
		if(getHeuristic(s[a]) < getHeuristic(s[b]) ) 
		{
			better_pop.push_back(s[a]);
//			cout<<" selected "<<a<<endl;
		}
		else {
			better_pop.push_back(s[b]);
//			cout<<" selected "<<b<<endl;
		}

			
	}
	return better_pop;
}

vector<string> getCrossOver(vector<string> &s)
{
	//randomly select a pair of solutions, randomly select a size to cut off at and Crossover the cutoff parts.
	//not useful for 8 queen problem because the problem requires for a gene to have non repetitive pattern
	//causes infinite loop with Mutation.
	vector<string> new_st;
	int population_size = s.size();
	int size = s[0].size();
	
	srand(time(0));
	for(int i =0;i< int(population_size*0.5);i++)
	{
		int cutoff = 0;
		int a = rand() % population_size;
		int b = rand() % population_size;
		while( cutoff == 0)
		{
			cutoff = rand() % (size-1);

		}
		string n1 = s[a];
		string n2 = s[b];
		string temp = s[a].substr(cutoff);
		n2.replace(cutoff,size-cutoff,temp);

		temp = s[b].substr(cutoff);
		n1.replace(cutoff,size-cutoff,temp);
		new_st.push_back(n1);
		new_st.push_back(n2);
	}
	if(size%2 != 0) new_st.push_back(s[(s.size()/2)]);

	return new_st;
}

void getMutation(vector<string> &s)
{
	//randomly select a solution and select a random position and a random value to replace it with and replace it.
	//doesnot work with getCrossOver
	int size = s[0].size();
	int population_size = s.size();
	
	srand(time(0));
	
	for(int i=0;i<population_size ;i++)
	{
		int cutoff = rand() % (size-1);
		int replace = rand() % size + 1 ;
		int pos = s[i].find(toChar(replace));
		char c = s[i][cutoff];
		s[i][cutoff] = toChar(replace); 
		s[i][pos] = c;
		
	}

}

char pmx(char p, string a, string b)
{
	int pos;
	pos = a.find(p);
	if(pos >-1)
	{
		return b[pos];
	}
	pos = b.find(p);
	while(pos >-1)
	{
		p = a[pos];
		pos = b.find(p);
	}
	return p;
}

vector<string> PMXCrossOver(string &p1, string &p2)
{
	vector<string> retVal;
	
	//randomize this
	string a = p1.substr(3,3);
	string b = p2.substr(3,3);
	for(int i = 0; i<p1.size(); i++)
	{
		p1[i]= pmx(p1[i], a, b);
		p2[i] = pmx(p2[i], b, a);
	}
	retVal.push_back(p1);
	retVal.push_back(p2);
	return retVal;	
}

void pmxOver(vector<string> &s)
{
	//partially mapped crossover

	vector<string> returnedVal;
//	int size = s[0].size();
	
	int population_size = s.size();
	
	srand(time(0));
	
	for(int i=0;i<(population_size*0.5);i++)
	{
		int a = rand() % population_size;
		int b = rand() % population_size;
		returnedVal = PMXCrossOver(s[a], s[b]);
		s[a] = returnedVal[0];
		s[b] = returnedVal[1];
			
	}
}

int isPopGood(vector<string> &s)
{
	// check is the population is within the threshold
	int threshold = 1;
	int size = s.size();
	int out = 1000;
	for(int i =0;i<size;i++)
	{
		if(getHeuristic(s[i]) < threshold)
		{
			out = i;
			break;
		}
	}
	return out;
}

void printVector(vector<string> s)
{
	for(int i = 0;i<s.size();i++) 
		cout<<s[i]<<"\n";
}

vector<string> getSolution(vector<string>  &sol)
{
	signal(SIGALRM, game_over);
	alarm(ALRM);
	
	vector<string> result;
	vector<string> close_result;
	int index = 0;
	int tot_sol = 0;
	int generation = 0;
//	while(c>0)
	{
//		c--;
		while(!done)
		{
			if(tot_sol <92)
			{
				++generation;
			}
//			cout<<"generations : "<<generation<<endl;
			sol = getBetterPopulation(sol);
//			sol = getCrossOver(sol);
			pmxOver(sol);
			getMutation(sol);
			for(int i =0; i< sol.size();i++)
			{
				index = getHeuristic(sol[i]);
				if( (index == 0) & ( find(result.begin() , result.end() , sol[i]) == result.end()) )
				{
					result.push_back(sol[i]);
					cout<<"found "<<sol[i]<<endl;
					printBoard(sol[i]);
					cout<<"total sol :"<<++tot_sol<<endl;
				}

			}
		}
	}
	cout<<"there are :"<<result.size()<<" results"<<endl;
	cout<<"total generations : "<<generation<<endl;

	return result;

}



