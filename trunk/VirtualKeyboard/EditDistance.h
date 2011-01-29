#pragma once

#include <string>
#include <vector>

class EditDistance{
public:
	enum Operation{
		Undef,
		Deletion,
		Insertion,
		Replacment
	};

	class OperationObject{
	public:
		OperationObject(){
			opTy = Undef;
			srcOffset = -1;
			tgtOffset = -1;
			srcChar = -1;
			tgtChar = -1;
		}

		OperationObject(Operation opTy, int srcOffset, int tgtOffset, char srcChar, char tgtChar){
			this->opTy = opTy;
			this->srcOffset = srcOffset;
			this->tgtOffset = tgtOffset;
			this->srcChar = srcChar;
			this->tgtChar = tgtChar;
		}

		Operation opTy;
		int srcOffset;
		int tgtOffset;
		char srcChar;
		char tgtChar;
	};

	typedef std::vector<OperationObject> Route;

	class DistanceResult{
	public:
		DistanceResult():distance(-1){}
		DistanceResult(int distance, const Route& route){
			this->distance = distance;
			this->route = route;
		}

		int distance;
		Route route;
	};

private:
	static const int del = 1;
	static const int ins = 2;
	static const int rep = 3;

public:

	// input will be src(1...m) and tgt(1...n)
	// create a 2d array dis[m + 1][n(tgt) + 1]for the string
	// the dis[0..m][0] means from src to empty tgt, delete from src
	// the dis[0][0..n] means from an empty src to tgt, insert to source

	static DistanceResult calc(const std::string& src, const std::string& tgt);
};
