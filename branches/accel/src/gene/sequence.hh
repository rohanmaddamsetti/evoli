/*
This file is part of the evoli project.
Copyright (C) 2004, 2005, 2006 Claus Wilke <cwilke@mail.utexas.edu>,
Allan Drummond <dadrummond@gmail.com>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1
*/


#ifndef SEQUENCE_HH
#define SEQUENCE_HH

#include <string>
#include <iostream>
#include <vector>

using namespace std;


/** \brief Basic class to store a biological sequence of any type (DNA, protein).
**/

//typedef string Sequence;

class Sequence : public string {
public:
	Sequence(const string& s) : string(s) {}
	Sequence(const char* s) : string(s) {}
	Sequence(const Sequence& s) : string(s) {}
	Sequence(unsigned int length, char val) : string(length, val) {}
	Sequence(unsigned int length) : string(length, 'A') {}
	Sequence(const Sequence& s, unsigned int start, unsigned int length) : string(s.substr(start,length)) {}

	virtual ~Sequence() {}
	
	/**
	 * Convert this sequence to a string representation.
	 * @return A string representation of the sequence.
	 */
	const string& toString() { return *this; }

	/**
	Calculates the Hamming distance (number of differences) to another sequence.  Counts length differences and character differences equally.
	@return The Hamming distance to the sequence given as argument.
	*/
	int distance(const Sequence& s2) const {
		int diffs = 0;
		const Sequence& s = *this;
		int min_index = min(s.length(), s2.length());
		int max_index = max(s.length(), s2.length());
		for (int i=0; i<min_index; i++) {
			if (s[i] != s2[i]) {
				diffs++;
			}
		}
		return diffs + (max_index - min_index);
	}

  vector<uint> getDifferences(const Sequence& s2) const {
	vector<uint> diffs;
		const Sequence& s = *this;
		int min_index = min(s.length(), s2.length());
		// Note: this does not treat nonoverlapping sequence as differences.
		for (uint i=0; i<min_index; i++) {
			if (s[i] != s2[i]) {
			  diffs.push_back(i);
			}
		}
		return diffs;
  }
};

//typedef pair<int,int> Contact;
class Contact : public pair<int,int> {
public:
	Contact(int i, int j) {
		if (i<j) {
			first = i;
			second = j;
		}
		else {
			first = j;
			second = i;
		}
	}
	Contact() { first = 0; second = 0;}
	virtual ~Contact() {}

	bool operator==(const Contact& c) const {
		// Order doesn't matter for contacts.
		return ((first == c.first) && (second == c.second)) || ((second == c.first) && (first == c.second));
	}
};

ostream& operator<<(ostream& os, const Contact& c);


#endif //SEQUENCE_HH
