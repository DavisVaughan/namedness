#include <Rcpp.h>

// [[Rcpp::export(rng = false)]]
void check_namedness(SEXP x) {
  if (MAYBE_REFERENCED(x)) {
    Rcpp::Rcout << "Maybe referenced!" << std::endl;
  }

  if (MAYBE_SHARED(x)) {
    Rcpp::Rcout << "Maybe shared!" << std::endl;
  }

  if (NO_REFERENCES(x)) {
    Rcpp::Rcout << "No references here!" << std::endl;
  }
}
