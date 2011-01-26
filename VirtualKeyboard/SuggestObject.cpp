#include "stdafx.h"
#include<string>
#include<SuggestObject.h>

SuggestObject::SuggestObject(string text, int minDist){
	this->text = text;
	this->minDist = minDist;
}

//bool SuggestObject:: WordSortPredicate(const SuggestObject& d1, const SuggestObject& d2)
//{
	//return d1.getMinDist() < d2.getMinDist();
//}