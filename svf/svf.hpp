#ifndef __SVF_SVF_HPP__
#define __SVF_SVF_HPP__

#include <svf/trace.hpp>
#include <svf/matrix.hpp>
#include <svf/matrix_correlate.hpp>

#include <stdint.h>
#include <cassert>

namespace SVF {

template <typename OracleType, typename OracleDist,
          typename SideChannelType, typename SideChannelDist>
class SVF {
    Trace<OracleType> oracleTrace;
    Trace<SideChannelType> sideChannelTrace;

public:
    SVF() { }

    void pushTimestep(OracleType oPoint, SideChannelType scPoint) {
        oracleTrace.push(oPoint);
        sideChannelTrace.push(scPoint);
    }

    template<typename Selector   = FullTriangleSelector,
             typename CorrelAlgo = Pearson>
    double computeSVF(Selector selector = Selector(),
                      CorrelAlgo correlAlgo = CorrelAlgo())
    {
        selector.init(this->oracleTrace, this->sideChannelTrace);

        LazyMatrix<OracleType, OracleDist> oMatrix(oracleTrace);
        LazyMatrix<SideChannelType, SideChannelDist> scMatrix(sideChannelTrace);
        return correlAlgo(oMatrix, scMatrix, selector);
    }

}; // class SVF

}; // namespace SVF

#endif // __SVF_SVF_HPP__
