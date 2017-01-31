#include <iostream>
#include "bintree.h"

using namespace std;

int main() {
	binary_tree L1("((B*0)*B)"), R1("(A*(A*0))");
	binary_tree L2("(B*0)"), R2("((0*A)*A)");
	binary_tree L3("B"), R3("A");
	binary_tree L4("((A*B)*((A*0)*(B*0)))"), R4("((C*D)*(D*C))");

	for( auto Z : {tie(L1,R1),tie(L2,R2),tie(L3,R3),tie(L4,R4)} ) {
		cout << get<0>(Z) << endl << get<1>(Z) << endl;
		substitution_rules X( &*get<0>(Z).crootitr(), &*get<1>(Z).crootitr() );
		cout << bool(X) << endl;
		if( X ) {
			cout << "a=" << X.at(0) << endl;
			cout << "b=" << X.at(1) << endl;
			try {
				cout << "c=" << X.at(2) << endl;
				cout << "d=" << X.at(3) << endl;
			} catch( ... ) {}
		}
		cout << "-------------------------------" << endl;
	}

	return 0;
}