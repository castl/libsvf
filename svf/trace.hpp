#ifndef __SVF_TRACE_HPP__
#define __SVF_TRACE_HPP__

#include <vector>

namespace SVF {

template <typename Type>
class Trace {
    std::vector<Type> trace;

public:
    Trace() {
    }

    ~Trace() {
    }

    void push(Type point) {
        trace.push_back(point);
    }

    Type operator[](size_t i) {
        return trace[i];
    }

    size_t size() {
        return trace.size();
    }
}; // class Trace

}; // namespace SVF

#endif // __SVF_TRACE_HPP__
