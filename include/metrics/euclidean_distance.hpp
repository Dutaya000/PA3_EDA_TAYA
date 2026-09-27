#ifndef EUCLIDEAN_DISTANCE_HPP
#define EUCLIDEAN_DISTANCE_HPP

#include "metric.hpp"
#include <cmath>
#include <stdexcept>

class EuclideanDistance : public Metric<std::vector<double>> {
public:
    double distance(const std::vector<double>& a, const std::vector<double>& b) const override {
        if (a.size() != b.size()) {
            throw std::invalid_argument("Los vectores deben tener la misma dimension.");
        }
        
        double sum = 0.0;
        for (size_t i = 0; i < a.size(); ++i) {
            double diff = a[i] - b[i];
            sum += diff * diff;
        }
        return std::sqrt(sum);
    }
};

#endif
