#pragma once
#include <iostream>
#include <stack>
#include <vector>
#include <map>

class binary_tree {
public:
	struct node {
		int id;
		int height;
		int size;
		node* child[2];
		bool leaf() const;
		bool grab( node*, std::map<int,node*>& );
		void print( std::ostream& );
		static node* scan( std::istream& );
		bool operator==( const node& other ) const;
		bool operator<( const node& other ) const;
		node* clone() const;
		node* cascade();
		node* clonesert( node* where, node* what ) const;
		node* subsitute( const std::map<int,node*>& substitution );
		node();
		node( int );
		node( node*, node* );
		node( node*, node*, int );
	};
	class iterator {
		std::stack<node*> loc;
		node* val;
		node* next;
	public:
		typedef node value_type;
        typedef size_t difference_type;
        typedef std::forward_iterator_tag iterator_category;
        typedef value_type* pointer;
        typedef value_type& reference;
		iterator& operator++();
		iterator operator++(int);
		pointer operator->() const;
		reference operator*() const;
		bool operator==( const iterator& ) const;
		bool operator!=( const iterator& ) const;
		iterator( node* );
	};
	class const_iterator {
		std::stack<const node*> loc;
		const node* val;
		const node* next;
	public:
		typedef node value_type;
        typedef size_t difference_type;
        typedef std::forward_iterator_tag iterator_category;
        typedef const value_type* pointer;
        typedef const value_type& reference;
		const_iterator& operator++();
		const_iterator operator++(int);
		pointer operator->() const;
		reference operator*() const;
		bool operator==( const const_iterator& ) const;
		bool operator!=( const const_iterator& ) const;
		const_iterator( const node* );
	};
private:
	static char BINOP_SYMBOL;
	node* root;
	mutable std::size_t _hash;
public:
	size_t size() const;
	size_t height() const;
	size_t hash() const;
	void scan( std::istream& );
	void print( std::ostream& );
	binary_tree clonesert( node*, node* );
	iterator begin();
	iterator end();
	const_iterator cbegin() const;
	const_iterator cend() const;
	bool operator==( const binary_tree& ) const;
	bool operator!=( const binary_tree& ) const;
	bool operator<( const binary_tree& ) const;
	bool operator>( const binary_tree& ) const;
	bool operator<=( const binary_tree& ) const;
	bool operator>=( const binary_tree& ) const;
	binary_tree();
	binary_tree( node* );
};

class subtree_equivalence {
	binary_tree tree[2];
public:
	binary_tree& first();
	binary_tree& second();
	bool apply( int, binary_tree&, binary_tree::iterator, binary_tree& );
	void scan( std::istream& );
	void print( std::ostream& );
};

namespace std {
	template<> struct std::hash<binary_tree> {
		typedef binary_tree argument_type;
		typedef std::size_t result_type;
		result_type operator()( const argument_type& ) const;
	};
}

std::ostream& operator<<( std::ostream& os, binary_tree& bt );
std::istream& operator>>( std::istream& is, binary_tree& bt );

binary_tree::BINOP_SYMBOL = '*';