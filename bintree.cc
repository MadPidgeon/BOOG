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

binary_tree::node* substitution_rules::intersect( const binary_tree::node* A, const binary_tree::node* B ) {
	// cout << "Intersect: " << A->id << " " << B->id << endl; 
	// ---------
	// both constant
	if( A->constant() and B->constant() ) { 
		if( A->id == B->id )
			return A->clone();
		return nullptr;
	}
	// either a variable
	if( B->leaf() and not B->constant() )
		swap( A, B );
	if( A->leaf() and not A->constant() ) {
		if( B->leaf() and not B->constant() ) { // both variables
			if( not add_equivalence( A->id, B->id ) )
				return nullptr;
		} else if( not add_rule( A->id, B ) )
			return nullptr;
		return A->clone();
	}
	// constant/operator conflict
	if( ( B->constant() and not A->leaf() ) or ( A->constant() and not B->leaf() ) ) 
		return nullptr;
	// both are operators
	if( A->id != B->id )
		return nullptr;
	binary_tree::node* l = intersect( A->child[0], B->child[0] );
	if( l == nullptr )
		return nullptr;
	binary_tree::node* r = intersect( A->child[1], B->child[1] );
	if( r == nullptr ) {
		delete l;
		return nullptr;
	}
	return new binary_tree::node( l, r, A->id );
}

bool substitution_rules::add_equivalence( int as, int bs ) {
	// cout << "Add equivalence: " << char(as+'a'/*fix*/) << " = " << char(bs+'a') << endl;
	// ---------------
	int ai = equivalences.find( symbol_to_index[as] );
	int bi = equivalences.find( symbol_to_index[bs] );
	if( busy[ai] or busy[bi] )
		return false;
	if( equivalences.cup( ai, bi ) ) { // change occured
		if( gcst[ai].size() != 0 or gcst[bi].size() != 0 ) { // not both are empty
			if( gcst[ai].size() == 0 )
				swap( ai, bi );
			if( gcst[bi].size() == 0 and equivalences[ai] != ai ) // A is non-empty, B is empty and A moves to B
				swap( gcst[ai], gcst[bi] );
			else if( gcst[bi].size() != 0 ) { // A is non-empty, B is non-empty
				if( equivalences[ai] != ai )
					swap( ai, bi );
				busy[bi] = busy[ai] = true;
				gcst[ai] = binary_tree( intersect( &*gcst[ai].crootitr(), &*gcst[bi].crootitr() ) );
				gcst[bi] = binary_tree( nullptr );
				busy[ai] = false;
			}
		}
	}
	return true;
}

bool substitution_rules::add_rule( int symbol, const binary_tree::node* T ) {
	/*cout << "Add rule: " << char(symbol+'a') << " => ";
	T->print(cout);
	cout << endl;*/
	// ----------
	int index = equivalences[ symbol_to_index[symbol] ];
	if( gcst[index].size() == 0 ) {
		gcst[index] = binary_tree( T->clone() );
		return true;
	}
	if( busy[index] )
		return false;
	busy[index] = true;
	gcst[index] = binary_tree( intersect( &*gcst[index].crootitr(), T ) );
	busy[index] = false;
	return gcst[index] != nullptr;
}

substitution_rules::substitution_rules( const binary_tree::node* A, const binary_tree::node* B ) {
	// set up translation table
	for( const auto& Y : { A, B } ) {
		for( binary_tree::const_iterator X( Y, false ); X != binary_tree::const_iterator( nullptr ); ++X ) {
			if( X->leaf() and ( not X->constant() ) and symbol_to_index.count( X->id ) == 0 ) {
				symbol_to_index[X->id] = int( index_to_symbol.size() );
				index_to_symbol.push_back( X->id );
			}
		}
	}
	size_t n = index_to_symbol.size();
	/*cout << "Symbols(" << n << "): ";
	for( auto& X : index_to_symbol )
		cout << X << " ";
	cout << endl;*/
	// set up greatest common super trees
	gcst.resize( n );
	equivalences.clear( int(n) );
	busy.resize( n, false );
	// intersect A and B
	binary_tree r( intersect( A, B ) );
	non_contradiction = ( r.size() != 0 );
	/*for( int x = 0; x < n; ++x ) {
		cout << char(index_to_symbol[x]+'a') << " = " << gcst[x] << endl;
	}*/
	if( non_contradiction and n > 0) {
		// validate dependencies
		dep_graph.resize( n );
		for( size_t i = 0; i < n; ++i ) {
			if( equivalences[i] != int(i) )
				dep_graph[i].push_front( equivalences[i] );
			else if( gcst[i].size() != 0 ) {
				add_dep( i, &*gcst[i].crootitr() );
			}
		}
		//cout << "TOPSORTING " << n << endl;
		top_sort = topological_sort( dep_graph );
		/*for( int x : top_sort )
			cout << x << " ";
		cout << endl;*/
		non_contradiction = (top_sort.size() > 0);
		/*cout << "Order: ";
		for( int x : top_sort )
			cout << char(x+'a') << " ";
		cout << endl;*/
	}
	// cout << "Result: " << r << endl;
}

void substitution_rules::add_dep( int index, const binary_tree::node* T ) {
	if( T->leaf() ) {
		if( not T->constant() ) {
			//cout << "Add dependency: " << char(index_to_symbol[index]+'a') << " ~> " << char(T->id+'a') << endl;
			dep_graph[index].push_front( symbol_to_index[T->id] );
		}
	} else {
		add_dep( index, T->child[0] );
		add_dep( index, T->child[1] );
	}
}

const binary_tree& substitution_rules::at( int symbol ) const {
	return gcst.at(symbol_to_index.at(symbol));
}

substitution_rules::operator bool() const {
	return non_contradiction;
}

binary_tree::node* substitution_rules::apply( const binary_tree::node* T ) {
	if( not T->leaf() )
		return new binary_tree::node( apply( T->child[0] ), apply( T->child[1] ), T->id );
	if( T->constant() )
		return new binary_tree::node( T->id );
	if( symbol_to_index.count( T->id ) == 0 )
		symbol_to_index[T->id] = -get_free_variable();
	int y = symbol_to_index[T->id];
	if( y < 0 )
		return new binary_tree::node( -y );
	int x = equivalences.find( y );
	if( gcst[x].size() == 0 )
		return new binary_tree::node( index_to_symbol[x] );
	return apply( &*gcst[x].crootitr() );
}

binary_tree::node* substitution_rules::apply( const binary_tree& T ) {
	return apply( &*T.crootitr() );
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
	unordered_map<binary_tree,binary_tree,std::hash<binary_tree>,binary_tree::comparator> master;
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
		if( rule->id < BOUND_VARIABLE_OFFSET ) { // constant
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
		if( id < BOUND_VARIABLE_OFFSET ) // constant
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
	// return parent.transform( itr, side(i), side(not i), res );
	substitution_rules R( &*side(i).crootitr(), itr );
	if( not R )
		return false;
	res = std::move( parent.clonesert( itr, R.apply( side(not i) ) ) );
	return true;
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
		if( islower( c ) )
			return new node( c - 'a' + BOUND_VARIABLE_OFFSET );
		else
			return new node( c - 'A' + AXIOMATIC_VARIABLE_OFFSET );
	} else if( isdigit( c ) ) {
		int id;
		is >> id;
		is >> ws;
		return new node( BOUND_VARIABLE_OFFSET-id-1 );
	} else {
		throw;
	}
}

void binary_tree::node::print( ostream& os ) const {
	if( not child[0] and not child[1] ) {
		if( id < BOUND_VARIABLE_OFFSET )
			os << ( BOUND_VARIABLE_OFFSET - id - 1 );
		else if( id < AXIOMATIC_VARIABLE_OFFSET )
			os << char( 'a' + id - BOUND_VARIABLE_OFFSET );
		else if( id < FREE_VARIABLE_OFFSET ) 
			os << char( 'A' + id - AXIOMATIC_VARIABLE_OFFSET );
		else
			os << "t" << ( id-FREE_VARIABLE_OFFSET );
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

void binary_tree::node::hash_print( std::ostream& os, std::unordered_map<int,int>& table ) const {
	if( leaf() ) {
		if( constant() ) {
			if( id >= BOUND_VARIABLE_OFFSET )
				os << char( id - BOUND_VARIABLE_OFFSET + 'a' );
			else
				os << ( id - BOUND_VARIABLE_OFFSET + 1 );
		}
		else {
			if( table.count( id ) == 0 )
				table[id] = table.size();
			os << table[id];
		}
	} else {
		os << "(";
		child[0]->hash_print( os, table );
		os << char(id);
		child[1]->hash_print( os, table );
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

void binary_tree::hash_print( ostream& os ) const {
	if( root ) {
		std::unordered_map<int,int> table;
		root->hash_print( os, table );
	} else
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

bool binary_tree::comparator::operator()( const binary_tree& A, const binary_tree& B ) const {
	if( A.root and B.root ) {
		std::unordered_map<int,int> U,V;
		return cmp( A.root.get(), B.root.get(), U, V );
	}
	return A.root == B.root;
}

bool binary_tree::comparator::cmp( const binary_tree::node* A, const binary_tree::node* B, std::unordered_map<int,int>& AtoB, std::unordered_map<int,int>& BtoA ) const {
	if( A->leaf() != B->leaf() )
		return false;
	if( A->leaf() ) {
		if( A->constant() != B->constant() )
			return false;
		if( A->constant() )
			return A->id == B->id;
		if( AtoB.count( A->id ) == 0 )
			AtoB[A->id] = B->id;
		if( BtoA.count( B->id ) == 0 )
			BtoA[B->id] = A->id;
		return AtoB[A->id] == B->id and BtoA[B->id] == A->id;
	} 
	return cmp( A->child[0], B->child[0], AtoB, BtoA ) and cmp( A->child[1], B->child[1], AtoB, BtoA );
}

// ******************
// hashing
// ******************

size_t binary_tree::hash() const {
	if( _hash == 0 and root ) {
		ostringstream ss;
		hash_print( ss );
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
	if( root )
		root.reset();
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