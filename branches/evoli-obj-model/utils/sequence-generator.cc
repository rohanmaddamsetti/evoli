#include "protein-folder.hh"
#include "translator.hh"
#include "gene-util.hh"
#include "tools.hh"

#include <fstream>


struct Parameters
{
	double free_energy_cutoff;
	int repetitions;
	int random_seed;
	int struct_id;
};


ostream & operator<<( ostream &s, const Parameters &p )
{
	s << "# Parameters:" << endl;
	s << "#   free energy cutoff: " << p.free_energy_cutoff << endl;
	s << "#   repetitions: " << p.repetitions << endl;
	s << "#   random seed: " << p.random_seed << endl;
	s << "#   target structure id: " << p.struct_id << endl;
	s << "#" << endl;
	return s;
}

Parameters getParams( int ac, char **av )
{
	if ( ac != 5 )
	{
		cout << "Start program like this:" << endl;
		cout << "  " << av[0] << " <free_energy_cutoff> <repetitions> <random seed> [<struct id>|-1]" << endl;
		exit (-1);
	}

	Parameters p;
	int i = 1;
	p.free_energy_cutoff = atof( av[i++] );
	p.repetitions = atoi( av[i++] );
	p.random_seed = atoi( av[i++] );
	p.struct_id = atoi( av[i++] );

	return p;
}


// finds a random sequence with folding energy smaller than cutoff.
void getSequence( ProteinFolder &b, const Parameters &p, ostream &s )
{
	Gene g = Gene::getSequence(b, p.free_energy_cutoff);
	Protein prot = g.translate();
	pair<int,double> fdata = prot.fold(b);
	s << g << " " << fdata.second << " " << fdata.first << " " << GeneUtil::calcNeutrality( b, prot, p.free_energy_cutoff )
	  << endl;
}

// finds a random sequence with folding energy smaller than cutoff and structure given by struct_id
void getSequenceTargeted( ProteinFolder &b, const Parameters &p, const int struct_id, ostream &s )
{
	Gene g = Gene::getSequenceForStructure(b, p.free_energy_cutoff, struct_id);
	Protein prot = g.translate();
	pair<int,double> fdata = prot.fold(b);
	s << g << " " << fdata.second << " " << fdata.first << " " << GeneUtil::calcNeutrality( b, prot, p.free_energy_cutoff )
	  << endl;
}

int main( int ac, char **av)
{
	Parameters p = getParams( ac, av );

	// set random seed
	srand48( p.random_seed );

	// size of the lattice proteins is hardcoded
	const int size = 5;

	// initialize the protein folder
	ProteinFolder b(size);
	b.enumerateStructures();

	cout << p;
	cout << "# <sequence> <free energy> <structure id> <neutrality>" << endl;

	for ( int i=0; i<p.repetitions; i++ )
	{
		if (p.struct_id < 0) {
			getSequence( b, p, cout );
		}
		else {
			getSequenceTargeted( b, p, p.struct_id, cout );
		}
	}
}




