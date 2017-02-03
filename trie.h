#pragma once
#include <map>
#include <iostream>
#include <string>

class trie {
	struct node {
		int v;
		std::map<char,node*> c;

		void print( std::ostream&, std::string& ) const;
		bool compress( const char* s, std::string& r ) const;
		bool expand( const char* s, std::string& r ) const;
		const node* ifget( const char* s ) const;
		node* insget( const char* s );
		int prefix( const char* s, int d = 0 ) const;
		node();
		~node();
	};
	node root;
public:
	void insert( const std::string& s );
	int count( const std::string& s ) const;
	std::string compress( const std::string& s ) const;
	std::string expand( const std::string& s ) const;
	void print( std::ostream& ) const;
	trie() = default;
	~trie() = default;
};
