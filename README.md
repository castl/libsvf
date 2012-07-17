LibSVF -- A Library for computing SVF
======

Library to help in computing Side-channel Vulnerability Factor, a
metric defined in "Side-channel Vulnerability Factor: A Metric for
Measuring Information Leakage", published in ISCA 2012.

http://www.cs.columbia.edu/~simha/preprint_isca12_svf.pdf


Usage
======
Usage examples for libsvf can be found in tests/*.cpp. As a small
example, here is a very basic usage using scalars as the data
collection points:

```c++
#include <svf/svf.hpp>

struct ScalarDist {
    template <typename T>
    double operator()(T a, T b) {
        assert(!isnan(a));
        assert(!isnan(b));
        return sqrt(pow((double)a, 2) + pow((double)b, 2));
    }
};

// ...
SVF::SVF<int, ScalarDist, int, ScalarDist> svf;

for (size_t i=0; i<1000; i++) {
    svf.pushTimestep(i, i);
}

double svfVal = svf.computeSVF();
// This will be 1.0 since points are the same
```


Here's an example of sparse vectors and doubles as data points. In
this example, the "attacker" can see only the length of the oracle
vector, but there is still some correlation:
```c++
#include <svf/svf.hpp>
#include <svf/sparse_vector.hpp>

struct ScalarDist {
    template <typename T>
    double operator()(T a, T b) {
        assert(!isnan(a));
        assert(!isnan(b));
        return sqrt(pow((double)a, 2) + pow((double)b, 2));
    }
};

// ...

SVF::SVF<SVF::SparseVector<double>, SVF::SparseVector<double>::EuclideanDistance<>,
         double, ScalarDist> svf; 

for (size_t i=0; i<100; i++) {
    // Just the length seems a decent approximation for 6
    // dimensions
    SVF::SparseVector<double> a = randVec(6);
    svf.pushTimestep(a, a.length());
}

double svfVal = svf.computeSVF();
// This will be between 0 and 1
```

Documentation
=====
Yeah, right! Read the code and the unit tests.


Credits
=====
SVF research is conducted at the Computer Architecture and Security
Technologies Lab in the Computer Science department at Columbia
University in the city of New York. The original inventors of SVF
are John Demme, Robert Martin, Adam Waksman and Simha Sethumadhavan.

LibSVF code written and occasionally maintained by John Demme
<jdd@cs.columbia.edu>.
