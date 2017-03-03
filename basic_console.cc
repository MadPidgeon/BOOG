#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <sstream>
#include "bintree.h"
#include "trie.h"

using namespace std;

void error( string errortext ) {
	cout << "{\"type\":\"ERROR\",\"error\":\"" << errortext << "\"}" << endl;
	exit(0); 
}

void timeout() {
	cout << "{\"type\":\"TIMEOUT\"}" << endl;
	exit(0);
}

void succes( const vector<binary_tree>& proof ) {
	cout << "{\"type\":\"SUCCESS\",\"result\":[";
	bool comma = false;
	for( const auto& line : proof ) {
		if( comma )
			cout << ",";
		comma = true;
		cout << "\"" << line << "\"";
	}
	cout << "]}" << endl;
}

int main( int argc, char** argv ) {
	if( argc <= 2 )
		error("Insufficient arguments");
	try {
		string s = argv[1];
		subtree_equivalence equi( s );
		vector<subtree_equivalence> premises;
		for( int i = 2; i < argc; ++i )
			premises.emplace_back( string( argv[i] ) );
		try {
			auto proof = equi.prove( premises );
			proof = mend_proof( premises, proof, equi.side(1) );
			succes( proof );
		} catch( computation_timeout ) {
			timeout();
		}
	} catch( std::runtime_error e ) {
		error( e.what() );
	}
	return 0;
}
