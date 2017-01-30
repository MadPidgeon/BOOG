#include <iostream>
#include <vector>
#include "bintree.h"
#include "counter.h"
using namespace std;

int main() {
	subtree_equivalence equi1("(a+a)=a");
	cout << "Equivalence 1: " << equi1 << endl;

	subtree_equivalence equi2("((a+b)+c)=((b+c)+a)");
	cout << "Equivalence 2: " << equi2 << endl;

	subtree_equivalence equi3("(a+b)=(b+a)");
	cout << "Equivalence 3: " << equi3 << endl;

	subtree_equivalence equi4("((a+b)+c)=(a+(b+c))");
	cout << "Equivalence 4: " << equi4 << endl;

	subtree_equivalence equi5("(a+0)=a");
	cout << "Equivalence 5: " << equi5 << endl;

	subtree_equivalence equi6("(0+a)=a");
	cout << "Equivalence 6: " << equi6 << endl;

	subtree_equivalence equi7("((a+b)+c)=(c+(b+a))");
	cout << "Equivalence 7: " << equi7 << endl;

	subtree_equivalence equi8("(0+0)=0");
	cout << "Equivalence 8: " << equi8 << endl;

	subtree_equivalence equi9("(0*a)=0");
	cout << "Equivalence 9: " << equi9 << endl;

	subtree_equivalence equi10("(0*(1/0))=1");
	cout << "Equivalence 10: " << equi10 << endl;

	subtree_equivalence equi11("0=1");
	cout << "Equivalence 11: " << equi11 << endl;

	subtree_equivalence equi12("a=(a+(a+(a+b)))");
	cout << "Equivalence 12: " << equi12 << endl;

	subtree_equivalence equi13("a=(((b+a)+a)+a)");
	cout << "Equivalence 13: " << equi13 << endl;

	cout << "---------------------------------" << endl;

	binary_tree statement("(a+b)");
	cout << "Statement 1: " << statement << endl;

	cout << "---------------------------------" << endl;

	binary_tree result;
	cout << "Apply equivalence 1 to statement 1: " << endl;
	cout << ( equi1.apply( 1, statement, statement.crootitr(), result ) ? "Succesful" : "Failure" ) << endl;
	cout << result << endl;

	cout << "---------------------------------" << endl;

	cout << "Prove equivalence 3 from 1 and 2:" << endl;
	auto proof1 = equi3.prove( {equi1,equi2} );
	if( proof1.empty() )
		cout << "The statement cannot be constructively proven from the axioms." << endl;
	else for( auto step : proof1 )
		cout << step << endl;

	cout << "---------------------------------" << endl;

	cout << "Prove equivalence 3 from 5, 6 and 7:" << endl;
	auto proof2 = equi3.prove( {equi5,equi6,equi7} );
	if( proof2.empty() )
		cout << "The statement cannot be constructively proven from the axioms." << endl;
	else for( auto step : proof2 )
		cout << step << endl;

	cout << "---------------------------------" << endl;

	cout << "Prove equivalence 8 from 5:" << endl;
	auto proof3 = equi8.prove( {equi5} );
	if( proof3.empty() )
		cout << "The statement cannot be constructively proven from the axioms." << endl;
	else for( auto step : proof3 )
		cout << step << endl;

	cout << "---------------------------------" << endl;

	cout << "Prove equivalence 4 from 12,13:" << endl;
	auto proof4 = equi4.prove( {equi12,equi13} );
	if( proof4.empty() )
		cout << "The statement cannot be constructively proven from the axioms." << endl;
	else for( auto step : proof4 )
		cout << step << endl;

	cout << "---------------------------------" << endl;

	cout << "Prove equivalence 11 from 9,10:" << endl;
	auto proof5 = equi11.prove( {equi9,equi10} );
	if( proof5.empty() )
		cout << "The statement cannot be constructively proven from the axioms." << endl;
	else for( auto step : proof5 )
		cout << step << endl;

	cout << "---------------------------------" << endl;

	/*operator_table XOR( '+', {0,1,1,0} );
	cout << XOR << endl << endl;

	operator_table OR( '|', {0,1,1,1} );
	cout << OR << endl;*/

	return 0;
}
