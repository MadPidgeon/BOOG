#include <iostream>
#include <stack>
#include <cassert>
#include <cmath>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <queue>
#include "bintree.h"
#include "top_sort.h"


// #define DEBUG

using namespace std;

// ******************
// Bullshit
// ******************
 
substitution_rules::substitution_rules( const binary_tree& A, const binary_tree& B ) {
	for( auto& X : A ) {
		if( X.leaf() and ( not X.constant() ) and symbol_to_index[0].count( X.id ) == 0 ) {
			symbol_to_index[0][X.id] = int( index_to_symbol.size() );
			index_to_symbol.push_back( X.id );
		}
	}
	separator = index_to_symbol.size();
	for( const auto& X : B ) {
		if( X.leaf() and ( not X.constant() ) and symbol_to_index[1].count( X.id ) == 0 ) {
			symbol_to_index[1][X.id] = int( index_to_symbol.size() );
			index_to_symbol.push_back( X.id );
		}
	}
	cout << "Symbols(" << index_to_symbol.size() << "): ";
	for( auto& X : index_to_symbol )
		cout << X << " ";
	cout << endl;

	dep_graph.resize( index_to_symbol.size() );
	equivalences.clear( index_to_symbol.size() );
	gcst.resize( index_to_symbol.size() );
	non_contradiction = true;
	if( separator != 0 and separator != index_to_symbol.size() ) {
		if( not cycle_check( &*A.crootitr(), &*B.crootitr() ) ) {
			cerr << "Cycle check contradiction" << endl;
			return;
		}
		if( not validate_dependency() ) {
			cerr << "Validate dependency contradiction" << endl;
			return;
		}
	} else {
		cerr << "Nothing to verify" << endl;
	}
	cerr << "No cycles detected" << endl;
	if( not parallel_walk( 0, &*A.crootitr(), 1, &*B.crootitr() ) ) {
		cerr << "Substitution failed!" << endl;
		return;
	}
	for( int x = 0; x < index_to_symbol.size(); ++x ) {
		if( equivalences.find(x) == x and gcst[x].size() == 0 ) {
			gcst[x] = binary_tree( new binary_tree::node( get_free_variable() ) );
		}
	}

}

const binary_tree& substitution_rules::at( int s, int id ) const {
	return gcst.at(symbol_to_index[s].at(id));
}

substitution_rules::operator bool() const {
	return non_contradiction;
}

// A and B are not interchangable
bool substitution_rules::cycle_check( const binary_tree::node* A, const binary_tree::node* B ) {
	cout << A->id << "," << B->id << endl;
	if( A->leaf() ) {
		if( B->leaf() ) {
			if( A->constant() ) {
				if( B->constant() ) {
					if( A->id != B->id ) {
						return non_contradiction = false;
					}
				} else {
					add_dep( 1, B->id, A );
				}
			} else {
				if( B->constant() ) {
					add_dep( 0, A->id, B );
				} else {
					add_equivalence( A->id, B->id );
				}
			}
		} else {
			if( A->constant() ) {
				return non_contradiction = false;
			} else {
				add_dep( 0, A->id, B );
			}
		}
	} else {
		cout << B->leaf() << endl;
		if( B->leaf() ) {
			if( B->constant() ) {
				return non_contradiction = false;
			} else {
				add_dep( 1, B->id, A );
			}
		} else {
			if( A->id == B->id ) {
				return cycle_check( A->child[0], B->child[0] ) && cycle_check( A->child[1], B->child[1] );
			} else {
				return non_contradiction = false;
			}
		}
	}
	// cout << "DONE~" <<endl;
	return true;
}

bool substitution_rules::validate_dependency() {
	for( size_t x = 0; x < equivalences.size(); ++x ) {
		size_t y = equivalences.find( int(x) );
		if( x != y )
			dep_graph[y].splice_after( dep_graph[y].before_begin(), dep_graph[x] );
	}
	top_sort_mod_eq = topological_sort( dep_graph );
	return non_contradiction = ( top_sort_mod_eq.size() != 0 );
}

void substitution_rules::add_equivalence( int idA, int idB ) {
	if( equivalences.cup( symbol_to_index[0][idA], symbol_to_index[1][idB] ) ) { // new equivalence
		return;//
	}
}

void substitution_rules::add_dep( int s, int id, const binary_tree::node* T ) {
	cout << "Add dep:";
	T->print(cout);
	cout << endl; 
	if( T->leaf() ) {
		if( not T->constant() )
			dep_graph[symbol_to_index[s][id]].push_front( symbol_to_index[not s][T->id] );
	} else {
		add_dep( s, id, T->child[0] );
		add_dep( s, id, T->child[1] );
	}
}

// A and B are iterchangable
bool substitution_rules::parallel_walk( int sa, const binary_tree::node* A, int sb, const binary_tree::node* B ) {
	cout << A->id << "," << B->id << endl;
	if( A->leaf() ) {
		if( B->leaf() ) {
			if( A->constant() ) {
				if( B->constant() ) {
					if( A->id != B->id ) {
						return non_contradiction = false;
					}
				} else {
					return add_rule( sb, B->id, A );
				}
			} else {
				if( B->constant() ) {
					return add_rule( sa, A->id, B );
				} else {
					// return add_equivalence( A->id, B->id );
				}
			}
		} else {
			if( A->constant() ) {
				return non_contradiction = false;
			} else {
				return add_rule( sa, A->id, B );
			}
		}
	} else {
		if( B->leaf() ) {
			if( B->constant() ) {
				return non_contradiction = false;
			} else {
				return add_rule( sb, B->id, A );
			}
		} else {
			if( A->id == B->id ) {
				return parallel_walk( sa, A->child[0], sb, B->child[0] ) && parallel_walk( sa, A->child[1], sb, B->child[1] );
			} else {
				return non_contradiction = false;
			}
		}
	}
	return true;
}

bool substitution_rules::add_rule( int s, int id, const binary_tree::node* T ) {
	cout << "Add rule (" << s << "," << id << "): ";
	T->print( cout );
	cout << endl;
	cout << "GCST: ";
	for( int i = 0; i < index_to_symbol.size(); ++i )
		cout << gcst[i] << " ";
	cout << endl;
	cout << "Or:" << symbol_to_index[s][id] << endl;
	size_t x = equivalences.find( symbol_to_index[s][id] );
	cout << "Eq:" << x << endl;
	if( gcst[x].size() == 0 ) {
		gcst[x] = binary_tree( T->clone() );
		return true;
	} else {
		non_contradiction = parallel_walk( s, T, x >= separator, &*gcst[x].crootitr() );
		if( not non_contradiction )
			cout << "WATAW" << endl;
		return non_contradiction;
	}
}

// ******************
// Magic
// ******************

int get_free_variable() {
	static int F = FREE_VARIABLE_OFFSET;
	return F++;
}

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
		#ifdef DEBUG
		cout << "Tree: " << X << endl;
		#endif

		for( const auto& axiom : axioms ) {
			#ifdef DEBUG
			cout << "Axiom: " << axiom << endl;
			#endif

			for( const auto& N : X ) {
				#ifdef DEBUG
				cout << "Sub: "; N.print( cout ); cout << endl;
				#endif

				for( int i = 0; i < 2; ++i ) {
					bool res = axiom.apply( i, X, &N, Y );
					if( res and master.count( Y ) == 0 ) {
						#ifdef DEBUG
						cout << "Res: " << Y << endl;
						#endif

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
		#ifdef DEBUG
		cin.get();
		#endif
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

bool binary_tree::node::constant() const {
	return leaf() and id < 0; // leaf perhaps not required
}

/*binary_tree::node* binary_tree::node::cascade() {
	if( child[0] ) delete child[0]->cascade();
	if( child[1] ) delete child[1]->cascade();
	return this;
}*/

bool binary_tree::node::grab( const binary_tree::node* rule, map<int,const binary_tree::node*>& result ) const {
	if( leaf() and not rule->leaf() ) {
		return false;
	}

	//if( rule->height > height ) // rephrase
	//	return false;
	if( rule->child[0] and child[0] and not child[0]->grab( rule->child[0], result ) )
		return false;
	if( rule->child[1] and child[1] and not child[1]->grab( rule->child[1], result ) )
		return false;
	if( rule->leaf() ) {
		if( rule->id < 0 ) { // constant
			if( leaf() ) {
				if( rule->id == id )
					return true;
				// TODO: force (this) variable to constant
			}
			return false;
		} else { // variable
			if( result.count( rule->id ) )
				return *result[rule->id] == *this;
			result[rule->id] = this;
		}
	}
	return true;
}

//***** MERGING *****//
//    #      #   ->  #
//  a  t0  t1 b    a  b
//*******************//
/*
binary_tree::node* binary_tree::node::join( binary_tree::node* other, int sub_offset ) {
	node *a, *b;
	if( leaf() == other->leaf() ) {
		if( id == other->id ) {
			if( leaf() )
				return new node( id );
			a = child[0]->join( other->child[0] );
			b = child[1]->join( other->child[1] );
			if( a == nullptr or b == nullptr )
				return nullptr;
			return new node( id, a, b );
		}
		return false;
	}
	if( leaf() )
		a = this, b = other;
	else
		a = other, b = this;
	if( a->id >= sub_offset ) {

	}
}



bool binary_tree::node::grab( const binary_tree::node* rule, map<int,const binary_tree::node*>& result ) const {
	if( rule->leaf() ) {
		if( rule->id < 0 ) { // constant
			if( not leaf() )
				return false;
			if( rule->id != id ) {
				if( id >= FREE_VARIABLE_OFFSET ) {
					if( result.count( id ) ) 
						return result[id]->merge( rule );
					result[id] = rule;
				}
			}
		}
	}

	return true;

	if( leaf() ) {
		if( rule->leaf() ) {
			if( id >= FREE_VARIABLE_OFFSET ) { // free variable
				cerr << "FREE VARIABLE PANIC!!" << endl;
				throw;
			} else
				return 0;
		}
	} else {

	}

	//if( rule->height > height ) // rephrase
	//	return false;
	if( rule->child[0] and child[0] and not child[0]->grab( rule->child[0], result ) )
		return false;
	if( rule->child[1] and child[1] and not child[1]->grab( rule->child[1], result ) )
		return false;
	if( rule->leaf() ) {
		if( rule->id < 0 ) { // constant
			if( leaf() ) {
				if( rule->id == id )
					return true;
				// TODO: force (this) variable to constant
			}
			return false;
		} else { // variable
			if( result.count( rule->id ) )
				return *result[rule->id] == *this;
			result[rule->id] = this;
		}
	}
	return true;
}*/

binary_tree::node* binary_tree::node::subsitute( map<int,const binary_tree::node*>& substitution ) const {
	if( leaf() ) {
		if( id < 0 ) // constant
			return new node( id );
		else if( substitution.count(id) ) // bound variable 
			return substitution.at(id)->clone();
		else { // free variable
			substitution[id] = new node( get_free_variable() );
			return substitution[id]->clone();
		}
	} else
		return new node( child[0]->subsitute( substitution ), child[1]->subsitute( substitution ), id );
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

binary_tree::const_iterator binary_tree::begin() const{
	return const_iterator( root.get(), false );
}


binary_tree::iterator binary_tree::end() {
	return iterator();
}

binary_tree::const_iterator binary_tree::end() const {
	return const_iterator();
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

binary_tree::const_iterator binary_tree::cend() const {
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
		assert( ispunct( op ) and op != '(' and op != ')' );
		node* b = scan( is );
		is >> ws;
		assert( is.get() == ')' );
		return new node( a, b, op );
	} else if( isalpha( c ) ) {
		is.get();
		is >> ws;
		return new node( tolower( c ) - 'a' );
	} else if( isdigit( c ) ) {
		int id;
		is >> id;
		is >> ws;
		return new node( -id-1 );
	} else {
		throw;
	}
}

void binary_tree::node::print( ostream& os ) const {
	if( not child[0] and not child[1] ) {
		if( id < 0 )
			os << (-id-1);
		else if( id < 26 )
			os << char( 'a' + id );
		else
			os << "t" << (id-26);
	} else {
		os << "(";
		if( not child[0] )
			os << ERROR_SYMBOL;
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