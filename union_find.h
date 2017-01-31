#pragma once
#include <vector>

struct UF : private std::vector<int> {
	using std::vector<int>::size;
	using std::vector<int>::operator[];
	using std::vector<int>::begin;
	using std::vector<int>::end;
	int find( int s );
	bool cup( int a, int b );
	void clear( int N = -1 );
	UF( int N );
	UF() = default;
};
