/*
This file is part of the E.voli project.
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


#include "protein.hh"
#include "translator.hh"
#include "codon.hh"
#include <sstream>

Protein::Protein(const int length) : Sequence(length) {
}

Protein::Protein(const Protein& p) : Sequence(p.m_sequence) {
}

Protein::Protein(const string& seq_string) : Sequence(0) {
	stringstream s(seq_string);
	string seq;
	s >> seq;

	m_sequence.clear();
	for (int i=0; i<seq.size(); i++) {
		m_sequence.push_back( GeneticCodeUtil::letter_to_residue_map[seq[i]] );
	}
}

int Protein::distance(const Protein& p) const {
	// we don't handle sequences of different lengths properly, 
	// so they better be of the same length	
	assert( length() == p.length() );
	
	int diffs = 0;
	Protein::const_iterator qit = begin();
	Protein::const_iterator pit = p.begin();

	for (; qit != end() && pit != p.end(); qit++, pit++) {
		if (*pit != *qit) {
			diffs++;
		}
	}
	return diffs;
}

Protein Protein::createRandom(const int length) {
	Protein p(length);
	Protein::iterator pit = p.begin();
	for (; pit != p.end(); pit++) {
		*pit = Random::rint(20);
	}
	return p;
}

string Protein::toString() const {
	string res;
	for ( Protein::const_iterator it=begin(); it != end(); it++) {
		res += GeneticCodeUtil::residueLetters[*it+1];
	}
	return res;
}



Gene::Gene() : Sequence(0) {
}

Gene::Gene(const int length) : Sequence(length/3) {
}

Gene::Gene(const Gene& g) : Sequence(g.m_sequence) {
}

Gene::Gene(const string& seq_string) : Sequence(0) {
	stringstream s(seq_string);
	char c1, c2, c3;

	// read leading whitespace
	do {
		s.get( c1 );
	}
	while ( s && ( ( c1 == ' ' ) || ( c1 == '\n' ) || ( c1 == '\t' ) ) );

	// read until next whitespace
	while ( s && ( c1 != ' ' ) && ( c1 != '\n' ) && ( c1 != '\t' ) ) {
		s.get( c2 );
		s.get( c3 );
		m_sequence.push_back( CodonUtil::lettersToCodon( c1, c2, c3 ) );
		s.get( c1 );
	}
}

char Gene::getBase(const uint index) const {
	char res = 'X';
	int triplet_pos = index % 3;
	int codon = (*this)[(index-triplet_pos)/3];
	//cout << codon << " ";
	//CodonUtil::printCodon(cout, codon);
	//cout << " ";
	int x = (codon >> (2*(2-triplet_pos))) & 3;
	switch( x )	{
		case 0:
			res = 'A';
			break;
		case 1:
			res = 'C';
			break;
		case 2:
			res = 'G';
			break;
		case 3:
			res = 'U';
			break;
	}
	//cout << res << endl;
	return res;
}

string Gene::toString() const {
	string res;
	for (uint i=0; i<m_sequence.size()*3; i++) {
		res += getBase(i);
	}
	return res;
}

Gene Gene::createRandom(const int length ) {
	Gene g( length );
	Gene::iterator it = g.begin();

	for ( ; it != g.end(); it++) {
		*it = Random::rint( 64 );
	}
	return g;
}

Gene Gene::createRandomNoStops(const int length ) {
	Gene g( length );
	Gene::iterator it = g.begin();

	for ( ; it != g.end(); it++) {
		do {
			*it = Random::rint( 64 );
		} while (GeneticCodeUtil::geneticCode[*it] < 0);
	}
	return g;
}

bool Gene::encodesFullLength(void) const {
	bool full_length = (length() % 3)==0;
	Gene::const_iterator it = begin();
	while (it != end() && full_length) {
		full_length = (GeneticCodeUtil::geneticCode[*it] >= 0);
		it++;
	}
	return full_length;
}

bool Gene::mutate(const double prob) {
	bool changed = false;
	Gene::iterator it = begin();
	for ( ; it != end(); it++) {
		int codon = *it;
		(*it) = CodonUtil::mutateCodon( prob, codon );
		if ( !changed && (*it) != codon ) {
			changed = true;
		}
	}
	return changed;
}

Protein Gene::translate(const Translator& t) const {
	Protein prot(codonLength());
	bool translation_successful = t.translate(*this, prot);
	assert( translation_successful ); // This function should never be used on genes that don't translate correctly
	return prot;
}

Protein Gene::translate(void) const {
	int len = codonLength();
	Translator t;
	Protein prot(len);
	bool translation_successful = t.translate(*this, prot);
	assert( translation_successful ); // This function should never be used on genes that don't translate correctly
	//cout << this << tab << "translation" << endl;
	return prot;
}


