#ifndef BK_TREE_HPP
#define BK_TREE_HPP

#include "../core/dataset.hpp"
#include "../core/query_result.hpp"
#include "../metrics/metric.hpp"
#include <string>
#include <vector>
#include <map>

// Nodo específico para cadenas en el BK-Tree
struct BKNode {
    Producto producto;
    std::map<int, BKNode*> hijos; // Hijos indexados por su distancia entera

    BKNode(const Producto& p) : producto(p) {}
    ~BKNode() {
        for (auto& par : hijos) {
            delete par.second;
        }
    }
};

class BKTree {
private:
    BKNode* raiz;
    const Metric<std::string>& metrica;

    // Inserción dinámica recursiva
    void insertarRecursivo(BKNode* nodo, const Producto& nuevo_prod) {
        // En el catálogo, indexamos basándonos en el nombre del producto
        int dist = static_cast<int>(metrica.distance(nodo->producto.nombre, nuevo_prod.nombre));

        // Si ya existe un hijo con esa distancia exacta, delegamos la inserción hacia abajo
        if (nodo->hijos.find(dist) != nodo->hijos.end()) {
            insertarRecursivo(nodo->hijos[dist], nuevo_prod);
        } else {
            // Si no existe, creamos el nuevo camino discretizado
            nodo->hijos[dist] = new BKNode(nuevo_prod);
        }
    }

    // Búsqueda por Rango con Poda en Espacio Métrico Discreto
    void buscarPorRangoRecursivo(BKNode* nodo, const std::string& query_nombre, double r, std::vector<QueryResult>& resultados) const {
        if (!nodo) return;

        int d = static_cast<int>(metrica.distance(nodo->producto.nombre, query_nombre));

        // Si la distancia de edición entra en el radio de tolerancia, lo capturamos
        if (d <= r) {
            resultados.push_back({nodo->producto, static_cast<double>(d)});
        }

        // --- PODA POR DESIGUALDAD TRIANGULAR EN BK-TREE ---
        // Solo revisamos los subárboles cuyas distancias estén dentro del umbral geométrico [d - r, d + r]
        int limite_inferior = d - static_cast<int>(r);
        int limite_superior = d + static_cast<int>(r);

        for (int i = limite_inferior; i <= limite_superior; ++i) {
            if (nodo->hijos.find(i) != nodo->hijos.end()) {
                buscarPorRangoRecursivo(nodo->hijos[i], query_nombre, r, resultados);
            }
        }
    }

public:
    BKTree(const Metric<std::string>& metrica_texto) : raiz(nullptr), metrica(metrica_texto) {}
    
    ~BKTree() {
        delete raiz;
    }

    void insertar(const Producto& p) {
        if (!raiz) {
            raiz = new BKNode(p);
        } else {
            insertarRecursivo(raiz, p);
        }
    }

    std::vector<QueryResult> buscarPorRango(const std::string& query_nombre, double r) const {
        std::vector<QueryResult> resultados;
        buscarPorRangoRecursivo(raiz, query_nombre, r, resultados);
        return resultados;
    }
};

#endif

