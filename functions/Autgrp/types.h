/* some definitions of constants */

/* the maximal entry in the short vectors should not exceed MAXENTRY to
   avoid overflow */
#define	MAXENTRY	10000
/* it is checked that the product of the maximal entry in the short vectors
   and the maximal entry in the products of Gram-matrices with short vectors
   does not exceed MAXNORM, since the program works just with int's */
#define	MAXNORM		100000000
#define	STRLEN		80
/* since the LLL-reduction works with a floating-point model, there might be
   some rounding error, which should not reach EPS */
#define	EPS		0.001
/* the constant for the LLL-condition, which should be < 1, but 0.75 is the
   standard value, a higher value might yield a slightly better result but
   increases the running time */
#define	LLL_CONST	0.75
#define DEF_PRIME	16001


/* structure to hold the generators of the group according to the stabilizer
   chain */
typedef struct {
	int	dim;
	int	*ord;
	int	*ng;
	int	*nsg;
	int	****g;
	} group;

/* structure for a list of vectors */
typedef struct {
	int	dim;
	int	len;
	int	prime;
	int	n;
	int	**v;
	} veclist;

/* structure for the invariant forms and their products with the list of
   short vectors */
typedef struct {
	int	dim;
	int	n;
	int	***A;
	int	***v;
	} invar;

/* structure for the diagonal of the fingerprint, the new order and the indices
   of the standard-base in the list of short vectors */
typedef struct {
	int	*diag;
	int	*per;
	int	*e;
	} fpstruct;

/* structure for the scalar product combinations, the transformation matrices
   between the vector sums and that lattice bases and the scalar products of
   the lattice bases */
typedef struct {
	veclist	list;
	int	rank;
	int	**trans;
	int	**coef;
	int	***F;
	} scpcomb;

/* structure for a Bacher-polynomial */
typedef struct {
	int	mind;
	int	maxd;
	int	sum;
	int	*coef;
	} bachpol;

/* structure for the option flags */
typedef struct {
	int	DEPTH;
	int	STAB;
	int	BACH[3];
	int	BACHDEP;
	int	BACHSCP;
	int	GEN;
	int	PRINT;
	} flagstruct;

/* functions in auttools.c */
static	int	cand(int *CI, int I, int *x, veclist V, invar F, fpstruct fp, scpcomb *comb, bachpol *bach, flagstruct flags);
static	void	autom(group *G, veclist V, invar F, fpstruct fp, scpcomb *comb, bachpol *bach, flagstruct flags);
static	int	aut(int step, int *x, int **C, group *G, veclist V, invar F, fpstruct fp, scpcomb *comb, bachpol *bach, flagstruct flags);

/* functions in bachtools.c */
static	void	bacher(bachpol *pol, int I, int S, veclist V, int **Fv);
static	int	bachcomp(bachpol pol, int I, int S, veclist V, int **Fv);
static	void	fputbach(FILE *outfile, bachpol pol);

/* functions in iotools.c */
static  void	getflags(flagstruct *fl, int *options);
static	bravais_TYP 	*putgens(group G, flagstruct flags);
static	void	putord(group G, flagstruct flags, bravais_TYP *B);
static	matrix_TYP	*putiso(int **X, flagstruct flags, int dim);

/* functions in isotools.c */
static	int	isocand(int *CI, int I, int *x, veclist V, invar F, invar FF, fpstruct fp, scpcomb *comb, bachpol *bach, flagstruct flags);
static	matrix_TYP 	*bs_isometry(veclist V, invar F, invar FF, fpstruct fp, int ***G, int nG, scpcomb *comb, bachpol *bach, flagstruct flags);
static	int	iso(int step, int *x, int **C, veclist V, invar F, invar FF, fpstruct fp, int ***G, int nG, scpcomb *comb, bachpol *bach, flagstruct flags);
static	int	isostab(int ****H, int pt, int ***G, int nG, veclist V, int Maxfail);

/* functions in lattools.c */
static	int	lll(int **F, int **T, int dim);
static	void	initialize(int i, double **mu, double *B, int **gram);
static	int	red(int *m, int *l, int **gram, int **T, double **mu, double *B, int dim, int *rank);
static	void	check(double *B, int *m, int *l, int **gram, int **T, double **mu, int dim, int *rank);
static	void	decrease(int *m, int *l, int **gram, double **mu, double *B, int *rank);
static	void	interchange(int *m, int *l, int **gram, int **T, double **mu, double *B, int dim);
static	int	iround(double x);

/* functions in mattools.c */
static	void	vecmatmul(int *x, int **A, int n, int *y);
static	void	matmul(int **A, int **B, int n, int **C);
static	int	scp(int *x, int **F, int *y, int n);
static	int	sscp(int *x, int *y, int n);
static	void	psolve(int **X, int **A, int **B, int n, int p);
static	void	pgauss(int r, int **A, int **B, int n, int p);
static	int	isprime(int n);

/* functions in orbtools.c */
static int	operate(int nr, int **A, veclist V);
static int	orbit(int *pt, int npt, int ***G, int nG, veclist V, int **orb);
static int	orbitlen(int pt, int orblen, int ***G, int nG, veclist V);
static int	delete(int *orb1, int l1, int *orb2, int l2);
static void	stab(int I, group *G, fpstruct fp, veclist V);
static void	matgen(int *x, int **X, int dim, int *per, int **v);
static void	stabil(int **S, int *x1, int *x2, int *per, int **G, veclist V);

/* functions in preproc.c */
static void	checkvecs(veclist *V, invar F, veclist norm);
static int	checkgen(int **g, invar F);
static void	fingerprint(fpstruct *fp, invar F, veclist V);
static int	possible(invar F, veclist V, int *per, int I, int J);
static void	scpvector(int *scpvec, int *w, int *b, int I, int dep, invar F);
static void	scpvecs(veclist *list, int ***vec, int I, int *b, int dep, veclist V, invar F);
static void	base(scpcomb *com, int ***b, int **v, int **F, int dim);
static void	coef(scpcomb *com, int **b, int **v, int **F, int dim);
static void	scpforms(scpcomb *com, int **b, invar F);

/* functions in sorttools.c */
static	int	comp(int *x, int *y, int n);
static	int	numberof(int *vec, veclist V);
static	void	sortvecs(veclist *V);
static	void	quicksort(int **v, int inf, int sup, int dim);

/* functions in perfecttools.c */
static int normal_aut_test(int *x, int I, veclist V);
