#include "mex.h"
#include "blas.h"
#include "conv2d.h"

#define DEBUG 0
/* 
 * @brief double precision vector multiply operation. 
 * t = x .* y 
 */
void dvecm(double N, double alpha, double *x, double *y, double *t)
{
    int i;
    // #ifdef _OPENMP
    // #pragma omp parallel for num_threads(DG_THREADS)
    // #endif
    for (i = 0; i < N; i++)
    {
        t[i] = t[i] + alpha * x[i] * y[i];
    }
}

/* 
 * @brief double precision vector divide operation. 
 * t = x .* y 
 */
void dvecd(double N, double alpha, double *x, double *y, double *t)
{
    int i;
    // #ifdef _OPENMP
    // #pragma omp parallel for num_threads(DG_THREADS)
    // #endif
    for (i = 0; i < N; i++)
    {
        t[i] = t[i] + alpha * x[i] / y[i];
    }
}

/*
 */
void rhs_parall(size_t Np, size_t K, size_t Nfp,
                double *Dr, double *Ds, double *LIFT,
                double *rx, double *ry, double *sx, double *sy,
                double *J, double *Js,
                double *dflux, double *eflux, double *gflux,
                double *rhs)
{
}
/*
 */
void rhs_term(size_t Np, size_t K, size_t Nfp,
              double *Dr, double *Ds, double *LIFT,
              double *rx, double *ry, double *sx, double *sy,
              double *J, double *Js,
              double *dflux, double *eflux, double *gflux,
              double *rhs)
{
    char *chn = "N";
    double one = 1.0, zero = 0.0;
    double *vtemp = calloc(Np * K, sizeof(double));
    dgemm(chn, chn, &Np, &K, &Np, &one, Dr, &Np, eflux, &Np, &zero, vtemp, &Np);
    dvecm(Np * K, -1, rx, vtemp, rhs);
    dgemm(chn, chn, &Np, &K, &Np, &one, Ds, &Np, eflux, &Np, &zero, vtemp, &Np);
    dvecm(Np * K, -1, sx, vtemp, rhs);
    dgemm(chn, chn, &Np, &K, &Np, &one, Dr, &Np, gflux, &Np, &zero, vtemp, &Np);
    dvecm(Np * K, -1, ry, vtemp, rhs);
    dgemm(chn, chn, &Np, &K, &Np, &one, Ds, &Np, gflux, &Np, &zero, vtemp, &Np);
    dvecm(Np * K, -1, sy, vtemp, rhs);

    double *stemp = calloc(Nfp * K, sizeof(double));
    dvecm(Nfp * K, 1, Js, dflux, stemp);
    dgemm(chn, chn, &Np, &K, &Nfp, &one, LIFT, &Np, stemp, &Nfp, &zero, vtemp, &Np);
    dvecd(Np * K, 1, vtemp, J, rhs);

    free(vtemp);
    free(stemp);
    return;
}

/**
 * @brief calculate the R.H.S of conv2d problem.
 *
 * Usages:
 *  [ rhsQ, rhsV ] = rhs_term(f_Q, v_Q, f_ext, u, v, 
 *                      nx, ny, eidM, eidP, eidtype, EToR,    % for surface term
 *                      Dr, Ds, rx, ry, sx, sy, LIFT, J, Js)  % for rhs term
 */
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    /* check input */
    if (nrhs != 20)
    {
        mexErrMsgIdAndTxt("MATLAB:rhs_term:invalidNumInputs",
                          "20 input required.");
    }
    else if (nlhs > 2)
    {
        mexErrMsgIdAndTxt("MATLAB:rhs_term:maxlhs",
                          "Too many output arguments.");
    }

    double *f_Q = mxGetPr(prhs[0]);
    double *v_Q = mxGetPr(prhs[1]); // finite volume values
    double *f_extQ = mxGetPr(prhs[2]);
    double *u = mxGetPr(prhs[3]);
    double *v = mxGetPr(prhs[4]);
    double *nx = mxGetPr(prhs[5]);
    double *ny = mxGetPr(prhs[6]);
    double *eidM = mxGetPr(prhs[7]);
    double *eidP = mxGetPr(prhs[8]);
    signed char *eidtype = (signed char *)mxGetData(prhs[9]);
    signed char *EToR = (signed char *)mxGetData(prhs[10]);
    double *Dr = mxGetPr(prhs[11]);
    double *Ds = mxGetPr(prhs[12]);
    double *rx = mxGetPr(prhs[13]);
    double *ry = mxGetPr(prhs[14]);
    double *sx = mxGetPr(prhs[15]);
    double *sy = mxGetPr(prhs[16]);
    double *LIFT = mxGetPr(prhs[17]);
    double *J = mxGetPr(prhs[18]);
    double *Js = mxGetPr(prhs[19]);

    /* get dimensions */
    size_t Np = mxGetM(prhs[0]);
    size_t K = mxGetN(prhs[0]);
    size_t Nfp = mxGetM(prhs[6]);

    /* allocate output array */
    plhs[0] = mxCreateDoubleMatrix((mwSize)Np, (mwSize)K, mxREAL);
    plhs[1] = mxCreateDoubleMatrix((mwSize)Np, (mwSize)K, mxREAL);
    double *rhsQ = mxGetPr(plhs[0]);
    double *rhsV = mxGetPr(plhs[1]);

    /* surfce integral term */
    double *dflux = calloc(Nfp * K, sizeof(double));
    surf_term(Nfp, K, f_Q, f_extQ, u, v, nx, ny, eidM, eidP, eidtype, EToR, dflux);

#if DEBUG
    mexPrintf("dflux = \n");
    int n, k;
    for (n = 0; n < Nfp; n++)
    {
        mexPrintf("\t");
        for (k = 0; k < K; k++)
        {
            mexPrintf("%e\t", dflux[k * Np + n]);
        }
        mexPrintf("\n");
    }
#endif
    /* volume flux term */
    double *eflux = calloc(Np * K, sizeof(double));
    double *gflux = calloc(Np * K, sizeof(double));
    flux_term(Np, K, f_Q, u, v, EToR, eflux, gflux);

#if DEBUG
    mexPrintf("e = \n");
    for (n = 0; n < Np; n++)
    {
        mexPrintf("\t");
        for (k = 0; k < K; k++)
        {
            mexPrintf("%f\t", eflux[k * Np + n]);
        }
        mexPrintf("\n");
    }

    mexPrintf("g = \n");
    for (n = 0; n < Np; n++)
    {
        mexPrintf("\t");
        for (k = 0; k < K; k++)
        {
            mexPrintf("%f\t", gflux[k * Np + n]);
        }
        mexPrintf("\n");
    }
#endif
    /* DG rhs term */
    rhs_term(Np, K, Nfp, Dr, Ds, LIFT, rx, ry, sx, sy, J, Js, dflux, eflux, gflux, rhsQ);
    /* fv surface term */
    free(dflux);
    free(eflux);
    free(gflux);
    return;
}