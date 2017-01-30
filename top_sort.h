#pragma once
#include <stack>
#include <vector>
#include <iterator>
#include <forward_list>

template<typename T>
std::vector<T> topological_sort( const std::vector<std::forward_list<T>>& graph ) {
	size_t n = graph.size();
	std::stack<size_t> zero_degree_nodes;
	std::vector<size_t> indegree( n, 0 );
	std::vector<T> order;
	order.reserve( n );
	for( const auto& L : graph )
		for( T x : L )
			++indegree[x];
	for( size_t i = 0; i < n; ++i )
		if( indegree[i] == 0 )
			zero_degree_nodes.push( i );
	while( not zero_degree_nodes.empty() ) {
		size_t x = zero_degree_nodes.top();
		zero_degree_nodes.pop();
		order.push_back( static_cast<T>(x) );
		for( size_t y : graph[x] )
			if( --indegree[y] == 0 )
				zero_degree_nodes.push(y);
	}
	if( order.size() == n )
		return order;
	return {};
}  