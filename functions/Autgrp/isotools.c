/*****	This file includes those routines for the isometry program which
	differ from the routines for the automorphism program	*****/
#include "typedef.h"
#include "types.h"


/*****************************************************************\
|	tests, whether x[0],...,x[I-1] is a partial 
|	isometry, using scalar product combinations
|	and Bacher-polynomials depending on the chosen 
|	options, puts the candidates for x[I] (i.e. the
|	vectors vec such that the scalar products of 
|	(x[0],...,x[I-1],vec) are correct) on CI, 
|	returns their number 
|	(should be fp.diag[I])
\*****************************************************************/
int 
isocand (int *CI, int I, int *x, veclist V, invar F, invar FF, fpstruct fp, scpcomb *comb, bachpol *bach, flagstruct flags)
{
	int	i, j, k, dim, okp, okm, sign, nr, fail, num;
	int	*vec, *scpvec, **xvec, **xbase, **Fxbase, DEP, len, rank, n;
	int	*Vvj, *FAiI, **Fvi, *xnum, xk, *comtri, *comcoi, xbij, vj;
	scpcomb	com;
	int	test;

	dim = F.dim;
	DEP = flags.DEPTH;
	len = F.n * DEP;

        if(normal_option == TRUE && I >0)
        {
           test = normal_aut_test(x, I-1, V);
           if(test == FALSE)
             return(0);
        }
	if (I-1 >= 0  &&  I-1 < flags.BACHDEP)
	{
		if (flags.BACH[2] == 0)
			flags.BACHSCP = V.v[abs(x[I-1])][dim] / 2;
		if (bachcomp(bach[I-1], x[I-1], flags.BACHSCP, V, FF.v[0]) == 0)
			return(0);
	}
	if ((vec = (int*)malloc(dim * sizeof(int))) == 0)
		exit (2);
	if ((scpvec = (int*)malloc(len * sizeof(int))) == 0)
		exit (2);
	if (I-1 >= 0  &&  DEP > 0)
	{
		com = comb[I-1];
		rank = com.rank;
		n = com.list.n;
/* xvec is the list of vector sums which are computed with respect to the 
   partial basis in x */
		if ((xvec = (int**)malloc((n+1) * sizeof(int*))) == 0)
			exit (2);
		for (i = 0; i <= n; ++i)
		{
			if ((xvec[i] = (int*)malloc(dim * sizeof(int))) == 0)
				exit (2);
			for (j = 0; j < dim; ++j)
				xvec[i][j] = 0;
		}
/* xbase should be a basis for the lattice generated by the vectors in xvec,
   it is obtained via the transformation matrix comb[I-1].trans */
		if ((xbase = (int**)malloc(rank * sizeof(int*))) == 0)
			exit (2);
		for (i = 0; i < rank; ++i)
		{
			if ((xbase[i] = (int*)malloc(dim * sizeof(int))) == 0)
				exit (2);
		}
/* Fxbase is the product of a form F with the base xbase */
		if ((Fxbase = (int**)malloc(rank * sizeof(int*))) == 0)
			exit (2);
		for (i = 0; i < rank; ++i)
		{
			if ((Fxbase[i] = (int*)malloc(dim * sizeof(int))) == 0)
				exit (2);
		}
	}
	else
	{
	    memset(&com, 0, sizeof(com));
	    xvec = 0;
	    xbase = 0;
	    Fxbase = 0;
	    rank = 0;
	    n = 0;
	}
/* CI is the list for the candidates */
	for (i = 0; i < fp.diag[I]; ++i)
		CI[i] = 0;
	nr = 0;
	fail = 0;
	for (j = 1; j <= V.n  &&  fail == 0; ++j)
	{
		Vvj = V.v[j];
		okp = 0;
		okm = 0;
		for (k = 0; k < len; ++k)
			scpvec[k] = 0;
		for (i = 0; i < F.n; ++i)
		{
			FAiI = F.A[i][fp.per[I]];
			Fvi = FF.v[i];
/* vec is the vector of scalar products of V.v[j] with the first I base vectors
   x[0]...x[I-1] with respect to the forms in FF */
			for (k = 0; k < I; ++k)
			{
				if ((xk = x[k]) > 0)
					vec[k] = sscp(Vvj, Fvi[xk], dim);
				else
					vec[k] = -sscp(Vvj, Fvi[-xk], dim);
			}
			for (k = 0; k < I  &&  vec[k] == FAiI[fp.per[k]]; ++k);
			if (k == I  &&  Vvj[dim+i] == FAiI[fp.per[I]])
/* V.v[j] is a candidate for x[I] with respect to the form F.A[i] */
				++okp;
			for (k = 0; k < I  &&  vec[k] == -FAiI[fp.per[k]]; ++k);
			if (k == I  &&  Vvj[dim+i] == FAiI[fp.per[I]])
/* -V.v[j] is a candidate for x[I] with respect to the form F.A[i] */
				++okm;
			if (I-1 >= 0  &&  DEP > 0)
			{
				for (k = I-1; k >= 0  &&  k > I-1-DEP; --k)
					scpvec[i*DEP+I-1-k] = vec[k];
			}
		}
		if (I-1 >= 0  &&  DEP > 0)
/* check, whether the scalar product combination scpvec is contained in the
   list comb[I-1].list */
		{
			for (i = 0; i < len  &&  scpvec[i] == 0; ++i);
			if (i == len)
				num = 0;
			else
			{
				num = numberof(scpvec, com.list);
				sign = num > 0 ? 1 : -1;
				num *= sign;
			}
			if (num > n)
/* scpvec is not found, hence x[0]...x[I-1] is not a partial isometry */
				fail = 1;
			else if (num > 0)
/* scpvec is found and the vector is added to the corresponding vector sum */
			{
				xnum = xvec[num];
				for (k = 0; k < dim; ++k)
					xnum[k] += sign * Vvj[k];
			}
		}
		if (okp == F.n)
/* V.v[j] is a candidate for x[I] */
		{
			if (nr < fp.diag[I])
			{
				CI[nr] = j;
				++nr;
			}
			else
/* there are too many candidates */
				fail = 1;
		}
		if (okm == F.n)
/* -V.v[j] is a candidate for x[I] */
		{
			if (nr < fp.diag[I])
			{
				CI[nr] = -j;
				++nr;
			}
			else
/* there are too many candidates */
				fail = 1;
		}
	}
	if (fail == 1)
		nr = 0;
	if (nr == fp.diag[I]  &&  I-1 >= 0  &&  DEP > 0)
/* compute the basis of the lattice generated by the vectors in xvec via the
   transformation matrix comb[I-1].trans */
	{
		for (i = 0; i < rank; ++i)
		{
			comtri = com.trans[i];
			for (j = 0; j < dim; ++j)
			{
				xbij = 0;
				for (k = 0; k <= n; ++k)
					xbij += comtri[k] * xvec[k][j];
				xbase[i][j] = xbij;
			}
		}
	}
	if (nr == fp.diag[I]  &&  I-1 >= 0  &&  DEP > 0)
/* check, whether the base xbase has the right scalar products */
	{
		for (i = 0; i < F.n  &&  nr > 0; ++i)
		{
			for (j = 0; j < rank; ++j)
			{
				for (k = 0; k < dim; ++k)
					Fxbase[j][k] = sscp(FF.A[i][k], xbase[j], dim);
			}
			for (j = 0; j < rank  &&  nr > 0; ++j)
			{
				for (k = 0; k <= j  &&  nr > 0; ++k)
				{
					if (sscp(xbase[j], Fxbase[k], dim) != com.F[i][j][k])
/* a scalar product is wrong */
						nr = 0;
				}
			}
		}
	}
	if (nr == fp.diag[I]  &&  I-1 >= 0  &&  DEP > 0)
/* check, whether comb[I-1].coef * xbase = xvec */
	{
		for (i = 0; i <= n  &&  nr > 0; ++i)
		{
			comcoi = com.coef[i];
			for (j = 0; j < dim; ++j)
			{
				vj = 0;
				for (k = 0; k < rank; ++k)
					vj += comcoi[k] * xbase[k][j];
				if (vj != xvec[i][j])
/* an entry is wrong */
					nr = 0;
			}
		}
	}
	if (I-1 >= 0  &&  DEP > 0)
	{
		for (i = 0; i <= n; ++i)
			free(xvec[i]);
		free(xvec);
		for (i = 0; i < rank; ++i)
		{
			free(xbase[i]);
			free(Fxbase[i]);
		}
		free(xbase);
		free(Fxbase);
	}
	free(scpvec);
	free(vec);
	return(nr);
}

/*****************************************************\
|	search for an isometry
\*****************************************************/
matrix_TYP *
bs_isometry (veclist V, invar F, invar FF, fpstruct fp, int ***G, int nG, scpcomb *comb, bachpol *bach, flagstruct flags)
{
	int	i, dim, *x, **C, **X, found;
	matrix_TYP *erg;

	dim = F.dim;
	if ((C = (int**)malloc(dim * sizeof(int*))) == 0)
		exit (2);
/* C[i] is the list of candidates for the image of the i-th base-vector */
	for (i = 0; i < dim; ++i)
	{
		if ((C[i] = (int*)malloc(fp.diag[i] * sizeof(int))) == 0)
			exit (2);
	}
/* x is the new base, i.e. x[i] is the index in V.v of the i-th base-vector */
	if ((x = (int*)malloc(dim * sizeof(int))) == 0)
		exit (2);
/* compute the candidates for x[0] */
	isocand(C[0], 0, x, V, F, FF, fp, comb, bach, flags);
	found = 0;
/* go into the recursion */
	found = iso(0, x, C, V, F, FF, fp, G, nG, comb, bach, flags);
/***********************************
        for(i=0;i<nG;i++)
        {
           for(j=0;j<dim;j++)
             free(G[i][j]);
           free(G[i]);
        }
*****************************/
	if (found == 1)
	{
		if ((X = (int**)malloc(dim * sizeof(int*))) == 0)
			exit (2);
		for (i = 0; i < dim; ++i)
		{
			if ((X[i] = (int*)malloc(dim * sizeof(int))) == 0)
				exit (2);
		}
		matgen(x, X, dim, fp.per, V.v);
/* print the isometry */
		erg = putiso(X, dim);
		for (i = 0; i < dim; ++i)
			free(X[i]);
		free(X);
	}
	else
            erg = NULL;
	for (i = 0; i < dim; ++i)
		free(C[i]);
	free(C);
	free(x);
	return(erg);
}

/**********************************************************\
|	the heart of the program: the recursion
\**********************************************************/
int 
iso (int step, int *x, int **C, veclist V, invar F, invar FF, fpstruct fp, int ***G, int nG, scpcomb *comb, bachpol *bach, flagstruct flags)
{
	FILE	*outfile;
	int	i, j, dim, ***H, nH, *orb, orblen, found, Maxfail;

	dim = F.dim;
	found = 0;
	while (C[step][0] != 0  &&  found == 0)
	{
		if (step < dim-1)
		{
/* choose the image of the base-vector nr. step */
			x[step] = C[step][0];
/* check, whether x[0]...x[step] is a partial automorphism and compute the
   candidates for x[step+1] */
			if (isocand(C[step+1], step+1, x, V, F, FF, fp, comb, bach, flags) == fp.diag[step+1])
			{
/* go deeper into the recursion */
				Maxfail = 0;
/* determine the heuristic value of Maxfail for the break condition in 
   isostab */
				for (i = 0; i <= step; ++i)
				{
					if (fp.diag[i] > 1)
						Maxfail += 1;
				}
				for (i = step+1; i < dim; ++i)
				{
					if (fp.diag[i] > 1)
						Maxfail += 2;
				}
/* compute the stabilizer H of x[step] in G */
				nH = isostab(&H, x[step], G, nG, V, Maxfail);
				found = iso(step+1, x, C, V, F, FF, fp, H, nH, comb, bach, flags);
                                /* inserted 16/1/97 tilman */
                                free(H);

			}
			if (found == 1)
			{
				for (i = 0; i < nG; ++i)
				{
					for (j = 0; j < dim; ++j)
						free(G[i][j]);
					free(G[i]);
				}
				return(found);
			}
			orblen = orbit(&(x[step]), 1, G, nG, V, &orb);
/* delete the orbit of the chosen vector from the list of candidates */
			delete(C[step], fp.diag[step], orb, orblen);
			free(orb);
			if (step == 0  &&  flags.PRINT == 1)
/* if the -P option is given, print on top level the number of excluded 
   candidates on ISOM.tmp */
			{
				outfile = fopen("ISOM.tmp", "a");
				fprintf(outfile, "excluded %d vectors on top level\n", orblen);
				fclose(outfile);
			}
		}
		else
		{
/* an isomorphism is found */
                    if(normal_option == TRUE)
                    {
                           found = 0;
                           for(i=0;i<fp.diag[step] && C[step][0] != 0 && found == 0;i++)
                           {
                              x[step] = C[step][0];
                              found = normal_aut_test(x, step, V);
                              if(found == 0)
                              {
                                 for(j=0;j<fp.diag[step]-1;j++)
                                  C[step][j] = C[step][j+1];
                                 C[step][fp.diag[step]-1] = 0;
                              }
                           }
                    }
                    else
                    {
			x[dim-1] = C[dim-1][0];
			found = 1;
                    }
		}
	}
	for (i = 0; i < nG; ++i)
	{
		for (j = 0; j < dim; ++j)
			free(G[i][j]);
		free(G[i]);
	}

	return(found);
}

/*********************************************************************\
|	computes the orbit of V.v[pt] under the generators 
|	G[0],...,G[nG-1] and elements stabilizing V.v[pt], which are 
|	stored in H, returns the number of generators in H
\*********************************************************************/
int 
isostab (int ****H, int pt, int ***G, int nG, veclist V, int Maxfail)
{
	int	*orb, len, cnd, orblen, tmplen, rpt;
	int	***w, *flag, **A, **B;
	int	i, j, k, dim, im, nH, fail;

/* a heuristic break condition for the computation of stabilizer elements:
   it would be too expensive to calculate all the stabilizer generators, which
   are obtained from the orbit, since this is highly redundant, 
   on the other hand every new generator which enlarges the group reduces the
   number of orbits and hence the number of candidates to be tested,
   after Maxfail subsequent stabilizer elements, that do not enlarge the group,
   the procedure stops,
   increasing Maxfail will possibly decrease the number of tests,
   but will increase the running time of the stabilizer computation
   there is no magic behind the heuristic, tuning might be appropriate */
	dim = V.dim;
/* H are the generators of the stabilizer of V.v[pt] */
	if ((*H = (int***)malloc(1 * sizeof(int**))) == 0)
		exit (2);
	if (((*H)[0] = (int**)malloc(dim * sizeof(int*))) == 0)
		exit (2);
	for (i = 0; i < dim; ++i)
	{
		if (((*H)[0][i] = (int*)malloc(dim * sizeof(int))) == 0)
			exit (2);
	}
	nH = 0;
/* w[i+V.n] is a matrix that maps V.v[pt] on V.v[i] */
	if ((w = (int***)malloc((2*V.n+1) * sizeof(int**))) == 0)
		exit (2);
/* orb contains the orbit of V.v[pt] */
	if ((orb = (int*)malloc(2*V.n * sizeof(int))) == 0)
		exit (2);
/* orblen is the length of the orbit of a random vector in V.v */
	orblen = 1;
	for (i = 0; i < 2*V.n; ++i)
		orb[i] = 0;
/* if flag[i+V.n] = 1, then the point i is already in the orbit */
	if ((flag = (int*)malloc((2*V.n+1) * sizeof(int))) == 0)
		exit (2);
	for (i = 0; i <= 2*V.n; ++i)
		flag[i] = 0;
	orb[0] = pt;
	flag[orb[0]+V.n] = 1;
/* w[pt+V.n] is the Identity */
	if ((w[orb[0]+V.n] = (int**)malloc(dim * sizeof(int*))) == 0)
		exit (2);
	for (i = 0; i < dim; ++i)
	{
		if ((w[orb[0]+V.n][i] = (int*)malloc(dim * sizeof(int))) == 0)
			exit (2);
		for (j = 0; j < dim; ++j)
			w[orb[0]+V.n][i][j] = 0;
		w[orb[0]+V.n][i][i] = 1;
	}
/* A and B are matrices for temporary use */
	if ((A = (int**)malloc(dim * sizeof(int*))) == 0)
		exit (2);
	for (i = 0; i < dim; ++i)
	{
		if ((A[i] = (int*)malloc(dim * sizeof(int))) == 0)
			exit (2);
	}
	if ((B = (int**)malloc(dim * sizeof(int*))) == 0)
		exit (2);
	for (i = 0; i < dim; ++i)
	{
		if ((B[i] = (int*)malloc(dim * sizeof(int))) == 0)
			exit (2);
	}
	cnd = 0;
	len = 1;
/* fail is the number of successive failures */
	fail = 0;
	while (len > cnd  &&  fail < Maxfail)
	{
		for (i = 0; i < nG  &&  fail < Maxfail; ++i)
		{
			im = operate(orb[cnd], G[i], V);
			if (flag[im+V.n] == 0)
/* a new element is found, appended to the orbit and an element mapping
   V.v[pt] to im is stored in w[im+V.n] */
			{
				orb[len] = im;
				flag[im+V.n] = 1;
				if ((w[im+V.n] = (int**)malloc(dim * sizeof(int*))) == 0)
					exit (2);
				for (j = 0; j < dim; ++j)
				{
					if ((w[im+V.n][j] = (int*)malloc(dim * sizeof(int))) == 0)
						exit (2);
				}
				matmul(w[orb[cnd]+V.n], G[i], dim, w[im+V.n]);
				++len;
			}
			else
/* the image was already in the orbit */
			{
				matmul(w[orb[cnd]+V.n], G[i], dim, B);
/* check, whether the old and the new element mapping pt on im differ */
				for (j = 0; j < dim  &&  comp(B[j], w[im+V.n][j], dim) == 0; ++j);
				if (j < dim)
				{
/* w[im+V.n] is copied to A, since psolve changes the matrices */
					for (j = 0; j < dim; ++j)
					{
						for (k = 0; k < dim; ++k)
							A[j][k] = w[im+V.n][j][k];
					}
/* new stabilizer element H[nH] = w[orb[cnd]+V.n] * G[i] * (w[im+V.n])^-1 */
					psolve((*H)[nH], A, B, dim, V.prime);
					rpt = rand() % V.n + 1;
					tmplen = orbitlen(rpt, 2*V.n, *H, nH+1, V);
					while (tmplen < orblen)
/* the orbit of this vector is shorter than a previous one, hence choose a new
   random vector */
					{
						rpt = rand() % V.n + 1;
						tmplen = orbitlen(rpt, 2*V.n, *H, nH+1, V);
					}
					if (tmplen > orblen)
/* the new stabilizer element H[nH] enlarges the group generated by H */
					{
						orblen = tmplen;
						++nH;
/* allocate memory for the new generator */
						if ((*H = (int***)realloc(*H, (nH+1) * sizeof(int**))) == 0)
							exit (2);
						if (((*H)[nH] = (int**)malloc(dim * sizeof(int*))) == 0)
							exit (2);
						for (k = 0; k < dim; ++k)
						{
							if (((*H)[nH][k] = (int*)malloc(dim * sizeof(int))) == 0)
								exit (2);
						}
						fail = 0;
					}
					else
/* the new stabilizer element does not enlarge the orbit of a random vector */
						++fail;
				}
/* if H[nH] is the identity, nothing is done */
			}
		}
		++cnd;
	}
	for (i = 0; i <= 2*V.n; ++i)
	{
		if (flag[i] == 1)
		{
			for (j = 0; j < dim; ++j)
				free(w[i][j]);
			free(w[i]);
		}
	}
	free(w);
	for (i = 0; i < dim; ++i)
	{
		free(A[i]);
		free(B[i]);
		free((*H)[nH][i]);
	}
	free(A);
	free(B);
	free((*H)[nH]);
	free(orb);
	free(flag);

	return(nH);
}
