#include "StdAfx.h"
#include "EditDistance.h"

#include <algorithm>
//#include <iostream>
#include <cassert>

using namespace std;

EditDistance::DistanceResult EditDistance::calc( const string& src, const string& tgt )
{
	int srcLen = src.size();
	int tgtLen = tgt.size();

	vector<vector<int>> dis;
	dis.resize(srcLen + 1);

	for_each(dis.begin(), dis.end(), [tgtLen](vector<int>& inner){
		inner.resize(tgtLen + 1);
	});

	for (int i = 0; i <= srcLen; ++i)
		dis[i][0] = del * i;

	for (int j = 0; j <= tgtLen; ++j)
		dis[0][j] = ins * j;

	for (int j = 1; j <= tgtLen; ++j)
		for (int i = 1; i <= srcLen; ++i){
			if (src[i - 1] == tgt[j - 1])
				dis[i][j] = dis[i-1][j-1]; //dis[0][0] means both src and tgt is empty
			//dis[1][1] mean both src and tgt has one char
			//so, dis[1][1] stands for src[0] and tgt[0]
			else{
				//remember the del and add
				// why dis[i-1][j] => dis[i][j] is delete in src
				// i is src from i-1 to i
				// j is tgt from j to j
				// now we want to convert src to tgt
				// and tgt no change, src string increase 1, means that we don't need the char ar src[i-1]
				// dis[i][j] means the min edit distance from src[0...i] => tgt[0...j]
				dis[i][j] = min(min (dis[i-1][j] + del, dis[i][j-1] + ins), dis[i-1][j-1] + rep);
			}
		}

		//get the mini changing route
		Route route;
		int x = srcLen;
		int y = tgtLen;
		while ( ! (x == 0 && y == 0) ){
			int del1 = -1;
			if (x != 0)
				del1 = dis[x - 1][y] + del;

			int ins1 = -1;
			if (y != 0)
				ins1 = dis[x][y-1] + ins;

			int rep1 = -1;
			if (x != 0 && y != 0)
				if (src[x-1] == tgt[y-1]) 
					rep1 = dis[x-1][y-1];
				else
					rep1 = dis[x-1][y-1] + rep;

			if (dis[x][y] == del1){
				route.push_back(OperationObject(Deletion, x-1, -1, src[x-1], -1));
				//cout<<"delete "<<src[x-1]<< " at src:"<<x<<endl;
				--x;
			}
			else if (dis[x][y] == ins1){
				route.push_back(OperationObject(Insertion, x-1, -1, -1, tgt[y-1]));
				//cout<<"insert "<<tgt[y-1]<< " after src:"<<x<<endl;
				--y;
			}
			else if (dis[x][y] == rep1){
				if (src[x-1] != tgt[y-1]){
					route.push_back(OperationObject(Replacment, x-1, y-1, src[x-1], tgt[y-1]));
					//cout<<"replace "<<src[x-1]<< " at src:"<<x<<" with tgt:"<<y<<endl;
				}
				--x;
				--y;
			}else
				assert(0);
		}

		return DistanceResult(dis[srcLen][tgtLen], route);
}
