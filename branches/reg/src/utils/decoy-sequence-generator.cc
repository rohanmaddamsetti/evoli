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


#include "decoy-contact-folder.hh"
#include "translator.hh"
#include "folder-util.hh"
#include "tools.hh"
#include "random.hh"

#include <fstream>


struct Parameters
{
	string structure_file;
	string structure_dir;
	int protein_length;
	double free_energy_cutoff;
	int repetitions;
	int random_seed;
	int struct_id;
};


ostream & operator<<( ostream &s, const Parameters &p )
{
	s << "# Parameters:" << endl;
	s << "#   protein length: " << p.protein_length << endl;
	s << "#   free energy cutoff: " << p.free_energy_cutoff << endl;
	s << "#   repetitions: " << p.repetitions << endl;
	s << "#   random seed: " << p.random_seed << endl;
	s << "#   target structure id: " << p.struct_id << endl;
	s << "#" << endl;
	return s;
}

Parameters getParams( int ac, char **av )
{
	if ( ac != 8 )
	{
		cout << "Start program like this:" << endl;
		cout << "  " << av[0] << " <struct list file> <struct dir> <prot length> <free_energy_cutoff> <repetitions> <random seed> [<struct id>|-1]" << endl;
		exit (-1);
	}

	Parameters p;
	int i = 1;
	p.structure_file = av[i++];
	p.structure_dir = av[i++];
	p.protein_length = atoi( av[i++] );
	p.free_energy_cutoff = atof( av[i++] );
	p.repetitions = atoi( av[i++] );
	p.random_seed = atoi( av[i++] );
	p.struct_id = atoi( av[i++] );

	return p;
}


// finds a random sequence with folding energy smaller than cutoff.
void getSequence( Folder &b, const Parameters &p, ostream &s )
{
	Gene g = FolderUtil::getSequence(b, 3*p.protein_length, p.free_energy_cutoff);
	Protein prot = g.translate();
	auto_ptr<FoldInfo> fdata( b.fold(prot) );
	s << g << " " << fdata->getDeltaG() << " " << fdata->getStructure() << endl; //" " << FolderUtil::calcNeutrality( b, prot, p.free_energy_cutoff ) << endl;
}

// finds a random sequence with folding energy smaller than cutoff and structure given by struct_id
void getSequenceTargeted( Folder &b, const Parameters &p, const int struct_id, ostream &s )
{
	Gene g = FolderUtil::getSequenceForStructure(b, 3*p.protein_length, p.free_energy_cutoff, struct_id);
	Protein prot = g.translate();
	auto_ptr<FoldInfo> fdata( b.fold(prot) );
	s << g << " " << fdata->getDeltaG() << " " << fdata->getStructure() << endl; // << " " << FolderUtil::calcNeutrality( b, prot, p.free_energy_cutoff ) << endl;
}

int main( int ac, char **av)
{
	Parameters p = getParams( ac, av );

	// set random seed
	Random::seed( p.random_seed );

	string path = (p.structure_dir+p.structure_file);
	ifstream fin(path.c_str());
	if (!fin.good()) { // if we can't read the contact maps file, bail out
		cerr << "ERROR: can't read contact maps from " << path << endl;
		return 1;
	}
	double log_nconf = 10.0*log(10.0);
	DecoyContactFolder folder(p.protein_length, log_nconf, fin, p.structure_dir);
	if (!folder.good()) {// if we can't read the contact maps file, bail out
		cerr << "ERROR: couldn't initialize folder." << endl;
		return 1;
	}
	fin.close();

	cout << p;
	cout << "# <sequence> <free energy> <structure id>" << endl;

	for ( int i=0; i<p.repetitions; i++ )
	{
		if (p.struct_id < 0) {
			getSequence( folder, p, cout );
		}
		else {
			getSequenceTargeted( folder, p, p.struct_id, cout );
		}
	}

	return 0;
}




