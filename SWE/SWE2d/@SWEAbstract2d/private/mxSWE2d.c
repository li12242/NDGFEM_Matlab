#include "mxSWE2d.h"
#include <math.h>

/**
 * @brief Evaluate the flux term in the surface integration.
 *
 */
void evaluateSurfFluxTerm(const double hmin,
                          const double gra,
                          const double h,
                          const double hu,
                          const double hv,
                          double* E,
                          double* G) {
  double u, v;
  evaluateFlowRateByDeptheThreshold(hmin, h, hu, hv, &u, &v);
  const double huv = h * u * v;
  const double h2 = h * h;
  E[0] = hu;
  G[0] = hv;
  E[1] = h * u * u + 0.5 * gra * h2;
  G[1] = huv;
  E[2] = huv;
  G[2] = h * v * v + 0.5 * gra * h2;
  return;
}

/**
 Evaluate the flow rate depending on the depth threshold
 */
void evaluateFlowRateByDeptheThreshold(const double hcrit,
                                       const double h,
                                       const double hu,
                                       const double hv,
                                       double* u,
                                       double* v) {
  if (h > hcrit) {
//     const double sqrt2 = 1.414213562373095;
//     double h4 = pow(h, 4);
//     *u = sqrt2 * h * hu / sqrt( h4 + max( hcrit, h4 ) );
//     *v = sqrt2 * h * hv / sqrt( h4 + max( hcrit, h4 ) );
    *u = hu / h;
    *v = hv / h;
  } else {
    *u = 0.0;
    *v = 0.0;
  }

  return;
}

/**
 Evaluate the flow rate depending on the cell states
 */
void evaluateFlowRateByCellState(const NdgRegionType type,
                                 const double h,
                                 const double hu,
                                 const double hv,
                                 double* u,
                                 double* v) {
  switch (type) {
    case NdgRegionWet:
      *u = hu / h;
      *v = hv / h;
      break;
    default:
      *u = 0;
      *v = 0;
      break;
  }
}

void evaluateSlipWallAdjacentNodeValue(const double nx,
                                       const double ny,
                                       double* fm,
                                       double* fp) {
  const double hM = fm[0];
  const double qxM = fm[1];
  const double qyM = fm[2];
  double qnM = qxM * nx + qyM * ny;   // outward normal flux
  double qvM = -qxM * ny + qyM * nx;  // outward tangential flux
  // adjacent value
  fp[0] = hM;
  fp[1] = (-qnM) * nx - qvM * ny;
  fp[2] = (-qnM) * ny + qvM * nx;
  return;
}

void evaluateNonSlipWallAdjacentNodeValue(const double nx,
                                          const double ny,
                                          double* fm,
                                          double* fp) {
  const double hM = fm[0];
  const double qxM = fm[1];
  const double qyM = fm[2];
  // adjacent value
  fp[0] = hM;
  fp[1] = -qxM;
  fp[2] = -qyM;
  return;
}

void evaluateFlatherAdjacentNodeValue(double nx,
                                      double ny,
                                      double* fm,
                                      double* fe) {
  const double gra = 9.81;
  double hM = fm[0];
  double h_ext = fe[0];
  double qx_ext = fe[1];
  double qy_ext = fe[2];
  double qn_ext = qx_ext * nx + qy_ext * ny;   // outward normal flux
  double qv_ext = -qx_ext * ny + qy_ext * nx;  // tangential flux

  double qn = qn_ext - sqrt(gra * hM) * (h_ext - hM);
  double qv = qv_ext;
  // double qn = - sqrt(solver.gra*hM)*(2*etaE - etaM);
  fe[0] = fm[0];
  fe[1] = qn * nx - qv * ny;
  fe[2] = qn * ny + qv * nx;
  // f_P[1] = qn*nx;
  // f_P[2] = qn*ny;
  return;
}