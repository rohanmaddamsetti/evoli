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


#include "translator.hh"

#include "random.hh"
#include "tools.hh"
#include "genetic-code.hh"
#include "protein.hh"


Translator::Translator( double mutation_prob)
		: m_mutation_prob( mutation_prob )
{}

Translator::Translator()
		: m_mutation_prob( 0 )
{}

bool Translator::translateErrorFree( const Gene &g, Protein& residue_sequence ) const {
	bool no_stop = true;

	int i = 0;
	for ( Gene::const_iterator it = g.begin(); it != g.end() && no_stop; it++ )	{
		int residue = GeneticCodeUtil::geneticCode[*it];
		no_stop = (residue >= 0 );
		residue_sequence[i++] = residue;
	}
	return no_stop;
}

bool Translator::translate( const Gene &g, Protein& residue_sequence ) const {
	if ( m_mutation_prob == 0 )
		return translateErrorFree( g, residue_sequence );

	bool no_stop = true;

	int i = 0;
	for ( Gene::const_iterator it = g.begin(); it != g.end() && no_stop; it++ )	{
		int residue = GeneticCodeUtil::geneticCode[*it];
		no_stop = (residue >= 0 );
		if ( Random::runif() < m_mutation_prob )
			residue = ( residue + Random::rint( 20 ) ) % 20;
		residue_sequence[i++] = residue;
	}
	return no_stop;
}

int Translator::translateWeighted( const Gene &g, Protein& residue_sequence, const vector<vector<pair<double, int> > >& weights,
									       const double* prefCodons, const double nonPrefCodonPenalty, bool& truncated)
{
	double mut_weight_total = 0.0;
	for ( Gene::const_iterator it = g.begin(); it != g.end(); it++ )	{
		mut_weight_total += (1.0 + prefCodons[*it]*(nonPrefCodonPenalty-1));
	}

	truncated = false;
	int numErrors = 0;
	for ( int i=0; i<g.codonLength() && !truncated; i++) {
		int residue = GeneticCodeUtil::geneticCode[g[i]];
		residue_sequence[i] = residue;

		if ( residue < 0 ) {
			truncated = true;
		}
		else {
			double threshold = m_mutation_prob*(1.0 + prefCodons[g[i]]*(nonPrefCodonPenalty-1))/(mut_weight_total/g.codonLength());
			double rand = Random::runif();
			if ( rand < threshold ) {
				// Weight the outcomes of a missense substitution.
				rand = Random::runif();
				double targ = 0.0;
				// The first member of the pair is a cumulative probability; the second is
				// the residue resulting from the error.
				for (unsigned int j=0; j<weights[g[i]].size() && (rand > targ); j++) {
					pair<double, int> p = weights[g[i]][j];
					residue_sequence[i] = p.second;
					targ = p.first;
				}
				if (residue_sequence[i] < 0) { // truncation error
					truncated = true;
				}
				if (residue_sequence[i] != residue) {
					numErrors++;
				}
			}
		}
	}
	return numErrors;
}

int Translator::translateRelativeWeighted( const Gene &g, Protein& residue_sequence, const double error_weight,
										   const vector<vector<pair<double, int> > >& weights, const double* prefCodons, 
										   const double nonPrefCodonPenalty, bool& truncated)
{
	truncated = false;
	int numErrors = 0;
	// For an average gene encoding a folded protein, the sum of the
	// site_weights over all codons should be equal to error_weight,
	// and thus the per-codon probability of a translation error
	// (possibly synonymous) will be given by m_mutation_prob.  Genes
	// with higher site_weight sums are more likely to be
	// mistranslated.
	//
	// When codons are translated with equal accuracy (no codon
	// preference), error_weight should simply be the length of the
	// gene, each codon's site_weight should be 1.0, and the per-codon
	// probability of error is exactly given by m_mutation_prob.
	double avg_error_per_site_weight = error_weight/g.codonLength();
	for ( int i=0; i<g.codonLength() && !truncated; i++) {
		int residue = GeneticCodeUtil::geneticCode[g[i]];
		residue_sequence[i] = residue;

		// Compute the site weight, accounting for codon preference.
		// Each site weight is proportional to the probability that an
		// error occurs at this codon relative to other codons in the gene.
		double site_weight = (1.0 + prefCodons[g[i]]*(nonPrefCodonPenalty-1));

		// With probability threshold_prob, make a translation error (possibly synonymous).
		double threshold_prob = m_mutation_prob * site_weight / avg_error_per_site_weight;
		double rand = Random::runif();
		if ( rand < threshold_prob ) {
			// We've made an error.  Now determine what it is.
			rand = Random::runif();
			double targ = 0.0;
			// The first member of the pair is a cumulative probability; the second is
			// the residue resulting from the error.
			// Find the event corresponding to rand.
			for (unsigned int j=0; j<weights[g[i]].size() && (rand > targ); j++) {
				pair<double, int> p = weights[g[i]][j];
				residue_sequence[i] = p.second;
				targ = p.first;
			}
			// Tabulate the results
			// Only count an error if the polymerized amino acid differs from
			// the natively encoded residue.
			if (residue_sequence[i] != residue) {
				numErrors++;
			}
		}
		// Check for truncation.  May not be an error!
		if (residue_sequence[i] < 0) {
			truncated = true;
		}
	}
	return numErrors;
}
