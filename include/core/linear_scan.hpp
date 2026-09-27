#ifndef LINEAR_SCAN_HPP
#define LINEAR_SCAN_HPP

#include "dataset.hpp"
#include "query_result.hpp"
#include "../metrics/metric.hpp"
#include <vector>
#include <algorithm>

class LinearScan {
private:
    const Metric<std::vector<double>>& metric;

public:
    LinearScan(const Metric<std::vector<double>>& metric_implementation) 
        : metric(metric_implementation) {}

    // Búsqueda por Rango (Fuerza Bruta Lineal)
    std::vector<QueryResult> rangeSearch(const std::vector<Producto>& catalogo, const std::vector<double>& query_embedding, double r) {
        std::vector<QueryResult> resultados;

        for (const auto& prod : catalogo) {
            double dist = metric.distance(query_embedding, prod.embedding);
            if (dist <= r) {
                resultados.push_back({prod, dist});
            }
        }

        // Ordenamos los resultados del más similar al menos similar
        std::sort(resultados.begin(), resultados.end());
        return resultados;
    }

    // Búsqueda k-NN (Fuerza Bruta Lineal)
    std::vector<QueryResult> knnSearch(const std::vector<Producto>& catalogo, const std::vector<double>& query_embedding, size_t k) {
        std::vector<QueryResult> todos_los_resultados;
        todos_los_resultados.reserve(catalogo.size());

        for (const auto& prod : catalogo) {
            double dist = metric.distance(query_embedding, prod.embedding);
            todos_los_resultados.push_back({prod, dist});
        }

        // Ordenamos todo el catálogo por distancia y tomamos los 'k' más cercanos
        std::sort(todos_los_resultados.begin(), todos_los_resultados.end());
        
        if (todos_los_resultados.size() > k) {
            todos_los_resultados.resize(k);
        }

        return todos_los_resultados;
    }
};

#endif
