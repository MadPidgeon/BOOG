#include <map>
#include <string>
#include "trie.h"

using namespace std;

bool trie::node::compress( const char* s, std::string& r ) const {
	if( *s == 0 )
		return v == 1;
	if( c.count(*s) == 0 )
		return false;
	if( c.size() > 1 or v == 1 )
		r.push_back(*s);
	return c.at(*s)->compress( s+1, r );
}


bool trie::node::expand( const char* s, std::string& r ) const {
	if( v == 1 and *s == 0 )
		return true;
	if( c.size() == 0 )
		return false;
	if( c.size() == 1 ) {
		const auto& d = *c.begin();
		r.push_back( d.first );
		return d.second->expand( s + ( v == 1 ), r );
	}
	r.push_back(*s);
	return c.at(*s)->expand( s+1, r );
}

const trie::node* trie::node::ifget( const char* s ) const {
	if( *s == 0 and v == 1 )
		return this;
	if( c.count( *s ) )
		return c.at(*s)->ifget( s+1 );
	return nullptr;
}

trie::node* trie::node::insget( const char* s ) {
	if( *s == 0 ) {
		v = 1;
		return this;
	}
	if( c.count(*s) == 0 )
		c[*s] = new node;
	return c[*s]->insget( s+1 );
}

int trie::node::prefix( const char* s, int d ) const {
	if( *s and c.count(*s) )
		return c.at(*s)->prefix( s+1, d+1 );
	return d;
}

void trie::node::print( std::ostream& os, std::string& s ) const {
	auto itr = c.begin();
	if( itr == c.end() ) {
		os << "\n";
		return;
	}
	os << itr->first;
	s.push_back( c.size() == 1 ? ' ' : '|' );
	itr->second->print( os, s );
	for( ++itr; itr != c.end(); ++itr ) {
		s.back() = '+';
		os << s << itr->first;
		s.back() = (std::next(itr,1) == c.end()) ? ' ' : '|';
		itr->second->print( os, s );
	}
	s.pop_back();
}

trie::node::~node() {
	for( auto& x : c )
		delete x.second;
}

trie::node::node() {
	v = 0;
}

void trie::insert( const std::string& s ) {
	root.insget( s.c_str() );
}

int trie::count( const std::string& s ) const {
	return root.ifget( s.c_str() ) != nullptr;
}

std::string trie::compress( const std::string& s ) const {
	std::string r;
	if( root.compress( s.c_str(), r ) )
		return r;
	return "";
}

std::string trie::expand( const std::string& s ) const {
	std::string r;
	if( root.expand( s.c_str(), r ) )
		return r;
	return "";
}

void trie::print( ostream& os ) const {
	os << "*";
	std::string s;
	root.print( os, s );
}