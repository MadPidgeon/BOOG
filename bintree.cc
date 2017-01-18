#include <iostream>
#include <stack>
#include <cassert>
#include <cmath>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <queue>
#include "bintree.h"

using namespace std;

// ******************
// Magic
// ******************

class sizecomp {
public:
	bool operator()( const binary_tree& a, const binary_tree& b ) {
		return a.height() > b.height() or ( a.height() == b.height() and a.size() > b.size() );
	}
};

vector<binary_tree> subtree_equivalence::prove( vector<subtree_equivalence> axioms ) const {
	// ITERATORS CANNOT BE TRUSTED
	// map<binary_tree,const binary_tree::const_iterator> master; // make unordered
	map<binary_tree,binary_tree> master;
	priority_queue<binary_tree,vector<binary_tree>,sizecomp> todo;
	todo.push( first() );
	//master.insert( make_pair( first(), binary_tree::cend() ) );
	binary_tree empty_tree;
	master[first()] = empty_tree;
	vector<binary_tree> proof;
	binary_tree Y;
	while( not todo.empty() ) {
		binary_tree X = move( todo.top() );
		todo.pop();
		for( const auto& axiom : axioms ) {
			//cout << "Axiom: " << axiom << endl;
			for( const auto& N : X ) {
				//cout << "Sub: "; N.print( cout ); cout << endl;
				for( int i = 0; i < 2; ++i ) {
					bool res = axiom.apply( i, X, &N, Y );
					if( res and master.count( Y ) == 0 ) {
						master[Y] = X;
						if( Y == second() ) {
							while( true ) {
								auto result = master.find( Y );
								proof.emplace_back( Y );
								if( result->second == empty_tree )
									return proof;
								Y = move( result->second );
							}
						}
						todo.emplace( move(Y) );
					}
				}
			}
		}
	}
	return {};
}

// ******************
// Tree operations
// ******************

binary_tree::node* binary_tree::node::clone() const {
	return new node( child[0] ? child[0]->clone() : nullptr, child[1] ? child[1]->clone() : nullptr, id );
}

binary_tree::node* binary_tree::node::clonesert( const binary_tree::node* where, binary_tree::node* what, int& sub ) const {
	if( this == where ) {
		sub++;
		return what;
	}
	return new node( child[0] ? child[0]->clonesert( where, what, sub ) : nullptr, child[1] ? child[1]->clonesert( where, what, sub ) : nullptr, id );
}

binary_tree binary_tree::clonesert( const binary_tree::node* where, binary_tree::node* what ) const {
	int subs = 0;
	auto R = root->clonesert( where, what, subs );
	if( subs != 1 )
		throw;
	return binary_tree( R );
}

bool binary_tree::node::leaf() const {
	return child[0] == nullptr and child[1] == nullptr;
}

/*binary_tree::node* binary_tree::node::cascade() {
	if( child[0] ) delete child[0]->cascade();
	if( child[1] ) delete child[1]->cascade();
	return this;
}*/

bool binary_tree::node::grab( const binary_tree::node* rule, map<int,const binary_tree::node*>& result ) const {
	if( rule->height > height )
		return false;
	if( rule->child[0] and child[0] and not child[0]->grab( rule->child[0], result ) )
		return false;
	if( rule->child[1] and child[1] and not child[1]->grab( rule->child[1], result ) )
		return false;
	if( rule->child[0] == nullptr and rule->child[1] == nullptr ) {
		if( result.count( rule->id ) )
			return *result[rule->id] == *this;
		result[rule->id] = this;
	}
	return true;
}

binary_tree::node* binary_tree::node::subsitute( const map<int,const binary_tree::node*>& substitution ) const {
	if( substitution.count(id) )
		return substitution.at(id)->clone();
	else
		return new node( child[0] ? child[0]->subsitute( substitution ) : nullptr, child[1] ? child[1]->subsitute( substitution ) : nullptr, id );
}

bool subtree_equivalence::apply( int i, const binary_tree& parent, const binary_tree::node* itr, binary_tree& res ) const {
	return parent.transform( itr, side(i), side(not i), res );
}

bool binary_tree::transform( const binary_tree::node* pos, const binary_tree& from, const binary_tree& to, binary_tree& res ) const {
	map<int,const binary_tree::node*> substitution;
	if( not pos->grab( from.root.get(), substitution ) )
		return false;
	// what if pos is root?
	res = std::move( clonesert( pos, to.root->subsitute( substitution ) ) );
	return true;
}

// ******************
// Getters & Setters
// ******************

binary_tree& subtree_equivalence::side( int i ) {
	return tree[!!i];
}

const binary_tree& subtree_equivalence::side( int i ) const {
	return tree[!!i];
}

binary_tree& subtree_equivalence::first() {
	return side(0);
}

binary_tree& subtree_equivalence::second() {
	return side(1);
}

const binary_tree& subtree_equivalence::first() const {
	return side(0);
}

const binary_tree& subtree_equivalence::second() const {
	return side(1);
}

size_t binary_tree::size() const {
	return root ? root->size : 0;
}

size_t binary_tree::height() const {
	return root ? (root->height + 1) : 0;
}

// ******************
// Iterators
// ******************

binary_tree::iterator::iterator() : iterator( nullptr ) {
}

binary_tree::iterator::iterator( node* n ) {
	val = nullptr;
	if( n ) {
		next = n;
		operator++();
	}
}

binary_tree::iterator::iterator( node* v, node* n ) {
	val = v;
	next = n;
}

binary_tree::iterator& binary_tree::iterator::operator++() {
	if( loc.empty() and next == nullptr )
		val = nullptr;
	while( not loc.empty() or next != nullptr ) {
		if( next != nullptr ) {
			loc.push( next );
			next = next->child[0];
		} else {
			next = loc.top();
			loc.pop();
			val = next;
			next = next->child[1];
			break;
		}
	}
	return *this;
}

binary_tree::iterator binary_tree::iterator::operator++(int) {
	auto old = binary_tree::iterator( *this );
	operator++();
	return old;
}

binary_tree::iterator::reference binary_tree::iterator::operator*() const {
	return *val;
}

binary_tree::iterator::pointer binary_tree::iterator::operator->() const {
	return val;
}

bool binary_tree::iterator::operator==( const iterator& other ) const {
	return val == other.val;
}

bool binary_tree::iterator::operator!=( const iterator& other ) const {
	return not operator==( other );
}

binary_tree::iterator binary_tree::begin() {
	return iterator( root.get() );
}

binary_tree::iterator binary_tree::end() {
	return iterator();
}

binary_tree::iterator binary_tree::rootitr() {
	return iterator( root.get(), root ? root->child[1] : nullptr );
}

binary_tree::const_iterator::const_iterator() : const_iterator( nullptr ) {
}

binary_tree::const_iterator::const_iterator( const node* n, bool fixed ) {
	val = nullptr;
	if( n ) {
		next = n;
		if( not fixed )
			operator++();
	}
}

binary_tree::const_iterator::const_iterator( const node* v, const node* n ) {
	val = v;
	next = n;
}

binary_tree::const_iterator& binary_tree::const_iterator::operator++() {
	if( loc.empty() and next == nullptr )
		val = nullptr;
	while( not loc.empty() or next != nullptr ) {
		if( next != nullptr ) {
			loc.push( next );
			next = next->child[0];
		} else {
			next = loc.top();
			loc.pop();
			val = next;
			next = next->child[1];
			break;
		}
	}
	return *this;
}

binary_tree::const_iterator binary_tree::const_iterator::operator++(int) {
	auto old = binary_tree::const_iterator( *this );
	operator++();
	return old;
}

binary_tree::const_iterator::reference binary_tree::const_iterator::operator*() const {
	return *val;
}

binary_tree::const_iterator::pointer binary_tree::const_iterator::operator->() const {
	return val;
}

bool binary_tree::const_iterator::operator==( const const_iterator& other ) const {
	return val == other.val;
}

bool binary_tree::const_iterator::operator!=( const const_iterator& other ) const {
	return not operator==( other );
}

binary_tree::const_iterator binary_tree::cbegin() const {
	return const_iterator( root.get(), false );
}

binary_tree::const_iterator binary_tree::cend() {
	return const_iterator();
}

binary_tree::const_iterator binary_tree::crootitr() const {
	return const_iterator( root.get(), root ? root->child[1] : nullptr );
}

binary_tree::const_iterator::operator const node*() const {
	return val;
}

// ******************
// I/O
// ******************

binary_tree::node* binary_tree::node::scan( istream& is ) {
	is >> ws;
	char c = char( is.peek() );
	if( c == '(' ) {
		is.get();
		node* a = scan( is );
		is >> ws;
		int op = is.get();
		node* b = scan( is );
		is >> ws;
		assert( is.get() == ')' );
		return new node( a, b, op );
	} else {
		int id;
		is >> id;
		is >> ws;
		return new node( id );
	}
}

void binary_tree::node::print( ostream& os ) const {
	if( not child[0] and not child[1] )
		os << id;
	else {
		os << "(";
		if( not child[0] )
			os << ERROR_SYMBOL; // error symbol
		else 
			child[0]->print( os );
		os << char(id);
		if( not child[1] )
			os << ERROR_SYMBOL;
		else 
			child[1]->print( os );
		os << ")";
	}
}

void binary_tree::scan( istream& is ) {
	root.reset( node::scan( is ) );
}

void binary_tree::print( ostream& os ) const {
	if( root )
		root->print( os );
	else
		os << ERROR_SYMBOL;
}

istream& operator>>( istream& is, binary_tree& bt ) {
	bt.scan( is );
	return is;
}

ostream& operator<<( ostream& os, const binary_tree& bt ) {
	bt.print( os );
	return os;
}

void subtree_equivalence::scan( istream& is ) {
	tree[0].scan( is );
	is >> ws;
	assert( is.get() == '=' );
	tree[1].scan( is );
}

void subtree_equivalence::print( ostream& os ) const {
	os << tree[0] << "=" << tree[1];
}

istream& operator>>( istream& is, subtree_equivalence& be ) {
	be.scan( is );
	return is;
}

ostream& operator<<( ostream& os, const subtree_equivalence& be ) {
	be.print( os );
	return os;
}

// ******************
// Comparison
// ******************

bool binary_tree::node::operator==( const binary_tree::node& other ) const {
	return height == other.height and id == other.id and (
			( child[0] == nullptr and other.child[0] == nullptr ) or 
			( child[0] != nullptr and other.child[0] != nullptr and *child[0] == *other.child[0] )
		) and (
			( child[1] == nullptr and other.child[1] == nullptr ) or 
			( child[1] != nullptr and other.child[1] != nullptr and *child[1] == *other.child[1] )
	);
}

bool binary_tree::node::operator!=( const binary_tree::node& other ) const {
	return not operator==( other );
}

bool binary_tree::node::operator<( const binary_tree::node& other ) const {
	if( height != other.height )
		return height < other.height;
	if( id != other.id )
		return id < other.id;
	if( child[0] and other.child[0] ) {
		if( *child[0] < *other.child[0] )
			return true;
		if( *child[0] == *other.child[0] )
			if( child[1] and other.child[1] )
				return *child[1] < *other.child[1];
	}
	return false;
}

bool binary_tree::node::operator>( const binary_tree::node& other ) const {
	return other.operator<( *this );
}

bool binary_tree::node::operator>=( const binary_tree::node& other ) const {
	return not operator<( other );
}

bool binary_tree::node::operator<=( const binary_tree::node& other ) const {
	return not operator>( other );
}

bool binary_tree::operator==( const binary_tree& other ) const {
	if( root and other.root )
		return *root == *other.root;
	return root == other.root;
}

bool binary_tree::operator!=( const binary_tree& other ) const {
	return not operator==( other );
}

bool binary_tree::operator<( const binary_tree& other ) const {
	if( root and other.root )
		return *root < *other.root;
	return root < other.root;
}

bool binary_tree::operator>( const binary_tree& other ) const {
	return other.operator<( *this );
}

bool binary_tree::operator>=( const binary_tree& other ) const {
	return not operator<( other );
}

bool binary_tree::operator<=( const binary_tree& other ) const {
	return not operator>( other );
}

// ******************
// hashing
// ******************

size_t binary_tree::hash() const {
	if( _hash == 0 and root ) {
		ostringstream ss;
		print( ss );
		_hash = std::hash<std::string>()( ss.str() );
	}
	return _hash;
}

namespace std {
	hash<binary_tree>::result_type hash<binary_tree>::operator()( argument_type const& bt ) const {
		return bt.hash();
	}
}

// ******************
// Constructors
// ******************

binary_tree::node::node() : node(-1) {
}

binary_tree::node::node( int v ) : node( nullptr, nullptr, v ) {
}

binary_tree::node::node( node* a, node* b, int v ) {
	id = v;
	child[0] = a;
	child[1] = b;
	int h1 = child[0] ? child[0]->height : -1;
	int h2 = child[1] ? child[1]->height : -1;
	height = max( h1, h2 )+1;
	size = ( child[0] ? child[0]->size : 0 ) + ( child[1] ? child[1]->size : 0 ) + not ( child[0] || child[1] );
}

binary_tree::node::node( node* a, node* b ) : node( a, b, -2 ) {
}

binary_tree::node::~node() { // test
	if( child[0] )
		delete child[0];
	if( child[1] )
		delete child[1];
}

binary_tree::binary_tree() : binary_tree( nullptr ) {
}

binary_tree::binary_tree( binary_tree::node* r ) { // change
	root.reset( r );
	_hash = 0;
}

binary_tree::binary_tree( string in ) {
	istringstream ss( move( in ) );
	scan( ss );
}

binary_tree::binary_tree( binary_tree::const_iterator itr ) : binary_tree( itr->clone() ) {
}

binary_tree::binary_tree( const binary_tree& other ) {
	*this = other;
}

binary_tree::binary_tree( binary_tree&& other ) {
	*this = std::move( other );
}

binary_tree& binary_tree::operator=( const binary_tree& other ) {
	/*if( root )
		root->cascade();*/
	root.reset( other.root ? other.root->clone() : nullptr );
	_hash = other._hash;
	return *this;
}

binary_tree& binary_tree::operator=( binary_tree&& other ) {
	root = other.root;
	_hash = other._hash;
	other.root.reset();
	other._hash = 0;
	return *this;
}

binary_tree::~binary_tree() {
	if( root ) {
		//root->cascade();
		root.reset();
	}
}

subtree_equivalence::subtree_equivalence( string in ) {
	istringstream ss( move( in ) );
	scan( ss );
}

subtree_equivalence::subtree_equivalence( const subtree_equivalence& other ) {
	first() = other.first();
	second() = other.second();
}

subtree_equivalence::subtree_equivalence( subtree_equivalence&& other ) {
	first() = std::move( other.first() );
	second() = std::move( other.second() );
}