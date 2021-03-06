#include <iostream>
#include <vector>
#include "bintree.h"
#include "counter.h"
using namespace std;

int main() {
	subtree_equivalence axiom1("(A+A)=A");
	cout << "Equivalence 1: " << axiom1 << endl;

	subtree_equivalence axiom2("((A+B)+C)=((B+C)+A)");
	cout << "Equivalence 2: " << axiom1 << endl;

	subtree_equivalence equi3("(a+b)=(b+a)");
	cout << "Equivalence 3: " << equi3 << endl;

	subtree_equivalence axiom4("((A+B)+C)=(A+(B+C))");
	cout << "Equivalence 4: " << axiom4 << endl;

	subtree_equivalence equi4("((a+b)+c)=(a+(b+c))");
	cout << "Equivalence 4: " << equi4 << endl;

	subtree_equivalence axiom5("(A+0)=A");
	cout << "Equivalence 5: " << axiom5 << endl;

	subtree_equivalence axiom6("(0+A)=A");
	cout << "Equivalence 6: " << axiom6 << endl;

	subtree_equivalence axiom7("((A+B)+C)=(C+(B+A))");
	cout << "Equivalence 7: " << axiom7 << endl;

	subtree_equivalence equi8("(0+0)=0");
	cout << "Equivalence 8: " << equi8 << endl;

	subtree_equivalence axiom9("(0*A)=0");
	cout << "Equivalence 9: " << axiom9 << endl;

	subtree_equivalence axiom10("(0*(1/0))=1");
	cout << "Equivalence 10: " << axiom10 << endl;

	subtree_equivalence equi11("0=1");
	cout << "Equivalence 11: " << equi11 << endl;

	subtree_equivalence axiom12("A=(A+(A+(A+B)))");
	cout << "Equivalence 12: " << axiom12 << endl;

	subtree_equivalence axiom13("A=((B+A)+A)");
	cout << "Equivalence 13: " << axiom13 << endl;

	cout << "---------------------------------" << endl;

	binary_tree statement("(a+b)");
	cout << "Statement 1: " << statement << endl;
	cout << "Statement 1 hashing: ";
	statement.hash_print( cout );
	cout << endl;

	cout << "---------------------------------" << endl;

	binary_tree statement2("(A*B)");
	binary_tree statement3("(B*C)");
	cout << "Statement 2: " << statement2 << endl;
	cout << "Statement 3: " << statement3 << endl;
	cout << "Statement 2 hashing: ";
	statement2.hash_print( cout );
	cout << endl;
	cout << "Statement 3 hashing: ";
	statement3.hash_print( cout );
	cout << endl;
	cout << "Equal: " << (statement2==statement3) << endl;
	binary_tree::comparator CMP;
	cout << "Equivalent: " << CMP( statement2, statement3 ) << endl;

	/*cout << "---------------------------------" << endl;

	binary_tree result;
	cout << "Apply equivalence 1 to statement 1: " << endl;
	cout << ( axiom1.apply( 1, statement, statement.crootitr(), result ) ? "Succesful" : "Failure" ) << endl;
	cout << result << endl;

	cout << "---------------------------------" << endl;

	cout << "Prove equivalence 3 from 1 and 2:" << endl;
	auto proof1 = equi3.prove( {axiom1,axiom2} );
	if( proof1.empty() )
		cout << "The statement cannot be constructively proven from the axioms." << endl;
	else for( auto step : proof1 )
		cout << step << endl;

	cout << "---------------------------------" << endl;

	cout << "Prove equivalence 3 from 5, 6 and 7:" << endl;
	auto proof2 = equi3.prove( {axiom5,axiom6,axiom7} );
	if( proof2.empty() )
		cout << "The statement cannot be constructively proven from the axioms." << endl;
	else for( auto step : proof2 )
		cout << step << endl;

	cout << "---------------------------------" << endl;

	cout << "Prove equivalence 8 from 5:" << endl;
	auto proof3 = equi8.prove( {axiom5} );
	if( proof3.empty() )
		cout << "The statement cannot be constructively proven from the axioms." << endl;
	else for( auto step : proof3 )
		cout << step << endl;

	cout << "---------------------------------" << endl;

	cout << "Prove equivalence 11 from 9,10:" << endl;
	auto proof5 = equi11.prove( {axiom9,axiom10} );
	if( proof5.empty() )
		cout << "The statement cannot be constructively proven from the axioms." << endl;
	else for( auto step : proof5 )
		cout << step << endl;
*/
	cout << "----- New Syntax Test -----------" << endl;
	binary_tree inversion( "([0:a]*[0:[0:a]])" );
	binary_tree inversion_axiom( "(A*[0:A])" );
	substitution_rules inversion_substitution( inversion_axiom.croot(), inversion.croot() );

	cout << "----- Iterator Test -------------" << endl;
	binary_tree source( "(A+(B+C))" );
	binary_tree target( "(a+(b+c))" );
	auto itr_source = source.cbegin();
	for( int i = 0; i < 5; ++i ) {
		cout << "i=" << i << endl;
		try {
			itr_source->print( cout ), cout << endl;
			auto res = target.mirror_iterator( source, itr_source );
			res->print(cout), cout << endl;
		} catch( runtime_error e ) {
			cout << e.what() << endl;;
		}
		itr_source++;
		cout << "---------" << endl;
	}




	cout << "---------------------------------" << endl;
	equi11.prove({});

	cout << "---------------------------------" << endl;

	binary_tree statementX( "((Q+(Q+R))+0)" );
	binary_tree statementY( "(A+(A+B))" );
	binary_tree statementZ( "A" );
	auto itrX = statementX.begin();
	cout << itrX->id << endl;
	substitution_rules Rulez( &*itrX, &*statementY.crootitr() );
	auto Resultz = Rulez.apply( &*statementZ.crootitr() );
	Resultz->print( cout );
	cout<<endl;


	cout << "---------------------------------" << endl;

	cout << "Prove equivalence 11 from 12,13:" << endl;
	auto proof4 = equi11.prove( {axiom12,axiom13} );
	if( proof4.empty() )
		cout << "The statement cannot be constructively proven from the axioms." << endl;
	else for( auto step : proof4 )
		cout << step << endl;

	cout << "---------------------------------" << endl;

	

	/*operator_table XOR( '+', {0,1,1,0} );
	cout << XOR << endl << endl;

	operator_table OR( '|', {0,1,1,1} );
	cout << OR << endl;*/

	return 0;
}
