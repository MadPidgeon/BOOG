#include <iostream>
#include "bintree.h"

using namespace std;

int main() {
	binary_tree L1("((B*0)*B)"), R1("(A*(A*0))");
	cout << L1 << endl << R1 << endl;
	substitution_rules X1( L1, R1 );
	cout << bool(X1) << endl;

	cout << "-------------------------------" << endl;

	binary_tree L2("(B*0)"), R2("((0*A)*A)");
	cout << L2 << endl << R2 << endl;
	substitution_rules X2( L2, R2 );
	cout << bool(X2) << endl;
	if( X2 ) {
		cout << "b=" << X2.at( 0, 1 ) << endl;
		cout << "a=" << X2.at( 1, 0 ) << endl;
	}

	cout << "-------------------------------" << endl;

	binary_tree L3("B"), R3("A");
	cout << L3 << endl << R3 << endl;
	substitution_rules X3( L3, R3 );
	cout << bool(X3) << endl;

	cout << "-------------------------------" << endl;

	binary_tree L4("((A*B)*((A*0)*(B*0)))"), R4("((C*D)*(D*C))");
	cout << L4 << endl << R4 << endl;
	substitution_rules X4( L4, R4 );
	cout << bool(X4) << endl;

	return 0;
}