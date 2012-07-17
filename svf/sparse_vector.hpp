#ifndef __SVF_SPARSE_VECTOR_HPP
#define __SVF_SPARSE_VECTOR_HPP

#include <boost/foreach.hpp>
#include <unordered_map>

namespace SVF {

    template<typename V, typename K = uint64_t>
    class SparseVector {
        std::unordered_map<K, V> vector;

    public:
        struct ScalarDist {
            template<typename S>
            double operator()(S a, S b) {
                return ((double)a) - b;
            }
        };

        template<typename VDist = ScalarDist>
        struct EuclideanDistance {
            VDist dist;
            EuclideanDistance(VDist dist = VDist()) : dist(dist) { }

            double operator()(const SparseVector& a, const SparseVector& b) {
                double sum = 0.0;
                BOOST_FOREACH(auto p, a.vector) {
                    K k = p.first;
                    V av = p.second;
                    auto bv = b.find(k);
                    if (bv) {
                        sum += pow(dist(av, *bv), 2);
                    } else {
                        sum += pow(av, 2);
                    }
                }

                BOOST_FOREACH(auto p, b.vector) {
                    K k = p.first;
                    V bv = p.second;
                    auto av = a.find(k);
                    if (!av) {
                        // Add if we didn't already see this key in
                        // 'a'
                        sum += pow(bv, 2);
                    }
                }
                return sqrt(sum);
            }
        };

        SparseVector() { }

        V& operator[](K k) {
            return vector[k];
        }

        boost::optional<V> find(K k) const {
            auto f = vector.find(k);
            if (f != vector.end())
                return f->second;
            return boost::optional<V>();
        }

        void clear() {
            vector.clear();
        }

        double length() {
            double sum = 0.0;
            BOOST_FOREACH(auto p, vector) {
                sum += pow((double)p.second, 2);
            }
            return sqrt(sum);
        }

        bool operator==(SparseVector& b) {
            if (vector.size() != b.vector.size())
                return false;
            BOOST_FOREACH(auto p, vector) {
                K k = p.first;
                V v = p.second;
                auto bv = b.find(k);
                if (!bv)
                    return false;
                if ((*bv) != v)
                    return false;
            }
            return true;
        }

        bool operator!=(SparseVector& b) {
            return !((*this) == b);
        }

    };

}; // namespace SVF

#endif // __SVF_SPARSE_VECTOR_HPP
