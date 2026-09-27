#ifndef BENCHMARK_RUNNER_HPP
#define BENCHMARK_RUNNER_HPP

#include "../core/dataset.hpp"
#include "../core/linear_scan.hpp"
#include "../core/distance_counter.hpp"
#include "../index/vp_tree.hpp"
#include "../index/bk_tree.hpp"

// --- ESTAS SON LAS LÍNEAS QUE DEBES AGREGAR PARA CORREGIR EL ERROR ---
#include "../metrics/euclidean_distance.hpp"
#include "../metrics/levenshtein_distance.hpp"

#include <iostream>
#include <chrono>
#include <iomanip>

class BenchmarkRunner {
public:
    static void ejecutarEvaluacion() {
        const size_t TOTAL_PRODUCTOS = 5000;
        const size_t DIMENSIONES = 64; // Vectores de alta dimensionalidad (Embeddings)
        const double RADIO_BUSQUEDA_VECTOR = 35.0;
        const size_t K_VECINOS = 5;

        std::cout << "=========================================================\n";
        std::cout << "     INICIANDO BENCHMARK: CATALOGO E-COMMERCE MÈTRICO   \n";
        std::cout << "=========================================================\n";
        std::cout << "[+] Generando dataset sintético de " << TOTAL_PRODUCTOS << " productos (" << DIMENSIONES << "D)...\n";
        
        auto catalogo = Dataset::generarCatalogo(TOTAL_PRODUCTOS, DIMENSIONES);

        // Instanciar métrica e instrumentador de distancias continuas
        EuclideanDistance metrica_base;
        DistanceCounter<std::vector<double>> metrica_contador(metrica_base);

        // Instanciar algoritmos
        LinearScan scan_lineal(metrica_contador);
        VPTree arbol_vp(metrica_contador);

        // --- EXPERIMENTO 1: CONSTRUCCIÓN DEL VP-TREE ---
        std::cout << "[+] Construyendo estructura métrica VP-Tree...\n";
        metrica_contador.reset();
        auto t_build_start = std::chrono::high_resolution_clock::now();
        arbol_vp.construir(catalogo);
        auto t_build_end = std::chrono::high_resolution_clock::now();
        
        long long dist_construccion = metrica_contador.get_count();
        auto tiempo_construccion = std::chrono::duration_cast<std::chrono::microseconds>(t_build_end - t_build_start).count();

        // Seleccionar un producto de consulta (Query) con ligeras variaciones (Duplicado tipográfico/visual)
        std::vector<double> query_embedding = catalogo[10].embedding;
        for (size_t d = 0; d < DIMENSIONES; ++d) {
            query_embedding[d] += 0.5; // Ruido simulado de baja calidad
        }

        // --- EXPERIMENTO 2: BÚSQUEDA POR RANGO (VP-TREE vs FUERZA BRUTA) ---
        std::cout << "[+] Ejecutando Búsqueda por Rango (Radio: " << RADIO_BUSQUEDA_VECTOR << ")...\n";

        // Caso Linear Scan
        metrica_contador.reset();
        auto t_scan_r_start = std::chrono::high_resolution_clock::now();
        auto res_scan_r = scan_lineal.rangeSearch(catalogo, query_embedding, RADIO_BUSQUEDA_VECTOR);
        auto t_scan_r_end = std::chrono::high_resolution_clock::now();
        long long dist_scan_r = metrica_contador.get_count();
        auto tiempo_scan_r = std::chrono::duration_cast<std::chrono::microseconds>(t_scan_r_end - t_scan_r_start).count();

        // Caso VP-Tree
        metrica_contador.reset();
        auto t_vp_r_start = std::chrono::high_resolution_clock::now();
        auto res_vp_r = arbol_vp.rangeSearch(query_embedding, RADIO_BUSQUEDA_VECTOR);
        auto t_vp_r_end = std::chrono::high_resolution_clock::now();
        long long dist_vp_r = metrica_contador.get_count();
        auto tiempo_vp_r = std::chrono::duration_cast<std::chrono::microseconds>(t_vp_r_end - t_vp_r_start).count();

        // --- EXPERIMENTO 3: BÚSQUEDA K-NN (VP-TREE vs FUERZA BRUTA) ---
        std::cout << "[+] Ejecutando Búsqueda de " << K_VECINOS << " Vecinos Más Cercanos (k-NN)...\n";

        // Caso Linear Scan
        metrica_contador.reset();
        auto t_scan_k_start = std::chrono::high_resolution_clock::now();
        auto res_scan_k = scan_lineal.knnSearch(catalogo, query_embedding, K_VECINOS);
        auto t_scan_k_end = std::chrono::high_resolution_clock::now();
        long long dist_scan_k = metrica_contador.get_count();
        auto tiempo_scan_k = std::chrono::duration_cast<std::chrono::microseconds>(t_scan_k_end - t_scan_k_start).count();

        // Caso VP-Tree
        metrica_contador.reset();
        auto t_vp_k_start = std::chrono::high_resolution_clock::now();
        auto res_vp_k = arbol_vp.knnSearch(query_embedding, K_VECINOS);
        auto t_vp_k_end = std::chrono::high_resolution_clock::now();
        long long dist_vp_k = metrica_contador.get_count();
        auto tiempo_vp_k = std::chrono::duration_cast<std::chrono::microseconds>(t_vp_k_end - t_vp_k_start).count();

        // --- IMPRESIÓN FORMATEADA DEL REPORTE (PUNTO C Y D DE LA CONSIGNA) ---
        std::cout << "\n=========================================================\n";
        std::cout << "             REPORTE FINAL DE METRICAS OBLIGATORIAS      \n";
        std::cout << "=========================================================\n";
        std::cout << "Fase de Construcción (VP-Tree):\n";
        std::cout << " -> Tiempo Empleado        : " << tiempo_construccion << " microsegundos\n";
        std::cout << " -> Eval. de Distancia     : " << dist_construccion << " operaciones\n";
        std::cout << "---------------------------------------------------------\n";
        std::cout << std::left << std::setw(25) << "MÉTODO / CONSULTA" 
                  << std::setw(18) << "TIEMPO (us)" 
                  << std::setw(20) << "# EVAL. DISTANCIA" 
                  << "RESULTADOS\n";
        std::cout << "---------------------------------------------------------\n";
        std::cout << std::setw(25) << "Linear Scan (Rango)" << std::setw(18) << tiempo_scan_r << std::setw(20) << dist_scan_r << res_scan_r.size() << " productos\n";
        std::cout << std::setw(25) << "VP-Tree (Rango)" << std::setw(18) << tiempo_vp_r << std::setw(20) << dist_vp_r << res_vp_r.size() << " productos\n";
        std::cout << "---------------------------------------------------------\n";
        std::cout << std::setw(25) << "Linear Scan (k-NN)" << std::setw(18) << tiempo_scan_k << std::setw(20) << dist_scan_k << res_scan_k.size() << " productos\n";
        std::cout << std::setw(25) << "VP-Tree (k-NN)" << std::setw(18) << tiempo_vp_k << std::setw(20) << dist_vp_k << res_vp_k.size() << " productos\n";
        std::cout << "=========================================================\n";

        // --- EXPERIMENTO EXTENSIÓN: PRUEBA COMPACTA BK-TREE (CADENAS) ---
        std::cout << "\n[+] Evaluando Extensión Opcional: BK-Tree para Cadenas discretas\n";
        LevenshteinDistance metrica_texto;
        BKTree arbol_bk(metrica_texto);

        // Insertar manualmente palabras con errores tipográficos comunes
        arbol_bk.insertar({1, "Zapatilla Nike Negra", {}});
        arbol_bk.insertar({2, "Zapatila Nike Ngra", {}}); // Error de tipeo
        arbol_bk.insertar({3, "Zapatillas Adidas Blancas", {}});
        arbol_bk.insertar({4, "Zapato Formal Cuero", {}});

        std::string query_texto = "Zapatilla Nike Nera"; // Error tipográfico del usuario
        double radio_texto = 3.0; // Tolerar hasta 3 ediciones de distancia

        auto res_bk = arbol_bk.buscarPorRango(query_texto, radio_texto);
        std::cout << "Query de texto: '" << query_texto << "' con Radio de tolerancia: " << radio_texto << "\n";
        std::cout << "Productos similares recuperados por el BK-Tree:\n";
        for (const auto& r : res_bk) {
            std::cout << " -> [" << r.producto.nombre << "] - Distancia de Edición Levenshtein: " << r.distancia << "\n";
        }
        std::cout << "=========================================================\n";
    }
};

#endif
