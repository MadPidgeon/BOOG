#pragma once
#include <iostream>
#include <vector>
#include <initializer_list>
#include <string>
#include "bintree.h"

class operator_table {
	std::string operator_id;
	size_t n;
	std::vector<int> M;
public:
	size_t size() const;
	std::string id() const;
	int operator()( int a, int b ) const;
	int& operator()( int a, int b );
	operator_table( std::string id, std::initializer_list<int> L );
	operator_table( std::string id, size_t n );
	~operator_table();
};

std::ostream& operator<<( std::ostream& os, const operator_table& T );