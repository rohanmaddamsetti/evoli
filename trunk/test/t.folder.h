#ifndef _T_FOLDER_H__
#define _T_FOLDER_H__
#include "cutee.h"
#include "sequence.hh"
#include "protein-folder.hh"
#include "decoy-contact-folder.hh"
#include "compact-lattice-folder.hh"
#include <fstream>

struct TEST_CLASS( folder_basic )
{
	const static int side_length = 5;
	const static int gene_length = side_length*side_length*3;
	void TEST_FUNCTION( init_lattice )
	{
		ProteinFolder* folder = new CompactLatticeFolder(side_length);
		dynamic_cast<CompactLatticeFolder*>(folder)->enumerateStructures();
		Protein p("CSVMQGGKTVFQMPIIERVMQAYNI"); //Gene::createRandomNoStops(gene_length);
		FoldInfo fi = folder->fold(p);
		TEST_ASSERT(abs(fi.getFreeEnergy()-0.564) < 1e-2);
		TEST_ASSERT(fi.getStructure() == (StructureID)225);
		delete folder;
		return;
	}
	void TEST_FUNCTION( init_decoy )
	{

		const char* filenames[] = {"1bgvA_6.0_CA.cmap","1c96A_6.0_CA.cmap","1cyo _6.0_CA.cmap","1dkuA_6.0_CA.cmap","1ebfA_6.0_CA.cmap","1eucB_6.0_CA.cmap",
							  "1bhgA_6.0_CA.cmap","1c9rB_6.0_CA.cmap","1czaN_6.0_CA.cmap","1dkvA_6.0_CA.cmap","1ecfA_6.0_CA.cmap","1euhA_6.0_CA.cmap",
							  "1bi7B_6.0_CA.cmap","1cb0A_6.0_CA.cmap","1d0nA_6.0_CA.cmap","1dl2A_6.0_CA.cmap","1edoA_6.0_CA.cmap","1ev7A_6.0_CA.cmap",
							  "1bif _6.0_CA.cmap","1cb6A_6.0_CA.cmap","1d1jA_6.0_CA.cmap","1dljA_6.0_CA.cmap","1edyA_6.0_CA.cmap","1evlA_6.0_CA.cmap",
							  "1bio _6.0_CA.cmap","1cbiA_6.0_CA.cmap","1d1qA_6.0_CA.cmap","1dn1A_6.0_CA.cmap","1eemA_6.0_CA.cmap","1ew2A_6.0_CA.cmap",
							  "1bj4A_6.0_CA.cmap","1cbs _6.0_CA.cmap","1d2eA_6.0_CA.cmap","1dnpA_6.0_CA.cmap","1eerA_6.0_CA.cmap","1ewfA_6.0_CA.cmap",
							  "1bjfA_6.0_CA.cmap","1cc8A_6.0_CA.cmap","1d2vC_6.0_CA.cmap","1dosA_6.0_CA.cmap","1efcA_6.0_CA.cmap","1ex0A_6.0_CA.cmap",
							  "1bkf _6.0_CA.cmap","1cdoA_6.0_CA.cmap","1d3bB_6.0_CA.cmap","1dot _6.0_CA.cmap","1efpA_6.0_CA.cmap","1ex2A_6.0_CA.cmap",
							  "1bkjA_6.0_CA.cmap","1ce2A_6.0_CA.cmap","1d3vA_6.0_CA.cmap","1dpgA_6.0_CA.cmap","1efvB_6.0_CA.cmap","1ex4A_6.0_CA.cmap",
							  "1blxA_6.0_CA.cmap","1cf1A_6.0_CA.cmap","1d4aA_6.0_CA.cmap","1dpjA_6.0_CA.cmap","1eg3A_6.0_CA.cmap","1ex7A_6.0_CA.cmap",
							  "1bn6A_6.0_CA.cmap","1cf3A_6.0_CA.cmap","1d4dA_6.0_CA.cmap","1dptA_6.0_CA.cmap","1eg7A_6.0_CA.cmap","1ex9A_6.0_CA.cmap",
							  "1bo1A_6.0_CA.cmap","1cfe _6.0_CA.cmap","1d4tA_6.0_CA.cmap","1dqeA_6.0_CA.cmap","1egdA_6.0_CA.cmap","1exbA_6.0_CA.cmap",
							  "1bob _6.0_CA.cmap","1cfyA_6.0_CA.cmap","1d4xA_6.0_CA.cmap","1dquA_6.0_CA.cmap","1ei6A_6.0_CA.cmap","1exrA_6.0_CA.cmap",
							  "1bqqT_6.0_CA.cmap","1cg7A_6.0_CA.cmap","1d5gA_6.0_CA.cmap","1dqvA_6.0_CA.cmap","1ei9A_6.0_CA.cmap","1eybA_6.0_CA.cmap",
							  "1bqv _6.0_CA.cmap","1ci0A_6.0_CA.cmap","1d5rA_6.0_CA.cmap","1dqwA_6.0_CA.cmap","1ejbA_6.0_CA.cmap","1ezfA_6.0_CA.cmap",
							  "1bruP_6.0_CA.cmap","1ci4A_6.0_CA.cmap","1d5tA_6.0_CA.cmap","1dt9A_6.0_CA.cmap","1ejiA_6.0_CA.cmap","1ezvA_6.0_CA.cmap",
							  "1bs0A_6.0_CA.cmap","1cipA_6.0_CA.cmap","1d7kA_6.0_CA.cmap","1duxC_6.0_CA.cmap","1ejkA_6.0_CA.cmap","1ezvB_6.0_CA.cmap",
							  "1bt0A_6.0_CA.cmap","1civA_6.0_CA.cmap","1d7uA_6.0_CA.cmap","1dwuA_6.0_CA.cmap","1ek6A_6.0_CA.cmap","1ezvC_6.0_CA.cmap",
							  "1bu6O_6.0_CA.cmap","1cjcA_6.0_CA.cmap","1d8uA_6.0_CA.cmap","1dx4A_6.0_CA.cmap","1ekbB_6.0_CA.cmap","1ezvE_6.0_CA.cmap",
							  "1bu7A_6.0_CA.cmap","1cjwA_6.0_CA.cmap","1dar _6.0_CA.cmap","1dytA_6.0_CA.cmap","1ekjA_6.0_CA.cmap","1ezvF_6.0_CA.cmap",
							  "1bucA_6.0_CA.cmap","1cjyA_6.0_CA.cmap","1db3A_6.0_CA.cmap","1dywA_6.0_CA.cmap","1ekrA_6.0_CA.cmap","1ezvG_6.0_CA.cmap",
							  "1bw0A_6.0_CA.cmap","1cksA_6.0_CA.cmap","1dceA_6.0_CA.cmap","1dzfA_6.0_CA.cmap","1el5A_6.0_CA.cmap","1ezvI_6.0_CA.cmap",
							  "1bx2B_6.0_CA.cmap","1cmxA_6.0_CA.cmap","1dceB_6.0_CA.cmap","1e0cA_6.0_CA.cmap","1elpA_6.0_CA.cmap","1f0jA_6.0_CA.cmap",
							  "1bx4A_6.0_CA.cmap","1cp2A_6.0_CA.cmap","1dciA_6.0_CA.cmap","1e0sA_6.0_CA.cmap","1eluA_6.0_CA.cmap","1f0yA_6.0_CA.cmap",
							  "1bxsA_6.0_CA.cmap","1cpy _6.0_CA.cmap","1ddjA_6.0_CA.cmap","1e1oA_6.0_CA.cmap","1emsA_6.0_CA.cmap","1f16A_6.0_CA.cmap",
							  "1by8A_6.0_CA.cmap","1cqxA_6.0_CA.cmap","1ddqC_6.0_CA.cmap","1e2cA_6.0_CA.cmap","1eovA_6.0_CA.cmap","1f1jA_6.0_CA.cmap",
							  "1bzyA_6.0_CA.cmap","1cqzA_6.0_CA.cmap","1de4C_6.0_CA.cmap","1e3mA_6.0_CA.cmap","1ep7A_6.0_CA.cmap","1f1sA_6.0_CA.cmap",
							  "1c02A_6.0_CA.cmap","1cs1A_6.0_CA.cmap","1deuA_6.0_CA.cmap","1e58A_6.0_CA.cmap","1epfA_6.0_CA.cmap","1f1uA_6.0_CA.cmap",
							  "1c0aA_6.0_CA.cmap","1cs6A_6.0_CA.cmap","1df0A_6.0_CA.cmap","1e5eA_6.0_CA.cmap","1epuA_6.0_CA.cmap","1f20A_6.0_CA.cmap",
							  "1c3cA_6.0_CA.cmap","1cs8A_6.0_CA.cmap","1dfcA_6.0_CA.cmap","1e79I_6.0_CA.cmap","1eq6A_6.0_CA.cmap","1f28A_6.0_CA.cmap",
							  "1c3pA_6.0_CA.cmap","1csn _6.0_CA.cmap","1dg9A_6.0_CA.cmap","1e7kA_6.0_CA.cmap","1eq9A_6.0_CA.cmap","1f2eA_6.0_CA.cmap",
							  "1c4xA_6.0_CA.cmap","1ct5A_6.0_CA.cmap","1dgnA_6.0_CA.cmap","1e7uA_6.0_CA.cmap","1erv _6.0_CA.cmap","1f35A_6.0_CA.cmap",
							  "1c4zD_6.0_CA.cmap","1ct9A_6.0_CA.cmap","1diiA_6.0_CA.cmap","1e7wA_6.0_CA.cmap","1es9A_6.0_CA.cmap","1f3lA_6.0_CA.cmap",
							  "1c7cA_6.0_CA.cmap","1ctqA_6.0_CA.cmap","1din _6.0_CA.cmap","1e8aA_6.0_CA.cmap","1etrH_6.0_CA.cmap",
							  "1c8kA_6.0_CA.cmap","1cvjA_6.0_CA.cmap","1dj0A_6.0_CA.cmap","1e9gA_6.0_CA.cmap","1etzH_6.0_CA.cmap",
							  "1c8uA_6.0_CA.cmap","1cynA_6.0_CA.cmap","1djxA_6.0_CA.cmap","1e9iA_6.0_CA.cmap","1eucA_6.0_CA.cmap"};

		vector<string> fnames(filenames, filenames + sizeof(filenames)/sizeof(filenames[0]));
		vector<DecoyContactStructure*> structs;
		ifstream fin;
		int protein_length = 300;
		int min_contacts = 200;
		int n_structs = 46;
		for (int i=0; i<fnames.size(); i++) {
			DecoyContactStructure* cstruct = new DecoyContactStructure();
			string filename = "test/data/contact_maps/"+fnames[i];
			fin.open(filename.c_str());
			TEST_ASSERT( fin.good() );
			if (!fin.good()) // if we can't read the contact maps, bail out
				return;
			cstruct->read(fin);
			fin.close();
			int maxres = cstruct->getMaxResidueNumber();
			int mincont = cstruct->getContacts().size();
			//cout << i << tab << maxres << tab << mincont << endl << flush;
			if (maxres < protein_length && mincont > min_contacts) {
				structs.push_back(cstruct);
				//cout << structs.size() << tab << filename << tab << maxres << tab << mincont << endl << flush;
			}
			else {
				//cout << "deleting" << endl;
				delete cstruct;
			}
		}
		cout << flush;

		double log_nconf = 160.0*log(10.0);
		ProteinFolder* folder = new DecoyContactFolder(protein_length, log_nconf, structs, structs[0]);
		int num_to_fold = 100;
		for (int j=0; j<num_to_fold; j++) {
			Gene g = Gene::createRandomNoStops(protein_length*3);
			Protein p = g.translate();
			FoldInfo fi = folder->fold(p);
			TEST_ASSERT(fi.getStructure()>-1);
			//cout << "folded:" << tab << fi.getStructure() << tab << fi.getFreeEnergy() << endl;
		}
		// Clean up
		delete folder;
		for (int i=0; i<structs.size(); i++) {
			delete structs[i];
		}

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
		const char* filenames[] = {"1qhwA_6_CB.cmap","1dmhA_6_CB.cmap","1hm4A_6_CB.cmap","1mj5A_6_CB.cmap","1pa1A_6_CB.cmap","1tdb _6_CB.cmap",
								 "16vpA_6_CB.cmap","1dmwA_6_CB.cmap","1hynP_6_CB.cmap","1mkdA_6_CB.cmap","1pp9A_6_CB.cmap","1tux _6_CB.cmap",
								 "1b65A_6_CB.cmap","1dtdA_6_CB.cmap","1i9zA_6_CB.cmap","1msvA_6_CB.cmap","1uby _6_CB.cmap","12asA_6_CB.cmap",
								 "1b7bA_6_CB.cmap","1eg1A_6_CB.cmap","1idm _6_CB.cmap","1nl4A_6_CB.cmap","1qoyA_6_CB.cmap","1uf5A_6_CB.cmap",
								 "1bg2 _6_CB.cmap","1g9nG_6_CB.cmap","1io7A_6_CB.cmap","1ofzA_6_CB.cmap","1rozA_6_CB.cmap","2bbvA_6_CB.cmap",
								 "1bt3A_6_CB.cmap","1ghqA_6_CB.cmap","1jfpA_6_CB.cmap","1ogqA_6_CB.cmap","1sdeA_6_CB.cmap","3por _6_CB.cmap",
								 "1bwdA_6_CB.cmap","1gq8A_6_CB.cmap","1lhpA_6_CB.cmap","1oquA_6_CB.cmap","1sidA_6_CB.cmap","3pvaA_6_CB.cmap",
								 "1by8A_6_CB.cmap","1gxrA_6_CB.cmap","1m50A_6_CB.cmap","1p8rA_6_CB.cmap","1sig _6_CB.cmap"};
		vector<string> fnames(filenames, filenames + sizeof(filenames)/sizeof(filenames[0]));
		vector<DecoyContactStructure*> structs;
		DecoyContactStructure* target_struct;
		ifstream fin;
		int protein_length = 300;
		for (int i=0; i<fnames.size(); i++) {
			DecoyContactStructure* cstruct = new DecoyContactStructure();
			string filename = "test/data/williams_contact_maps/"+fnames[i];
			fin.open(filename.c_str());
			TEST_ASSERT( fin.good() );
			if (!fin.good()) // if we can't read the contact maps, bail out
				return;
			cstruct->read(fin);
			fin.close();
			if (i==0) {
				target_struct = cstruct;
			}
			else {
				structs.push_back(cstruct);
				cout << filename << endl;
			}
		}
		cout << "loaded " << structs.size() << " contact sets" << endl << flush;
		cout << "length = " << native_1qhw_seq.size() << endl;
		double log_nconf = 160.0*log(10.0);
		ProteinFolder* folder = new DecoyContactFolder(protein_length, log_nconf, structs, target_struct);
		Protein p(native_1qhw_seq);
		FoldInfo fi = folder->fold(p);
		//TEST_ASSERT(fi.getStructure()>-1);
		cout << "Williams:" << endl;
		cout << "folded:" << tab << fi.getStructure() << tab << fi.getFreeEnergy() << endl;
		// Clean up
		delete folder;
		for (int i=0; i<structs.size(); i++) {
			delete structs[i];
		}
	}
};


#endif // _T_FOLDER_H__
