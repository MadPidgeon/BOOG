#include <iostream>
#include <vector>
#include "bintree.h"
using namespace std;

int main() {
	subtree_equivalence 
		equi1("(0+0)=0"), 
		equi2("((0+1)+2)=((1+2)+0)"), 
		equi3("(0+1)=(1+0)"), 
		equi4("((0*1)*2)=(0*(1*2))");
	binary_tree statement("((0*0)*1)");
	//map<int,const binary_tree::node*> result;
	binary_tree result;
	cout << statement << endl;
	cout << equi2.apply( 0, statement, statement.crootitr(), result ) << endl;
	cout << result << endl;

	cout << equi1 << endl << equi2 << endl << equi3 << endl << equi4 << endl << endl;

	auto proof = equi3.prove( {equi1,equi2} );
	if( proof.empty() )
		cout << "The statement cannot be constructively proven from the axioms." << endl;
	else for( auto step : proof )
		cout << step << endl;

	return 0;
}
