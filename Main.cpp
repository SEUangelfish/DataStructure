#define EXCEPTION_DETECTION
#include "pch.h"
#include "SplaySet.h"
using namespace std;
using namespace dsl;


int main() {
	srand(time(NULL));

	SplaySet<int> b;
	for (int i = 1; i < 1000000; i++) {
		b.Emplace(rand());
	}
	for (int i = 1; i < 10000; i++) {
		b.Erase(rand());
		b.Contains(rand());
		b.LowerBound(rand());
		b.Precursor(rand());
		b.Successor(rand());
	}

	SplaySet<int> a(b);

	b.Begin();
	b.Empty();
	b.begin();
	SplaySet<int>::Iterator itr = b.begin();
	++itr;
	--itr;


	for (auto itr = b.begin(); itr != b.end(); ++itr) {
		//cout << itr->Key() << ' ';
	}

	int last = b.begin()->Key() - 1;
	for (auto& i : b) {
		if (i.Key() <= last) {
			cout << "error\n";
			return 0;
		}
	}
}

