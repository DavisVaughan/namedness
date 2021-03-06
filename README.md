
<!-- README.md is generated from README.Rmd. Please edit that file -->

# namedness

``` r
# load all internals
devtools::load_all()
#> Loading namedness
```

Exploring the `MAYBE_REFERENCED()` and `MAYBE_SHARED()` interaction with
Rcpp.

Currently on R 3.5.3.

## Examples

If you just use `check_namedness()` as Rcpp exports it, we have a
problem where, no matter what, it looks like the objects are referenced.

This correctly says that `x` has a name attached to it.

``` r
x <- c(1, 2, 3)
check_namedness(x)
#> Maybe referenced!
#> Maybe shared!
```

But when we move to temporary (non ALTREP) objects, these shouldn’t be
named:

``` r
check_namedness(c(1, 2, 3))
#> Maybe referenced!
#> Maybe shared!
```

The problem is the fact that we have an R function wrapping the
`.Call()`. If we use the `.Call()` mechanism directly, all seems well.

``` r
.Call(`_namedness_check_namedness`, c(1, 2, 3))
#> No references here!
#> NULL
```

If we use an ALTREP-like object, it will be referenced, by design.

``` r
.Call(`_namedness_check_namedness`, 1)
#> Maybe referenced!
#> Maybe shared!
#> NULL
.Call(`_namedness_check_namedness`, 1L)
#> Maybe referenced!
#> Maybe shared!
#> NULL
.Call(`_namedness_check_namedness`, 1:2)
#> Maybe referenced!
#> Maybe shared!
#> NULL

# Look at NAM(3)!
.Internal(inspect(1L))
#> @7f91957ada60 13 INTSXP g0c1 [NAM(3)] (len=1, tl=0) 1

# NAM(3) and (compact)
.Internal(inspect(1:2))
#> @7f919c7575e8 13 INTSXP g0c0 [NAM(3)]  1 : 2 (compact)

# As opposed to this:
# No NAM() tag
.Internal(inspect(c(1, 2, 3)))
#> @7f919cae55e8 14 REALSXP g0c3 [] (len=3, tl=0) 1,2,3
```

What about temporaries?

``` r
x <- c(1, 2, 3)
y <- c(3, 4, 5)

# of course...
check_namedness(x + y)
#> Maybe referenced!
#> Maybe shared!

# nothing here!
.Call(`_namedness_check_namedness`, x + y)
#> No references here!
#> NULL
```

This idea is useful because we could mimic base R (in theory), and reuse
the memory of temporaries when performing operations like `x + y + z`.
Reuse the memory of `x + y = <temporary>` to compute `<temporary> + z`
(as long as the result has the right shape).

But, we obviously need to wrap this inside a function…which is basically
what Rcpp does for us. And we immediately hit the issue again.

``` r
nmdness <- function(x) .Call(`_namedness_check_namedness`, x)
nmdness(c(1, 2))
#> Maybe referenced!
#> Maybe shared!
#> NULL
```

There is a note here on the fact that `NAMED = 1` optimization only
really works effectively if the function is a primitive. “Note that
optimizing NAMED = 1 is only effective within a primitive (as the
closure wrapper of a .Internal will set NAMED = NAMEDMAX when the
promise to the argument is evaluated) and hence replacement functions
should where possible be primitive to avoid copying (at least in their
default methods).”
<https://cran.r-project.org/doc/manuals/r-release/R-ints.html#g_t_002eInternal-vs-_002ePrimitive>

Another note is in R Extensions, in 5.9.10 here at Named Objects and
Copying. “The case NAMED(foo) == 1 allows some optimization, but it can
be ignored (and duplication done whenever NAMED(foo) \> 0). (This
optimization is not currently usable in user code.) It is intended for
use within replacement functions.”
<https://cran.r-project.org/doc/manuals/r-release/R-exts.html#Named-objects-and-copying>

This still doesn’t suggest how one could use the `NAMED = 0` case
though.

I think this also applies to the `NAMED = 0` case with temporaries. If
we wrap it in a function, we immediately have issues.

I guess `NAMED = 0` could be useful if you pass a SEXP created at the
C++ level to another C++ function immediately, which could take
advantage of the fact that you haven’t bound it to any name at the R
level. Perhaps this is what `vctrs::vec_restore()` is doing when being
called from something like `vec_slice()`.
<https://github.com/r-lib/vctrs/blob/8251896d6db132ada6e7702b753de24dee66dc56/src/slice.c#L142>
<https://github.com/r-lib/vctrs/blob/84839833c78b5df23a769951cff62f37d9d43dec/src/cast.c#L278>

Note that all the base operators like `+` are primitives, so they can
take advantage of `NO_REFERENCES()` and reuse memory smartly.

## Solution

I don’t think there is one right now. Obviously we can’t have users
using `.Call()`, but it seems impossible to wrap the `.Call()` in
something more user friendly to use where we can still take advantage of
having “no references”.

Here is another example from Dirk showing the same issue:
<https://github.com/RcppCore/Rcpp/issues/943#issuecomment-469231746>
