#ifndef GENETICAL_HPP
#define GENETICAL_HPP

#include <string>
#include <iostream>
#include <vector>
#include <unistd.h>
#include <csignal>
#include <ctime>
#include <cstdlib>
#include <algorithm>


using namespace std;

int getHoriHeuristic(string &sol);
int getDiaHeuristic(string &sol);
void printBoard(string  s);
int getHeuristic(string sol);
vector<string> getBetterPopulation(vector<string> s);
vector<string> getCrossOver(vector<string> &s);
void getMutation(vector<string> &s);
char pmx(char p, string a, string b);
vector<string> PMXCrossOver(string &p1, string &p2);
void pmxOver(vector<string> &s);
int isPopGood(vector<string> &s);
void printVector(vector<string> s);
vector<string> getSolution(vector<string>  &sol);



#endif
