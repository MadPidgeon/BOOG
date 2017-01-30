#pragma once
#include <iostream>
#include <stack>
#include <vector>
#include <map>
#include <memory>
#include <forward_list>
#include <unordered_map>
#include "union_find.h"

#define ERROR_SYMBOL "?"
#define FREE_VARIABLE_OFFSET 26
// #define VERBOSE_PROOF

class binary_tree {
	friend class substitution_rules;
public:
	struct node {
		int id;
		int height;
		int size;
		node* child[2];
		bool leaf() const;
		bool constant() const;
		bool grab( const node*, std::map<int,const node*>& ) const;
		bool operator==( const node& other ) const;
		bool operator!=( const node& other ) const;
		bool operator<( const node& other ) const;
		bool operator>( const node& other ) const;
		bool operator<=( const node& other ) const;
		bool operator>=( const node& other ) const;
		node* clone() const;
		node* cascade();
		node* clonesert( const node* where, node* what, int& ) const;
		node* subsitute( std::map<int,const node*>& substitution ) const;
		void print( std::ostream& ) const;
		static node* scan( std::istream& );
		node();
		node( int );
		node( node*, node* );
		node( node*, node*, int );
		~node(); // test
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
		iterator();
		iterator( pointer );
		iterator( pointer, pointer );
		iterator( const iterator& ) = default;
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
		operator const node*() const;
		bool operator==( const const_iterator& ) const;
		bool operator!=( const const_iterator& ) const;
		const_iterator();
		const_iterator( pointer, bool fixed = true );
		const_iterator( pointer, pointer );
		const_iterator( const const_iterator& ) = default;
	};
private:
	std::shared_ptr<node> root;
	mutable std::size_t _hash;
public:
	size_t size() const;
	size_t height() const;
	size_t hash() const;
	void scan( std::istream& );
	void print( std::ostream& ) const;
	binary_tree clonesert( const node*, node* ) const;
	bool transform( const node*, const binary_tree&, const binary_tree&, binary_tree& ) const;
	iterator rootitr();
	iterator begin();
	const_iterator begin() const;
	iterator end();
	const_iterator end() const;
	const_iterator crootitr() const;
	const_iterator cbegin() const;
	const_iterator cend() const;
	bool operator==( const binary_tree& ) const;
	bool operator!=( const binary_tree& ) const;
	bool operator<( const binary_tree& ) const;
	bool operator>( const binary_tree& ) const;
	bool operator<=( const binary_tree& ) const;
	bool operator>=( const binary_tree& ) const;
	binary_tree();
	binary_tree( const binary_tree& );
	binary_tree( binary_tree&& );
	binary_tree( std::string );
	binary_tree( const_iterator );
	binary_tree& operator=( const binary_tree& );
	binary_tree& operator=( binary_tree&& );
	~binary_tree();
private:
	binary_tree( node* );
};

class subtree_equivalence {
	binary_tree tree[2];
public:
	binary_tree& side( int );
	binary_tree& first();
	binary_tree& second();
	const binary_tree& side( int ) const;
	const binary_tree& first() const;
	const binary_tree& second() const;
	bool apply( int, const binary_tree&, const binary_tree::node*, binary_tree& ) const;
	std::vector<binary_tree> prove( std::vector<subtree_equivalence> ) const;
	void scan( std::istream& );
	void print( std::ostream& ) const;
	subtree_equivalence( std::string );
	subtree_equivalence( const subtree_equivalence& );
	subtree_equivalence( subtree_equivalence&& );
	~subtree_equivalence() = default;
};

namespace std {
	template<> struct hash<binary_tree> {
		typedef binary_tree argument_type;
		typedef std::size_t result_type;
		result_type operator()( const argument_type& ) const;
	};
}

class substitution_rules {
	size_t separator;
	bool non_contradiction;
	std::unordered_map<int,int> symbol_to_index[2];
	std::vector<int> index_to_symbol;
	std::vector<binary_tree> gcst;
	std::vector<std::forward_list<int>> dep_graph;
	std::vector<int> top_sort_mod_eq;
	UF equivalences;
public:
	substitution_rules( const binary_tree& A, const binary_tree& B );
	bool cycle_check( const binary_tree::node* A, const binary_tree::node* B );
	void add_dep( int s, int id, const binary_tree::node* T );
	void add_equivalence( int idA, int idB );
	bool validate_dependency();
	bool parallel_walk( int sa, const binary_tree::node* A, int sb, const binary_tree::node* B );
	bool add_rule( int s, int id, const binary_tree::node* T );
	const binary_tree& at( int s, int id ) const;
	operator bool() const;
};

std::ostream& operator<<( std::ostream& os, const binary_tree& bt );
std::istream& operator>>( std::istream& is, binary_tree& bt );
std::ostream& operator<<( std::ostream& os, const subtree_equivalence& se );
std::istream& operator>>( std::istream& is, subtree_equivalence& se );

int get_free_variable();