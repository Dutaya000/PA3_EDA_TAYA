#ifndef METRIC_HPP
#define METRIC_HPP

#include <vector>
#include <string>

// Interfaz genérica para cualquier espacio métrico
template <typename T>
class Metric {
public:
    virtual ~Metric() = default;
    
    // Método puro que toda distancia debe implementar
    virtual double distance(const T& a, const T& b) const = 0;
};

#endif
