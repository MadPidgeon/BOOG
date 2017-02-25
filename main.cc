#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <string>
#include <iomanip>
#include <limits>
#include <sstream>
#include <dirent.h>
#include "bintree.h"
#include "trie.h"

#define ALWAYS_SHOW_PROOF

using namespace std;

vector<string> getdir( string dir  ) {
	DIR *dp;
	vector<string> f;
	struct dirent *dirp;
	const string ext = ".txt";
	if( (dp  = opendir( dir.c_str() ) ) == NULL)
		throw runtime_error("Could not open directory \"" + dir +"\"!" );
	while( (dirp = readdir( dp ) ) != NULL ) {
		string s = dirp->d_name;
		if( s.size() > 4 and equal( s.end()-4, s.end(), ext.begin() ) ) {
			s.resize( s.size()-4 );
			f.push_back( s );
		}
	}
	closedir(dp);
	return f;
}

void proof_print( const std::vector<binary_tree>& proof ) {
	if( proof.size() > 0 ) {
		cout << proof.back() << endl;
		for( auto itr = proof.rbegin()+1; itr != proof.rend(); ++itr )
			cout << "=" << (*itr) << endl;
	} else
		cout << "[No proof available]" << endl;
}


bool run_environment( string filename, vector<subtree_equivalence> premises ) {
	size_t original_premise_count = premises.size();
	vector<binary_tree> proof;
	subtree_equivalence input;
	ifstream env( "proof_environment/" + filename + ".txt" );
	if( not env ) {
		cout << "Error opening file \"" << filename << "\"!" << endl;
		return 1;
	}
	cout << endl << "The following properties will be validated:" << endl;
	while( not env.eof() ) {
		env >> ws;
		if( env.peek() == '%' ) {
			env.get();
			env.ignore( numeric_limits<streamsize>::max(), '\n' );
			continue;
		}
		if( env.peek() == '!' ) {
			env.get();
			if( env.eof() ) {
				cout << "Unexpected end of file!" << endl;
				return 1;
			}
			if( env.peek() == '~' ) {
				env.get();
				env >> ws;
				premises.resize( original_premise_count );
				continue;
			}
			env >> input >> ws;
			premises.push_back( input );
			continue;
		}
		env >> input >> ws;
		for( size_t i = original_premise_count; i < premises.size()-1; ++i )
			cout << premises.at(i) << ", ";
		if( original_premise_count < premises.size() )
			cout << premises.back() << " => ";
		cout << input << ": " << flush;
		try {
			proof = input.prove( premises );
		} catch( ... ) {
			cout << "Timeout" << endl;
			continue;
		}				
		if( proof.size() == 0 )
			cout << "Does not follow" << endl;
		else {
			#ifdef ALWAYS_SHOW_PROOF
			proof_print( proof );
			proof_print( mend_proof( premises, proof ) );
			#else
			cout << "True" << endl;
			#endif
		}
	}
	return 0;
}

int main() {
	// usage
	cout << "Please input the variables in axioms.txt as capital letters." << endl;
	cout << "You may use the characters 0,...,9 for constants." << endl;
	cout << "The characters for which ispunct returns true and that are not parenthesis may be used as operators." << endl;
	cout << "These operators are considered distinct, and multiple may be used in a single axiom." << endl;
	cout << "Each operator requires a corresponding pair of parenthesis." << endl << endl;

	// read data
	std::map<std::string,subtree_equivalence> axioms;
	std::string s, t;
	trie axiom_names;
	ifstream in( "axioms.txt" );
	if( not in ) {
		cout << "Could not open axioms.txt!" << endl;
		return 1;
	}
	cout << "Known axioms:" << endl;
	while( not in.eof() ) {
		in >> s >> ws;
		if( s[0] == '%' ) {
			in.ignore(numeric_limits<streamsize>::max(),'\n');
			continue;
		}
		axiom_names.insert( s );
		in >> axioms[s] >> ws;
	}
	cout << left;
	for( const auto& p : axioms )
		cout << setw(10) << axiom_names.compress(p.first) << " " << setw(28) << p.first << " " << p.second << endl;
	cout << endl;

	// pick axioms
	std::vector<subtree_equivalence> premises;
	cout << "Enter the abbreviation of the axioms you want to use, separated by a newline." << endl;
	cout << "Enter an empty line to continue." << endl;
	while( true ) {
		cout << ">";
		getline( cin, s );
		if( s.empty() )
			break;
		if( s.find_first_of('=') == std::string::npos ) {
			t = axiom_names.expand( s );
			if( t == "" or axioms.count( t ) == 0 ) {
				cout << "Unknown axiom \"" << s << "\"!" << endl;
				continue;
			}
			premises.push_back( axioms[t] );
		} else {
			try {
				istringstream ss( s );
				subtree_equivalence axiom;
				ss >> axiom;
				premises.push_back( std::move( axiom ) );
			} catch( const std::exception& e ) {
				cout << "Could not parse equivalence statement. Did you forget some brackets?" << endl;
				cerr << "ERROR: " << e.what() << endl;
				continue;
			}
		}
	}
	cout << endl;

	// environment
	subtree_equivalence input;
	trie environment_names;
	cout << "Enter one of the following proof packages or press enter to continue:" << endl;
	auto environments = getdir( "proof_environment" );
	for( const auto& s : environments )
		environment_names.insert( s );
	for( const auto& s : environments )
		cout << setw(10) << environment_names.compress( s ) << " " << s << endl;
	while( true ) {
		cout << ">";
		getline( cin, s );
		if( s.empty() )
			break;
		t = environment_names.expand( s );
		if( t != "" )
			return run_environment( t, premises );
		cout << "Unknown environment \"" << s << "\"" << endl;
	}

	// enter provable statement
	cout << endl;
	cout << "Now enter the statement you want to prove." << endl;
	cout << "Use lower case characters for variables." << endl << ">";
	cin >> input;
	cout << endl;

	// compute
	cout << "The program is now trying to prove the statement." << endl;
	cout << "Termination of the program is not guaranteed." << endl;
	cout << "Please close all your active programs, as this program requires a lot of RAM." << endl;
	cout << "You can terminate the computation by pressing CTRL+C." << endl << endl;
	auto proof = input.prove( premises );
	
	// proof
	cout << "Done!" << endl;
	if( proof.size() == 0 ) {
		cout << "The statement cannot be proven from the axioms." << endl;
	} else {
		cout << "The proof is as follows: " << endl;
		proof_print( proof );
	}

	return 0;
}