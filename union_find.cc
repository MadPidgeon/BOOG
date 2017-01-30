#include <vector>
#include <algorithm>
#include <cmath>
#include "union_find.h"

int UF::find( int s ) {
	int i = s, r = s;
	while( at(r) != r )
		r = at(r);
	while( at(s) != s ) {
		i = at(s);
		at(s) = r;
		s = i;
	}
	return s;
}

bool UF::cup( int a, int b ) {
	int y = find(b);
	int x = find(a);
	at( std::max(x,y) ) = std::min(x,y);
	return x != y;
}

void UF::clear( int N ) {
	if( N >= 0 )
		resize( N );
	std::iota( begin(), end(), 0 );
}

UF::UF( int N ) {
	clear( N );
}