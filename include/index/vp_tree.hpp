#ifndef VP_TREE_HPP
#define VP_TREE_HPP

#include "../core/dataset.hpp"
#include "../core/query_result.hpp"
#include "../metrics/metric.hpp"
#include <vector>
#include <algorithm>
#include <queue>
#include <stdexcept>

// Nodo interno del Vantage Point Tree
struct VPNode {
    Producto producto_pivote; // El punto de ventaja seleccionado
    double mu;                // Radio de la mediana (umbral de partición)
    VPNode* izquierdo;        // Subárbol interno: distancias <= mu
    VPNode* derecho;          // Subárbol externo: distancias > mu

    VPNode(const Producto& p) : producto_pivote(p), mu(0.0), izquierdo(nullptr), derecho(nullptr) {}
    ~VPNode() {
        delete izquierdo;
        delete derecho;
    }
};

class VPTree {
private:
    VPNode* raiz;
    const Metric<std::vector<double>>& metrica;

    // Método recursivo para construir el árbol usando la mediana
    VPNode* construirArbol(std::vector<Producto>& productos, size_t inicio, size_t fin) {
        if (inicio >= fin) return nullptr;

        // 1. Seleccionar el primer elemento del rango actual como Vantage Point (Pivote)
        VPNode* nodo = new VPNode(productos[inicio]);
        if (inicio + 1 == fin) return nodo;

        size_t indice_mediana = inicio + (fin - inicio) / 2;

        // 2. Ordenar parcialmente el subarreglo basándonos en la distancia al pivote
        std::nth_element(productos.begin() + inicio + 1, productos.begin() + indice_mediana, productos.begin() + fin,
            [this, nodo](const Producto& a, const Producto& b) {
                return metrica.distance(nodo->producto_pivote.embedding, a.embedding) < 
                       metrica.distance(nodo->producto_pivote.embedding, b.embedding);
            });

        // 3. El radio 'mu' se define como la distancia del pivote al elemento de la mediana
        nodo->mu = metrica.distance(nodo->producto_pivote.embedding, productos[indice_mediana].embedding);

        // 4. Dividir y vencer de forma balanceada
        nodo->izquierdo = construirArbol(productos, inicio + 1, indice_mediana + 1);
        nodo->derecho = construirArbol(productos, indice_mediana + 1, fin);

        return nodo;
    }

    // Búsqueda por Rango Recursiva con Poda Geométrica
    void buscarPorRangoRecursivo(VPNode* nodo, const std::vector<double>& query, double r, std::vector<QueryResult>& resultados) const {
        if (!nodo) return;

        double d = metrica.distance(nodo->producto_pivote.embedding, query);

        // Si el pivote actual está dentro del rango de tolerancia, lo guardamos
        if (d <= r) {
            resultados.push_back({nodo->producto_pivote, d});
        }

        // --- PODA MATEMÁTICA POR DESIGUALDAD TRIANGULAR ---
        // Condición para explorar la bola izquierda (Interior): ¿el query intersecta el espacio interno?
        if (d - r <= nodo->mu) {
            buscarPorRangoRecursivo(nodo->izquierdo, query, r, resultados);
        }
        // Condición para explorar la bola derecha (Exterior): ¿el query intersecta el espacio externo?
        if (d + r > nodo->mu) {
            buscarPorRangoRecursivo(nodo->derecho, query, r, resultados);
        }
    }

    // Búsqueda k-NN Recursiva con Radio Dinámico Acotado por una Max-Heap
    void buscarKNNRecursivo(VPNode* nodo, const std::vector<double>& query, size_t k, 
                             std::priority_queue<QueryResult>& heap_resultados) const {
        if (!nodo) return;

        double d = metrica.distance(nodo->producto_pivote.embedding, query);

        // Si la heap no está llena, insertamos directamente
        if (heap_resultados.size() < k) {
            heap_resultados.push({nodo->producto_pivote, d});
        } 
        // Si ya está llena, comparamos contra el peor de los vecinos actuales (el de mayor distancia en el top)
        else if (d < heap_resultados.top().distancia) {
            heap_resultados.pop();
            heap_resultados.push({nodo->producto_pivote, d});
        }

        // El "radio dinámico máximo" está determinado por la distancia del k-ésimo vecino actual
        // Si la heap no tiene k elementos aún, el rango efectivo es infinito (explora ambas ramas si es necesario)
        double radio_maximo = (heap_resultados.size() < k) ? std::numeric_limits<double>::max() : heap_resultados.top().distancia;

        // Decidir estratégicamente qué rama visitar primero para encontrar vecinos más cercanos más rápido
        if (d < nodo->mu) {
            // El query está dentro de la mediana, priorizar subárbol izquierdo
            if (d - radio_maximo <= nodo->mu) {
                buscarKNNRecursivo(nodo->izquierdo, query, k, heap_resultados);
            }
            // Actualizar el radio tras revisar la rama izquierda por si encontramos mejores vecinos
            radio_maximo = (heap_resultados.size() < k) ? std::numeric_limits<double>::max() : heap_resultados.top().distancia;
            if (d + radio_maximo > nodo->mu) {
                buscarKNNRecursivo(nodo->derecho, query, k, heap_resultados);
            }
        } else {
            // El query está fuera de la mediana, priorizar subárbol derecho
            if (d + radio_maximo > nodo->mu) {
                buscarKNNRecursivo(nodo->derecho, query, k, heap_resultados);
            }
            // Actualizar el radio tras revisar la rama derecha
            radio_maximo = (heap_resultados.size() < k) ? std::numeric_limits<double>::max() : heap_resultados.top().distancia;
            if (d - radio_maximo <= nodo->mu) {
                buscarKNNRecursivo(nodo->izquierdo, query, k, heap_resultados);
            }
        }
    }

public:
    VPTree(const Metric<std::vector<double>>& metrica_implementacion) 
        : raiz(nullptr), metrica(metrica_implementacion) {}

    ~VPTree() {
        delete raiz;
    }

    // Construye el árbol a partir del catálogo
    void construir(std::vector<Producto>& catalogo) {
        if (raiz) {
            delete raiz;
            raiz = nullptr;
        }
        raiz = construirArbol(catalogo, 0, catalogo.size());
    }

    // Ejecuta la consulta de búsqueda por rango
    std::vector<QueryResult> rangeSearch(const std::vector<double>& query, double r) const {
        std::vector<QueryResult> resultados;
        buscarPorRangoRecursivo(raiz, query, r, resultados);
        std::sort(resultados.begin(), resultados.end());
        return resultados;
    }

    // Ejecuta la consulta k-NN
    std::vector<QueryResult> knnSearch(const std::vector<double>& query, size_t k) const {
        if (k == 0) return {};
        
        std::priority_queue<QueryResult> heap_resultados;
        buscarKNNRecursivo(raiz, query, k, heap_resultados);

        // Extraer los datos de la heap y ordenarlos de menor a mayor distancia
        std::vector<QueryResult> resultados;
        resultados.reserve(heap_resultados.size());
        while (!heap_resultados.empty()) {
            resultados.push_back(heap_resultados.top());
            heap_resultados.pop();
        }
        std::reverse(resultados.begin(), resultados.end());
        return resultados;
    }
};

#endif
