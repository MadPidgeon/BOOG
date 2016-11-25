#include <iostream>
#include <stack>
#include <cassert>
#include <cmath>
#include <sstream>
#include <vector>
#include <map>
#include <queue>

using namespace std;


binary_tree::node* binary_tree::node::clone() const {
	return new node( child[0] ? child[0]->clone() : nullptr, child[1] ? child[1]->clone() : nullptr, id );
}

binary_tree::node* binary_tree::node::clonesert( binary_tree::node* where, binary_tree::node* what ) const {
	if( this == where )
		return what;
	return new node( child[0] ? child[0]->clonesert( where, what ) : nullptr, child[1] ? child[1]->clonesert( where, what ) : nullptr, id );
}

binary_tree binary_tree::clonesert( binary_tree::node* where, binary_tree::node* what ) {
	return binary_tree( root->clonesert( where, what ) );
}

bool binary_tree::node::leaf() const {
	return child[0] == nullptr and child[1] == nullptr;
}


binary_tree::node* binary_tree::node::cascade() {
	if( child[0] ) delete child[0]->cascade();
	if( child[1] ) delete child[1]->cascade();
	return this;
}

bool binary_tree::node::grab( node* rule, map<int,node*>& result ) {
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

binary_tree::node* binary_tree::node::subsitute( const map<int,node*>& substitution ) {
	if( substitution.count(id) )
		return substitution.at(id)->clone();
	else
		return new node( child[0] ? child[0]->subsitute( substitution ) : nullptr, child[1] ? child[1]->subsitute( substitution ) : nullptr, id );
}

bool subtree_equivalence::apply( int i, binary_tree& parent, binary_tree::node* n, binary_tree& res ) {
	map<int,binary_tree::node*> substitution;
	if( not n->grab( tree[i].root, substitution ) )
		return false;
	res = parent.clonesert( n, tree[not i].root->subsitute( substitution ) );
	return true;
}


// ******************
// Getters & Setters
// ******************

binary_tree& subtree_equivalence::first() {
	return tree[0];
}

binary_tree& subtree_equivalence::second() {
	return tree[1];
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

binary_tree::iterator::iterator( node* n ) {
	val = nullptr;
	if( n ) {
		next = n;
		operator++();
	}
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
	return iterator( root );
}

binary_tree::iterator binary_tree::end() {
	return iterator( nullptr );
}

binary_tree::const_iterator::const_iterator( const node* n ) {
	val = nullptr;
	if( n ) {
		next = n;
		operator++();
	}
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
	auto old = binary_tree::iterator( *this );
	operator++();
	return old;
}

binary_tree::iterator::reference binary_tree::const_iterator::operator*() const {
	return *val;
}

binary_tree::const_iterator::pointer binary_tree::const_iterator::operator->() const {
	return val;
}

bool binary_tree::const_iterator::operator==( const const_iterator& other ) const {
	return val == other.val;
}

bool binary_tree::const_iterator::operator!=( const iterator& other ) const {
	return not operator==( other );
}

binary_tree::const_iterator binary_tree::cbegin() {
	return const_iterator( root );
}

binary_tree::const_iterator binary_tree::cend() {
	return const_iterator( nullptr );
}

// ******************
// I/O
// ******************

binary_tree::node* binary_tree::node::scan( istream& is ) {
	is >> ws;
	char c = is.peek();
	if( c == '(' ) {
		is.get();
		node* a = scan( is );
		is >> ws;
		assert( is.get() == BINOP_SYMBOL );
		node* b = scan( is );
		is >> ws;
		assert( is.get() == ')' );
		return new node( a, b );
	} else {
		int id;
		is >> id;
		is >> ws;
		return new node( id );
	}
}

void binary_tree::node::print( ostream& os ) {
	if( not child[0] and not child[1] )
		os << id;
	else {
		os << "(";
		if( not child[0] )
			os << "?"; // error symbol
		else 
			child[0]->print( os );
		os << BINOP_SYMBOL;
		if( not child[1] )
			os << "?";
		else 
			child[1]->print( os );
		os << ")";
	}
}

void binary_tree::scan( istream& is ) {
	root = node::scan( is );
}

void binary_tree::print( ostream& os ) {
	if( root )
		root->print( os );
	else
		os << "?"; // error symbol
}

istream& operator>>( istream& is, binary_tree& bt ) {
	bt.scan( is );
	return is;
}

ostream& operator<<( ostream& os, binary_tree& bt ) {
	bt.print( os );
	return os;
}

void subtree_equivalence::scan( istream& is ) {
	tree[0].scan( is );
	is >> ws;
	assert( is.get() == '=' );
	tree[1].scan( is );
}

void subtree_equivalence::print( ostream& os ) {
	os << tree[0] << "=" << tree[1];
}

istream& operator>>( istream& is, subtree_equivalence& be ) {
	be.scan( is );
	return is;
}

ostream& operator<<( ostream& os, subtree_equivalence& be ) {
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
	if( not( root or other.root ) )
		return root == other.root;
	return *root == *other.root;
}

bool binary_tree::operator!=( const binary_tree& other ) const {
	return not operator==( other );
}

bool binary_tree::operator<( const binary_tree& other ) const {
	if( not( root or other.root ) )
		return root < other.root;
	return *root < *other.root;
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

class sizecomp {
public:
	bool operator()( const binary_tree& a, const binary_tree& b ) {
		return a.size() > b.size() or ( a.size() == b.size() and a.height() > b.height() );
	}
};

// ******************
// hashing
// ******************

size_t binary_tree::hash() const {
	if( _hash == 0 and root ) {
		ostringstream ss;
		print( ss );
		_hash = std::hash<std::string>( ss.str() );
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

binary_tree::binary_tree() : binary_tree( nullptr ) {
}

binary_tree::binary_tree( binary_tree::node* r ) {
	root = r;
	_hash = 0;
}