// Generated by using Rcpp::compileAttributes() -> do not edit by hand
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#include <Rcpp.h>

using namespace Rcpp;

// check_namedness
void check_namedness(SEXP x);
RcppExport SEXP _namedness_check_namedness(SEXP xSEXP) {
BEGIN_RCPP
    Rcpp::traits::input_parameter< SEXP >::type x(xSEXP);
    check_namedness(x);
    return R_NilValue;
END_RCPP
}

static const R_CallMethodDef CallEntries[] = {
    {"_namedness_check_namedness", (DL_FUNC) &_namedness_check_namedness, 1},
    {NULL, NULL, 0}
};

RcppExport void R_init_namedness(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
