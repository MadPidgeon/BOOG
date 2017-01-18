#include <iostream>
#include <vector>
#include "bintree.h"
using namespace std;

int main() {
	subtree_equivalence equi1("(0+0)=0");
	cout << "Equivalence 1: " << equi1 << endl;

	subtree_equivalence equi2("((0+1)+2)=((1+2)+0)");
	cout << "Equivalence 2: " << equi2 << endl;

	subtree_equivalence equi3("(0+1)=(1+0)");
	cout << "Equivalence 3: " << equi3 << endl;

	subtree_equivalence equi4("((0*1)*2)=(0*(1*2))");
	cout << "Equivalence 4: " << equi4 << endl;

	cout << "---------------------------------" << endl;

	binary_tree statement("(0+1)");
	cout << "Statement 1: " << statement << endl;

	cout << "---------------------------------" << endl;

	binary_tree result;
	cout << "Apply equivalence 1 to statement 1: " << endl;
	cout << ( equi1.apply( 1, statement, statement.crootitr(), result ) ? "Succesful" : "Failure" ) << endl;
	cout << result << endl;

	cout << "---------------------------------" << endl;

	cout << "Prove equivalence 3 from 1 and 2:" << endl;
	auto proof = equi3.prove( {equi1,equi2} );
	if( proof.empty() )
		cout << "The statement cannot be constructively proven from the axioms." << endl;
	else for( auto step : proof )
		cout << step << endl;

	return 0;
}
