#ifndef _T_FOLDER_H__
#define _T_FOLDER_H__
#include "cutee.h"
#include "decoy-contact-folder.hh"
#include "compact-lattice-folder.hh"
#include "protein.hh"
#include <fstream>

struct TEST_CLASS( folder_basic )
{
	const static int side_length = 5;
	const static int gene_length = side_length*side_length*3;
	void TEST_FUNCTION( init_lattice )
	{
		Folder* folder = new CompactLatticeFolder(side_length);
		Protein p("CSVMQGGKTVFQMPIIERVMQAYNI"); //Gene::createRandomNoStops(gene_length);
		FoldInfo fi = folder->fold(p);
		TEST_ASSERT(abs(fi.getFreeEnergy()-0.564) < 1e-2);
		TEST_ASSERT(fi.getStructure() == (StructureID)225);
		delete folder;
		return;
	}
	void TEST_FUNCTION( init_decoy )
	{
		ifstream fin("test/data/contact_maps/maps.txt");
		int protein_length = 300;
		double log_nconf = 160.0*log(10.0);
		Folder* folder = new DecoyContactFolder(protein_length, log_nconf, fin, "test/data/contact_maps/");
		TEST_ASSERT( folder->good() );
		if (!folder->good() )
			return;
		int num_to_fold = 1;
		for (int j=0; j<num_to_fold; j++) {
			Gene g = Gene::createRandomNoStops(protein_length*3);
			Protein p = g.translate();
			FoldInfo fi = folder->fold(p);
			TEST_ASSERT(fi.getStructure()>-1);
			//cout << "folded:" << tab << fi.getStructure() << tab << fi.getFreeEnergy() << endl;
		}
		// Clean up
		delete folder;
		return;
	}

	void TEST_FUNCTION( contact_reader ) {
		string seq = "STLRFVAVGDWGGVPNAPFHTAREMANAKEIARTVQIMGADFIMSLGDNFYFTGVHDANDKRFQETFEDVFSDRALRNIPWYVLAGNHDHLGNVSAQIAYSKISKRWNFPSPYYRLRFKVPRSNITVAIFMLDTVMLCGNSDDFVSQQPEMPRDLGVARTQLSWLKKQLAAAKEDYVLVAGHYPIWSIAEHGPTRCLVKNLRPLLAAYGVTAYLCGHDHNLQYLQDENGVGYVLSGAGNFMDPSVRHQRKVPNGYLRFHYGSEDSLGGFTYVEIGSKEMSITYVEASGKSLFKTSLPRRP";

		const char* fname = "1qhwA_6_CB.cmap";
		ifstream fin;
		DecoyContactStructure structure;
		string filename = string("test/data/williams_contact_maps/")+fname;
		fin.open(filename.c_str());
		TEST_ASSERT( fin.good() );
		if (!fin.good()) // if we can't read the contact maps, bail out
			return;
		structure.read(fin);
		fin.close();
		fin.open(filename.c_str());
		vector<Contact> contacts = structure.getContacts();
		int r1, r2;
		char r1aa, r2aa;
		for (int i=0; i<contacts.size() && !fin.eof(); i++) {
			fin >> r1 >> r1aa >> r2 >> r2aa;
			TEST_ASSERT(contacts[i].first == r1);
			TEST_ASSERT(contacts[i].second == r2);
			TEST_ASSERT(seq[r1] == r1aa);
			TEST_ASSERT(seq[r2] == r2aa);
			//cout << contacts[i].first << " " << r1 << " " << contacts[i].second << " " << r2 << " " << seq[r1] << " " << r1aa << " " << seq[r2] << " " << r2aa << endl;
		}
		fin.close();
		
	}

	void TEST_FUNCTION( williams_test ) {
		// Compare to 
		// Load the real sequence
		// Compute free energy
		string native_1qhw_seq = "STLRFVAVGDWGGVPNAPFHTAREMANAKEIARTVQIMGADFIMSLGDNFYFTGVHDANDKRFQETFEDVFSDRALRNIPWYVLAGNHDHLGNVSAQIAYSKISKRWNFPSPYYRLRFKVPRSNITVAIFMLDTVMLCGNSDDFVSQQPEMPRDLGVARTQLSWLKKQLAAAKEDYVLVAGHYPIWSIAEHGPTRCLVKNLRPLLAAYGVTAYLCGHDHNLQYLQDENGVGYVLSGAGNFMDPSVRHQRKVPNGYLRFHYGSEDSLGGFTYVEIGSKEMSITYVEASGKSLFKTSLPRRP";

		int protein_length = 300;
		double log_nconf = 160.0*log(10.0);
		ifstream fin("test/data/williams_contact_maps/maps.txt");
		Folder* folder = new DecoyContactFolder(protein_length, log_nconf, fin, "test/data/williams_contact_maps/");
		TEST_ASSERT( folder->good() );
		if (!folder->good() )
			return;
		Protein p(native_1qhw_seq);
		FoldInfo fi = folder->fold( p );
		//TEST_ASSERT(fi.getStructure()>-1);
		//cout << "Williams:" << endl;
		//cout << "folded:" << tab << fi.getStructure() << tab << fi.getFreeEnergy() << endl;
		// Clean up
		delete folder;
	}
};


#endif // _T_FOLDER_H__