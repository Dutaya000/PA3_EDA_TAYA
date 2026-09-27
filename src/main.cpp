#include "../include/benchmark/benchmark_runner.hpp"

int main() {
    // Invoca la simulación de e-commerce y genera el reporte de métricas obligatorias (VP-Tree vs Fuerza Bruta y BK-Tree)
    BenchmarkRunner::ejecutarEvaluacion();
    return 0;
}
