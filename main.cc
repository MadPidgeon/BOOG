#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <string>
#include <iomanip>
#include "bintree.h"

using namespace std;

int main() {
	// usage
	cout << "Please input the variables in axioms.txt as capital letters." << endl;
	cout << "You may use the characters 0,...,9 for constants." << endl;
	cout << "The characters for which ispunct returns true and that are not parenthesis may be used as operators." << endl;
	cout << "These operators are considered distinct, and multiple may be used in a single axiom." << endl;
	cout << "Each operator requires a corresponding pair of parenthesis." << endl << endl;

	// read data
	std::unordered_map<std::string,subtree_equivalence> axioms;
	std::string s;
	ifstream in( "axioms.txt" );
	cout << "Known axioms:" << endl;
	while( not in.eof() ) {
		in >> s >> ws;
		in >> axioms[s] >> ws;
		cout << setw(28) << left << s << " " << axioms[s] << endl;
	}
	cout << endl;

	// pick axioms
	std::vector<subtree_equivalence> premises;
	cout << "Enter the names of the axioms you want to use, separated by a newline." << endl;
	cout << "Enter an empty line to continue." << endl;
	while( true ) {
		cout << ">";
		getline( cin, s );
		if( s.empty() )
			break;
		if( axioms.count( s ) == 0 ) {
			cout << "Unknown axiom \"" << s << "\"!" << endl;
			continue;
		}
		premises.push_back( axioms[s] );
	}
	cout << endl;

	// enter provable statement
	subtree_equivalence input;
	cout << "Now enter the statement you want to prove." << endl;
	cout << "Use lower case characters for variables." << endl << ">";
	cin >> input;
	cout << endl;

	// compute
	cout << "The program is now trying to prove the statement." << endl;
	cout << "Termination of the program is not guaranteed." << endl;
	cout << "Please close all your active programs, as this program requires a lot of RAM." << endl;
	cout << "You can terminate the computation by pressing CTRL+C" << endl << endl;
	auto proof = input.prove( premises );
	
	// proof
	cout << "Done!" << endl;
	if( proof.size() == 0 ) {
		cout << "The statement cannot be proven from the axioms." << endl;
	} else {
		cout << "The proof is as follows: " << endl;
		cout << proof.back() << endl;
		for( auto itr = proof.rbegin()+1; itr != proof.rend(); ++itr ) {
			cout << "=" << (*itr) << endl;
		}
	}

	return 0;
}