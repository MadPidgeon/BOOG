#pragma once
#include <vector>
#include <map>
#include <iostream>
#include <cstddef>

template<typename T>
std::ostream& operator<<( std::ostream& os, const std::vector<T>& s ) {
	if( s.empty() )
		os << "Ø";
	else {
		auto x = s.begin();
		os << "[" << (*x);
		while( (++x) != s.end() )
			os << "," << (*x);
		os << "]";
	}
	return os;
}

template<typename T, typename U>
std::ostream& operator<<( std::ostream& os, const std::map<T,U>& s ) {
	if( s.empty() )
		os << "Ø";
	else {
		auto x = s.begin();
		os << "{" << x->first << " -> " << x->second;
		while( (++x) != s.end() )
			os << "," << x->first << " -> " << x->second;
		os << "}";
	}
	return os;
}

class all_ordered_tuples {
	int _n;
	int _r;
public:
	class iterator {
		int _n;
		int _r;
		std::vector<int> _tuple;
	public:
		typedef iterator self_type;
		typedef std::vector<int> value_type;
		typedef std::vector<int>& reference;
		typedef std::vector<int>* pointer;
		typedef std::forward_iterator_tag iterator_category;
		typedef size_t difference_type;
		iterator( int n, int r );
		iterator( const self_type& other );
		self_type& operator++();
		self_type operator++(int);
		reference operator*();
		pointer operator->();
		bool operator==(const self_type& rhs);
		bool operator!=(const self_type& rhs);
	};
	iterator begin() const;
	iterator end() const;
	size_t size() const;
	all_ordered_tuples( int n, int r );
};
