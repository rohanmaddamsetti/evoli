/*
This file is part of the evoli project.
Copyright (C) 2006 Allan Drummond <dadrummond@gmail.com>,
                               Claus O. Wilke <cwilke@mail.utexas.edu>

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


#include <Python.h> // needs to be first include
#include <iostream>
#include "genetic-code.hh"
#include "codon.hh"

static PyObject *CodonErrorObject;

/* ----------------------------------------------------- */


bool codonValid( const char* codon )
{
	bool valid = true;
	const char* accept_chars = "aAgGtTuUcC"; // we only allow these characters in codons
	for ( int i=0; i<3; i++ )
	{
		bool found = false;
		for ( int j=0; j<10; j++ )
		{
			if ( codon[i]==accept_chars[j] )
			{
				found = true;
				break;
			}
		}
		if ( !found )
		{
			valid = false;
			break;
		}
	}
	return valid;
}

static char codon_codonValid__doc__[] =
"Tests whether a codon is valid (i.e., contains exactly three letters, composed of the letters a, A, c, C, u, U, t, T, g, G) or not."
;

static PyObject *
codon_codonValid(PyObject *self /* Not used */, PyObject *args)
{
	const char *codon;
	int size;
	if ( !PyArg_ParseTuple( args, "s#", &codon, &size ) ) {
		return NULL;
	}

	int valid = 1;
	if ( size != 3 ) 
		valid = 0;
	else
		if ( !codonValid( codon ) )
			valid = 0;
	
	return Py_BuildValue( "i", valid );
}

static char codon_calcDnDs__doc__[] =
"Calculates the number of nonsynonymous and synonymous substitutions between two codons, and returns them in a list."
;

static PyObject *
codon_calcDnDs(PyObject *self /* Not used */, PyObject *args)
{
	const char *codon1;
	const char *codon2;
	int size1, size2;
	if ( !PyArg_ParseTuple( args, "s#s#", &codon1, &size1, &codon2, &size2 ) ) {
		return NULL;
	}

	if ( size1 != 3 || size2 != 3 ) 
	{
		PyErr_SetString(CodonErrorObject, "Each codon must be exactly three characters long.");
		return NULL;
	}

	if ( !codonValid( codon1 ) || !codonValid( codon2 ) )
	{
		PyErr_SetString(CodonErrorObject, "Invalid codon. Codons can contain only the letters a, A, u, U, t, T, g, G, c, C.");
		return NULL;
	}
	
	Codon c1(codon1);
	Codon c2(codon2);

	// calculate dn and ds and store in variables
	pair<double,double> dnds = GeneticCodeUtil::calcDnDs( c1, c2 );

	return Py_BuildValue( "dd", dnds.first, dnds.second );
}

static char codon_calcNS__doc__[] =
"Calculates the number of nonsynonymous and synonymous sites in a codon. Counts stop sites as nonsynonymous sites."
;

static PyObject *
codon_calcNS(PyObject *self /* Not used */, PyObject *args)
{
	const char *codon1;
	int size1;
	if ( !PyArg_ParseTuple( args, "s#", &codon1, &size1 ) ) {
		return NULL;
	}

	if ( size1 != 3 ) 
	{
		PyErr_SetString(CodonErrorObject, "Codon must be exactly three characters long.");
		return NULL;
	}

	if ( !codonValid( codon1 ) )
	{
		PyErr_SetString(CodonErrorObject, "Invalid codon. Codons can contain only the letters a, A, u, U, t, T, g, G, c, C.");
		return NULL;
	}

	Codon c1(codon1);

	// calculate N and S
	double S = GeneticCodeUtil::calcSynonymousSites( c1 );
	double N = 3-S;

	return Py_BuildValue( "dd", N, S );
}

static char codon_calcNSMutatOpport__doc__[] =
"Calculates the number of nonsynonymous and synonymous sites in a codon according to the mutational opportunity definition. Does not count stop sites as nonsynonymous sites. Parameters are the codon and a float giving the ration of transitions to transversions."
;

static PyObject *
codon_calcNSMutatOpport(PyObject *self /* Not used */, PyObject *args)
{
	const char *codon1;
	int size1;
	double rho;
	if ( !PyArg_ParseTuple( args, "s#d", &codon1, &size1, &rho ) ) {
		return NULL;
	}

	if ( size1 != 3 ) 
	{
		PyErr_SetString(CodonErrorObject, "Codon must be exactly three characters long.");
		return NULL;
	}

	if ( !codonValid( codon1 ) )
	{
		PyErr_SetString(CodonErrorObject, "Invalid codon. Codons can contain only the letters a, A, u, U, t, T, g, G, c, C.");
		return NULL;
	}
	
	Codon c1(codon1);

	// calculate N and S
	double S = GeneticCodeUtil::calcSynMutationOpportunity( c1, rho );
	double N = GeneticCodeUtil::calcNonsynMutationOpportunity( c1, rho );

	return Py_BuildValue( "dd", N, S );
}


static char codon_calcDnDsMutatOpport__doc__[] =
"Calculates the number of nonsynonymous and synonymous substitutions between two codons according to the mutational opportunity definition, and returns them in a list. Does not count stop sites as nonsynonymous sites. Parameters are the two codons and a float giving the ration of transitions to transversions.\nWarning: The underlying C++ function that does all the heavy lifting hasn't been tested extensively, and may contain bugs (though none are known)."
;

static PyObject *
codon_calcDnDsMutatOpport(PyObject *self /* Not used */, PyObject *args)
{
	const char *codon1;
	const char *codon2;
	int size1, size2;
	double rho;
	if ( !PyArg_ParseTuple( args, "s#s#d", &codon1, &size1, &codon2, &size2, &rho ) ) {
		return NULL;
	}

	if ( size1 != 3 || size2 != 3 ) 
	{
		PyErr_SetString(CodonErrorObject, "Each codon must be exactly three characters long.");
		return NULL;
	}

	if ( !codonValid( codon1 ) || !codonValid( codon2 ) )
	{
		PyErr_SetString(CodonErrorObject, "Invalid codon. Codons can contain only the letters a, A, u, U, t, T, g, G, c, C.");
		return NULL;
	}
	
	Codon c1(codon1);
	Codon c2(codon2);

	// calculate dn and ds and store in variables
	pair<double, double> p = GeneticCodeUtil::calcDnDsWeighted( c1, c2, rho );

	return Py_BuildValue( "dd", p.first, p.second );
}

/* List of methods defined in the module */

static struct PyMethodDef codon_methods[] = {
	{"codonValid",	(PyCFunction)codon_codonValid,	METH_VARARGS, codon_codonValid__doc__},
	{"calcDnDs",	(PyCFunction)codon_calcDnDs,	METH_VARARGS, codon_calcDnDs__doc__},
	{"calcNS",	(PyCFunction)codon_calcNS,	METH_VARARGS, codon_calcNS__doc__},
	{"calcNSMutatOpport",	(PyCFunction)codon_calcNSMutatOpport,	METH_VARARGS, codon_calcNSMutatOpport__doc__},
	{"calcDnDsMutatOpport",	(PyCFunction)codon_calcDnDsMutatOpport,	METH_VARARGS, codon_calcDnDsMutatOpport__doc__},
	{NULL,	 (PyCFunction)NULL, 0, NULL}		/* sentinel */
};


/* Initialization function for the module (*must* be called initfolder) */

static char codon_module_documentation[] = 
""
;

PyMODINIT_FUNC
initcodon(void)
{
	PyObject *m, *d;

	/* Create the module and add the functions */
	m = Py_InitModule4("codon", codon_methods,
		codon_module_documentation,
		(PyObject*)NULL,PYTHON_API_VERSION);

	/* Add some symbolic constants to the module */
	d = PyModule_GetDict(m);
	CodonErrorObject = PyString_FromString("codon.error");
	PyDict_SetItemString(d, "error", CodonErrorObject);

	/* XXXX Add constants here */
	
	/* Check for errors */
	if (PyErr_Occurred())
		Py_FatalError("can't initialize module codon");
}

