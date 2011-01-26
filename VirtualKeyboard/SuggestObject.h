#include<string>
using namespace std;

class SuggestObject{
public:
	SuggestObject(string text, int minDist);

	int getMinDist() const { return minDist; }

	string getText() const {return text;}

	//bool WordSortPredicate(const SuggestObject& d1, const SuggestObject& d2);

private:
	string text;
	int minDist;
};