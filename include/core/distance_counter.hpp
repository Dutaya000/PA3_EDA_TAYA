#ifndef DISTANCE_COUNTER_HPP
#define DISTANCE_COUNTER_HPP

#include "../metrics/metric.hpp"

template <typename T>
class DistanceCounter : public Metric<T> {
private:
    const Metric<T>& base_metric;
    mutable long long count; // mutable permite modificarlo en funciones const

public:
    DistanceCounter(const Metric<T>& metric) : base_metric(metric), count(0) {}

    double distance(const T& a, const T& b) const override {
        count++; // Incrementa cada vez que el VP-Tree o la Fuerza Bruta miden algo
        return base_metric.distance(a, b);
    }

    long long get_count() const { return count; }
    void reset() { count = 0; }
};

#endif
