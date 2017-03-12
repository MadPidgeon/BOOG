#include <vector>
#include <cmath>
#include "misc.h"

using namespace std;

all_ordered_tuples::iterator::iterator( int n, int r ) : _n(n), _r(r), _tuple( _r, 0 ) {
}

all_ordered_tuples::iterator::iterator( const self_type& other ) {
	_n = other._n;
	_r = other._r;
	_tuple = other._tuple;
}

size_t all_ordered_tuples::size() const {
	return size_t( pow( _n, _r ) );
}


all_ordered_tuples::iterator::self_type all_ordered_tuples::iterator::operator++(int) { 
	self_type i = *this; 
	++(*this); 
	return i; 
}

all_ordered_tuples::iterator::self_type& all_ordered_tuples::iterator::operator++() {
	for( int i = _r-1; i >= 0; i-- ) {
		_tuple[i] += 1;
		if( _tuple[i] == _n )
			_tuple[i] = 0;
		else
			return *this;
	}
	_n = -1;
	return *this;
}

all_ordered_tuples::iterator::reference all_ordered_tuples::iterator::operator*() { 
	return _tuple; 
}

all_ordered_tuples::iterator::pointer all_ordered_tuples::iterator::operator->() { 
	return &_tuple; 
}

bool all_ordered_tuples::iterator::operator==(const self_type& rhs) { 
	return _n == rhs._n && _tuple == rhs._tuple;; 
}

bool all_ordered_tuples::iterator::operator!=(const self_type& rhs) { 
	return _n != rhs._n || _tuple != rhs._tuple; 
}

all_ordered_tuples::iterator all_ordered_tuples::begin() const { 
	return iterator( _n, _r ); 
}

all_ordered_tuples::iterator all_ordered_tuples::end() const { 
	return iterator( -1, _r ); 
}

all_ordered_tuples::all_ordered_tuples( int n, int r ) : _n(n), _r(r) {}