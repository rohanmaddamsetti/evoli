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



#include "compact-lattice-folder.hh"
#include "fitness-evaluator.hh"
#include "population.hh"
#include "translator.hh"
#include "tools.hh"
#include "random.hh"
#include "gene-util.hh"
#include "folder-util.hh"

#include <cmath>
#include <cstdio>
#include <fstream>
#include <ctime>
#include <iomanip>

struct Stats
{
	double sum;
	double sumSquared;
	uint samples;

	void addValue(double val)
	{
		sum += val;
		sumSquared += val*val;
		samples++;
	}

	void reset(void)
	{
		sum = 0.0;
		sumSquared = 0.0;
		samples = 0;
	}

	double getMean(void) { return sum/samples; }
	double getVariance(void) { return sumSquared/samples - (sum/samples)*(sum/samples); }
	double getSampleVariance(void) { return sumSquared/(samples-1) - (sum/samples)*(sum/samples)*(double)samples/(samples-1); }
	double getStandardDeviation(void) { return sqrt(getVariance()); }
	double getStandardError(void) { return sqrt(getVariance()/samples); }
	double getZscore(double val) { return (val - getMean())/getStandardDeviation(); }
};

struct RunRecord
{
	string orf;
	string run_id;
	double cost;
	int rep;
	Gene gene;
};

ostream& operator<<(ostream& os, const RunRecord& rec) {
	os << rec.orf << tab << rec.cost << tab << rec.run_id << tab << "rep " << rec.rep << tab << rec.gene << endl;
	return os;
}

class Parameters {
public:
	string eval_type;
	int protein_length;
	int N;
	double ca_cost;
	double error_rate;
	double accuracy_weight;
	double error_weight;
	mutable int structure_ID;
	double free_energy_cutoff;
	double free_energy_minimum;
	int random_seed;
	string genotype_file_name;
	int num_to_fold;
	string out_file_name;
	bool valid;


	Parameters( int ac, char **av ) {
		if ( ac != 15 )	{
			valid = false;
			return;
		}

		int i = 1;
		eval_type = av[i++];
		protein_length = atoi( av[i++] );
		N = atoi( av[i++] );
		ca_cost = atof( av[i++] );
		error_rate = atof( av[i++] );
		accuracy_weight = atof( av[i++] );
		error_weight = atof( av[i++] );
		structure_ID = atoi( av[i++] );
		free_energy_cutoff = atof( av[i++] );
		free_energy_minimum = atof( av[i++] );
		random_seed = atoi( av[i++] );
		genotype_file_name = av[i++];
		num_to_fold = atoi( av[i++] );
		out_file_name = av[i++];

		valid = true;
	}
};


ostream & operator<<( ostream &s, const Parameters &p )
{
	s << "# Parameters:" << endl;
	s << "#   evaluation type: " << p.eval_type << endl;
	s << "#   protein_length: " << p.protein_length << endl;
	s << "#   structure id: " << p.structure_ID << endl;
	s << "#   free energy maximum: " << p.free_energy_cutoff << endl;
	s << "#   free energy minimum: " << p.free_energy_minimum << endl;
	s << "#   codon adaptation cost factor: " << p.ca_cost << endl;
	s << "#   transl. error rate per codon: " << p.error_rate << endl;
	s << "#   transl. accuracy weight: " << p.accuracy_weight << endl;
	s << "#   transl. error weight: " << p.error_weight << endl;
	s << "#   population size N: " << p.N << endl;
	s << "#   random seed: " << p.random_seed << endl;
	s << "#   genotype file: " << p.genotype_file_name << endl;
	s << "#   output file: " << p.out_file_name << endl;
	s << "#" << endl;
	return s;
}

StructureID getStructureID( const Folder &b, const Gene &g ) {
	if ( g.encodesFullLength() ) {
		Protein p = g.translate();
		auto_ptr<FoldInfo> fi( b.fold(p) );
		return fi->getStructure();
	}
	else
		return (StructureID)-1;
}

void misfoldDistExperiment(Parameters& p)
{
	// Seed random number generator
	long seconds = (long)time(NULL);
	Random::seed(p.random_seed);

	int side_length = (int)(sqrt(float(p.protein_length)));
	// initialize the protein folder
	CompactLatticeFolder folder(side_length);

	// Read the results.
	vector<RunRecord> runResults;
	ifstream fin(p.genotype_file_name.c_str());
	char buf[100];
	// Skip two lines
	fin.getline(buf,100);
	fin.getline(buf,100);
	while (!fin.eof()) {
		RunRecord rec;
		fin >> rec.orf >> rec.run_id >> rec.cost >> rec.rep >> rec.gene;
		if (rec.gene.length() > 0) {
			runResults.push_back(rec);
			//cout << rec;
		}
	}
	fin.close();

	ofstream fout( p.out_file_name.c_str() );
	fout << "# Read " << runResults.size() << " results" << endl;
	fout << p;
	fout << "orf\trun_id\ttr\trep\tdG\tfitness\tfop\tnu\tnacc\tnrob\tntrunc\tnfold\tfacc\tfrob\tftrunc\tffold\tcfacc\tcfrob\tcftrunc\tcffold\tfdens\tmrandrob\tsdrandrob\tgene" << endl;
	vector<RunRecord>::iterator it = runResults.begin();
	bool printCodonReport = true;

	int structure_ID = getStructureID( folder, (*it).gene );
	if ( structure_ID < 0 )	{
		cerr << "# Input sequence does not translate!" << endl;
		exit(1);
	}
	ErrorproneTranslation* fe = NULL;
	ErrorproneTranslation* ept = new ErrorproneTranslation( &folder, p.protein_length, p.structure_ID, p.free_energy_cutoff, 1, p.ca_cost, p.error_rate, p.accuracy_weight, p.error_weight );
	fe = ept;
	vector<bool> isOptimal = fe->getOptimalCodons(printCodonReport);

	double last_cost = -1;
	for (; it != runResults.end(); it++) {
		RunRecord& rec = *it;
		//cout << rec << endl;
		/*if (rec.cost < 0.01 || rec.cost > 100) {
			continue;
			}*/
		// find initial structure

		if (rec.cost != last_cost) {
			delete fe;
			if (p.eval_type == "tr") {
				ErrorproneTranslation* ept = new ErrorproneTranslation( &folder, p.protein_length, p.structure_ID, p.free_energy_cutoff, rec.cost, p.ca_cost, p.error_rate, p.accuracy_weight, p.error_weight );
				fe = ept;
			}
			else if (p.eval_type == "acc") {
				AccuracyOnlyTranslation* afe = new AccuracyOnlyTranslation( &folder, p.protein_length, p.structure_ID, p.free_energy_cutoff, rec.cost, p.ca_cost, p.error_rate, p.accuracy_weight, p.error_weight );
				fe = afe;
			}
			else if (p.eval_type == "rob") {
				RobustnessOnlyTranslation* rob = new RobustnessOnlyTranslation( &folder, p.protein_length, p.structure_ID, p.free_energy_cutoff, rec.cost, p.ca_cost, p.error_rate );
				fe = rob;
			}
			if (!fe) {
				cerr << "ERROR: unknown fitness evaluator type '" << p.eval_type << "'.  Exiting..." << endl;
				exit(1);
			}
		}
		//cout << "# Error rate: " << fe->getErrorRate() << endl;

		printCodonReport = false;

		double fop = GeneUtil::calcFop( rec.gene, isOptimal);
		Protein prot = rec.gene.translate();
		auto_ptr<FoldInfo> fi( folder.fold(prot) );
		double dG = fi->getDeltaG();

		int numAccurate = 0;
		int numTruncated = 0;
		int numFolded = 0;
		int numRobust = 0;

		fe->countOutcomes(rec.gene, p.num_to_fold, numAccurate, numRobust, numTruncated, numFolded);

		double ffold = (double)numFolded/p.num_to_fold;
		double facc = (double)numAccurate/p.num_to_fold;
		double frob = (double)numRobust/(p.num_to_fold-numAccurate);
		double ftrunc = (double)numTruncated/p.num_to_fold;

		double cffold, cfacc, cfrob, cftrunc;
		double fitness = fe->calcOutcomes(rec.gene, cfacc, cfrob, cftrunc, cffold);
		double fitnessDensity = -1; //fde.getFitnessDensity(rec.gene, *fe, p.N);
		double nu = FolderUtil::calcNeutrality( folder, prot, p.free_energy_cutoff );

		// Compute fraction robust for codon-randomized genes.
		vector<double> randrobs;
		double rfacc, rfrob, rftrunc, rffold;
		int max_rand_trials = 100;
		int ri = 0;
		//cout << "\n*";
		//GenotypeUtil::printProtein(cout, rec.gene);
		while (ri < max_rand_trials) {
			Gene rg = GeneUtil::randomizeCodons(rec.gene);
			double rfitness = fe->calcOutcomes(rg, rfacc, rfrob, rftrunc, rffold);
			if (rfitness > 0) {
				//cout << rg << tab << rfrob << endl;
				randrobs.push_back(rfrob);
				ri++;
			}
			else {
				ri++;
				//cout << " ";
				//GenotypeUtil::printProtein(cout, rg);
			}
		}
		double mean_randrob = mean(randrobs);
		double sd_randrob = sqrt(variance(randrobs));

		fout << rec.orf << tab << rec.run_id << tab << rec.cost << tab << rec.rep << tab << dG << tab << fitness << tab << fop << tab << nu << tab
			 << numAccurate << tab << numRobust << tab << numTruncated << tab << numFolded << tab
			 << setprecision(6) << facc << tab << frob << tab << ftrunc << tab << ffold << tab
			 << cfacc << tab << cfrob << tab << cftrunc << tab << cffold << tab
			 << fitnessDensity << tab << mean_randrob << tab << sd_randrob << tab << rec.gene << endl;

		last_cost = rec.cost;
	}
	fout.close();
	delete fe;
}

int main( int ac, char **av)
{
	Parameters p( ac, av );
	if (p.valid) {
		misfoldDistExperiment(p);
	}
	else {
		cout << "Start program like this:" << endl;
		cout << "\t" << av[0] << " <eval type> <prot length> <pop size> <ca cost> <error rate> <accuracy weight> <error weight> <structure id> <free energy cutoff> <free energy minimum> <random seed> <gene file name> <num. to fold> <outfile>" << endl;
	}
}


