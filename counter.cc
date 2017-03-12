#include <vector>
#include <exception>
#include <cmath>
#include <iostream>
#include "bintree.h"
#include "counter.h"
#include "misc.h"

using namespace std;

int evaluate( const binary_tree::node* n, const map<int,int>& values, const map<int,const operator_table*> operators ) {
	if( n->axiomatic_variable() or n->bound_variable() )
		return values.at( n->id );
	if( n->constant() )
		return n->numeric_value();
	return (*operators.at( n->id ))( evaluate( n->child[0], values, operators ), evaluate( n->child[1], values, operators ) );
}

bool check( vector<operator_table> operators, vector<int> operator_mapping, vector<subtree_equivalence> axioms ) {
	size_t n = operators.front().size();
	for( int i = 1; i < operators.size(); ++i )
		if( n != operators[i].size() )
			throw std::invalid_argument( "Operators have different domain!" );
	map<int,int> inverse_operator_mapping;
	map<int,const operator_table*> direct_operator_mapping;
	for( size_t i = 0; i < operator_mapping.size(); ++i ) {
		inverse_operator_mapping[operator_mapping[i]] = i;
		direct_operator_mapping[operator_mapping[i]] = &operators[i];
	}
	vector<int> bound_variable_mapping;
	map<int,int> inverse_bound_variable_mapping;
	for( const auto& axiom : axioms )
		for( int i = 0; i < 2; ++i )
			for( const auto& node : axiom.side(i) )
				if( node.bound_variable() and inverse_bound_variable_mapping.count( node.id ) == 0 ) {
					inverse_bound_variable_mapping[ node.id ] = bound_variable_mapping.size();
					bound_variable_mapping.push_back( node.id );
	}
	size_t q = bound_variable_mapping.size();
	for( const auto& bound_assignment : all_ordered_tuples( n, q ) ) {
		map<int,int> values;
		for( int i = 0; i < q; ++i )
			values[bound_variable_mapping[i]] = bound_assignment[i];
		for( const auto& axiom : axioms ) {
			vector<int> free_variable_mapping;
			map<int,int> inverse_free_variable_mapping;
			for( int s = 0; s < 2; ++s ) {
				for( const auto& node : axiom.side(s) ) {
					if( node.axiomatic_variable() and inverse_free_variable_mapping.count( node.id ) == 0 ) {
						inverse_free_variable_mapping[ node.id ] = free_variable_mapping.size();
						free_variable_mapping.push_back( node.id );
					}
				}
			}
			size_t r = free_variable_mapping.size();
			for( const auto& assignment : all_ordered_tuples( n, r ) ) {
				for( int i = 0; i < r; ++i )
					values[free_variable_mapping[i]] = assignment[i];
				int a = evaluate( axiom.side(0).croot(), values, direct_operator_mapping );
				int b = evaluate( axiom.side(1).croot(), values, direct_operator_mapping );
				// cout << values << ": " << a << "=" << b << endl;
				if( a != b )
					goto bound_fail;
			}
		}
		return true;
		bound_fail:;
	}
	return false;
}



// ******************
// I/O
// ******************

std::ostream& operator<<( std::ostream& os, const operator_table& T ) {
	size_t n = T.size();
	os << "  | ";
	for( size_t i = 0; i < n; ++i )
		os << i << " ";
	os << "\n--+";
	for( size_t i = 0; i < n; ++i )
		os << "--";
	for( size_t j = 0; j < n; ++j ) {
		os << "\n" << j << " | ";
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
