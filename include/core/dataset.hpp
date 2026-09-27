#ifndef DATASET_HPP
#define DATASET_HPP

#include <vector>
#include <string>
#include <random>

// Estructura que representa un artículo en el catálogo del e-commerce
struct Producto {
    int id;
    std::string nombre;
    std::vector<double> embedding; // Vector de características de la imagen/texto
};

// Clase utilitaria para manejar y generar el catálogo de pruebas
class Dataset {
public:
    // Genera un catálogo sintético simulando productos base y vendedores que suben duplicados con variaciones
    static std::vector<Producto> generarCatalogo(size_t cantidad_productos, size_t dimensiones, unsigned int semilla = 42) {
        std::vector<Producto> catalogo;
        catalogo.reserve(cantidad_productos);

        std::mt19937 gen(semilla);
        std::uniform_real_distribution<double> dist_base(0.0, 100.0);
        std::uniform_real_distribution<double> dist_ruido(-2.0, 2.0); // Ruido que simula variaciones/duplicados

        for (size_t i = 0; i < cantidad_productos; ++i) {
            Producto p;
            p.id = static_cast<int>(i);
            p.nombre = "Producto_" + std::to_string(i);
            p.embedding.resize(dimensiones);

            // Cada cierto tramo, creamos un "duplicado" o variación de un producto existente
            if (i > 0 && i % 10 == 0) {
                size_t indice_original = i - 1;
                p.nombre = catalogo[indice_original].nombre + "_Duplicado_Vendedor";
                for (size_t d = 0; d < dimensiones; ++d) {
                    p.embedding[d] = catalogo[indice_original].embedding[d] + dist_ruido(gen);
                }
            } else {
                // Producto totalmente nuevo
                for (size_t d = 0; d < dimensiones; ++d) {
                    p.embedding[d] = dist_base(gen);
                }
            }
            catalogo.push_back(p);
        }
        return catalogo;
    }
};

#endif
