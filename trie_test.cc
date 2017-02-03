#include <iostream>
#include <string>
#include "trie.h"

using namespace std;

int main() {
	trie words;
	words.insert("table");
	words.insert("tar");
	words.insert("tart");
	words.insert("trie");
	words.insert("trip");
	words.insert("try");
	// words.insert("tare");
	words.insert("tab");
	words.insert("milkman");
	words.insert("milkyway");
	words.insert("milady");
	words.print( cout );
	cout << endl;

	string milkyway = words.compress("milkyway");
	cout << "milkyway => " << milkyway << " => " << words.expand( milkyway ) << endl;

	string milkman = words.compress("milkman");
	cout << "milkman => " << milkman << " => " << words.expand( milkman ) << endl;

	string milady = words.compress("milady");
	cout << "milady => " << milady << " => " << words.expand( milady ) << endl;

	string tar = words.compress("tar");
	cout << "tar => " << tar << " => " << words.expand( tar ) << endl;

	string tart = words.compress("tart");
	cout << "tart => " << tart << " => " << words.expand( tart ) << endl;
}