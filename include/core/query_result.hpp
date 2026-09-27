#ifndef QUERY_RESULT_HPP
#define QUERY_RESULT_HPP

#include "dataset.hpp"

// Estructura para almacenar un producto emparejado junto con su distancia al query
struct QueryResult {
    Producto producto;
    double distancia;

    // Operador para poder ordenar los resultados por cercanía (de menor a mayor distancia)
    bool operator<(const QueryResult& otro) const {
        return distancia < otro.distancia;
    }
};

#endif
