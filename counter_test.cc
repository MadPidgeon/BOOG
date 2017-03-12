#include <iostream>
#include "bintree.h"
#include "counter.h"

using namespace std;

int main() {
	cout << boolalpha;
	operator_table mult3( "Z/3Z multiplication", {
		0,0,0,
		0,1,2,
		0,2,1
	});
	cout << mult3 << endl;

	cout << "-----------------------------" << endl;

	subtree_equivalence assoc( "((A*B)*C)=(A*(B*C))" );	
	cout << "Checking whether " << mult3.id() << " satisfies " << assoc << ": " << check( {mult3}, {int('*')}, {assoc} ) << endl;
	
	cout << "-----------------------------" << endl;

	subtree_equivalence explicit_unit( "(1*A)=A" );
	cout << "Checking whether " << mult3.id() << " satisfies " << explicit_unit << ": " << check( {mult3}, {int('*')}, {explicit_unit} ) << endl;

	cout << "-----------------------------" << endl;

	subtree_equivalence implicit_unit( "(a*A)=A" );
	cout << "Checking whether " << mult3.id() << " satisfies " << implicit_unit << ": " << check( {mult3}, {int('*')}, {implicit_unit} ) << endl;

	cout << "-----------------------------" << endl;
	subtree_equivalence idempotent( "(A*A)=A" );
	cout << "Exists counterexample to " << implicit_unit << ", " << assoc << " => " << idempotent << ": ";
	cout << ( check( {mult3}, {int('*')}, { implicit_unit, assoc } ) and not check( {mult3}, {int('*')}, {idempotent} ) ) << endl;

}