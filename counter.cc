#include <vector>
#include <exception>
#include <cmath>
#include <iostream>
#include "bintree.h"
#include "counter.h"

using namespace std;

bool check( vector<operator_table> operators, vector<subtree_equivalence> axioms ) {
	// do checks

	return false;
}



// ******************
// I/O
// ******************

std::ostream& operator<<( std::ostream& os, const operator_table& T ) {
	size_t n = T.size();
	os << T.id() << " | ";
	for( size_t i = 0; i < n; ++i )
		os << (i+'a') << " ";
	os << "\n--+";
	for( size_t i = 0; i < n; ++i )
		os << "--";
	for( size_t j = 0; j < n; ++j ) {
		os << "\n" << (j+'a') << " | ";
		for( size_t i = 0; i < n; ++i ) {
			os << T(i,j) << " ";
		}
	}
	return os;
}

// ******************
// Getters/Setters
// ******************

size_t operator_table::size() const {
	return n;
}

std::string operator_table::id() const {
	return operator_id;
}

int operator_table::operator()( int a, int b ) const {
	return M[a+n*b];
}

int& operator_table::operator()( int a, int b ) {
	return M[a+n*b];
}

// ******************
// Constructors
// ******************

operator_table::operator_table( std::string id, size_t _n ) {
	operator_id = id;
	n = _n;
	M.resize(n*n);
	for( int i = 0; i < n*n; ++i )
		M[i] = -1;
}

operator_table::operator_table( std::string id, std::initializer_list<int> L ) {
	operator_id = id;
	int s = L.size();
	n = sqrt(s);
	if( n*n != s )
		throw std::invalid_argument("Initializer list is not square");
	M.resize(s);
	auto itr = L.begin();
	for( int i = 0; i < s; ++i )
		M[i] = *(itr++);
}

operator_table::~operator_table() {
}
