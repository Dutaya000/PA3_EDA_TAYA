# Catálogo de E-Commerce Métrico: Búsqueda por Similitud en C++17

**Curso:** Estructuras de Datos Avanzadas  
**Estudiante:** Timoteo Flavio Taya Ramos  
**Entorno de Desarrollo:** Ubuntu (WSL) / C++17 / CMake  

Este repositorio contiene una solución modular de software desarrollada en **C++17** para resolver problemas críticos de integridad, duplicidad y baja calidad en catálogos de comercio electrónico (*e-commerce*). 

El sistema mitiga los listados redundantes provocados por **vendedores que suben imágenes muy parecidas o registran artículos con errores tipográficos** accidentales. Para lograrlo, abandona las búsquedas exactas e implementa algoritmos de **búsqueda por similitud en espacios métricos**, optimizando drásticamente el rendimiento mediante mecanismos de **poda geométrica**.



##  Características Principales (Features)

*   **Indexamiento Visual Continuo (VP-Tree):** Implementación de un *Vantage Point Tree* balanceado estáticamente mediante el cálculo de la mediana para estructurar *embeddings* vectoriales de alta dimensionalidad (64D), simulando características visuales de productos.
*   **Corrección de Texto Discreto (BK-Tree):** Extensión modular de un *Burkhard-Keller Tree* adaptado para indexar cadenas de texto de forma dinámica, aislando errores ortográficos en los nombres de los productos en tiempo mínimo.
*   **Poda Matemática por Desigualdad Triangular:** Integración estricta de las propiedades del espacio métrico para descartar de forma determinista ramas enteras del árbol durante consultas por rango y de k vecinos más cercanos (k-NN), evitando cálculos innecesarios.
*   **Arquitectura Desacoplada y Extensible:** Diseño genérico basado en plantillas (*templates*) que separa por completo las estructuras indexadoras de las funciones de distancia matemáticas aplicadas.
*   **Instrumentación de Rendimiento Transparente:** Incorporación del patrón decorador `DistanceCounter` para auditar con precisión absoluta el número de evaluaciones analíticas de distancia y el tiempo de reloj en microsegundos frente a un *baseline* lineal por fuerza bruta.

## 📁 Estructura del Proyecto

El código está organizado siguiendo los estándares profesionales de la industria para proyectos nativos en C++, separando interfaces, lógicas e implementaciones:

```text
PA2_TAYA_TIMOTEO_EDA/
├── build/                      # Binarios y archivos temporales de compilación
├── include/                    # Archivos de cabecera (.hpp)
│   ├── benchmark/
│   │   └── benchmark_runner.hpp# Orquestador de las pruebas de rendimiento
│   ├── core/
│   │   ├── dataset.hpp         # Modelado de Producto y generación de catálogo
│   │   ├── distance_counter.hpp# Decorador para auditoría de rendimiento
│   │   ├── linear_scan.hpp     # Baseline obligatorio de Fuerza Bruta
│   │   └── query_result.hpp    # Estructura de captura de datos coincidentes
│   ├── index/
│   │   ├── bk_tree.hpp         # Árbol métrico discreto para cadenas
│   │   └── vp_tree.hpp         # Árbol métrico esférico para vectores 64D
│   └── metrics/
│       ├── euclidean_distance.hpp # Métrica continua para embeddings
│       ├── levenshtein_distance.hpp # Métrica discreta para texto
│       └── metric.hpp          # Interfaz abstracta pura base
├── src/                        # Archivos de implementación (.cpp)
│   └── main.cpp                # Punto de entrada de la aplicación
├── CMakeLists.txt              # Configuración central de construcción de CMake
```

---

## 🛠️ Compilación y Ejecución (Flujo Build)

### Prerrequisitos Mínimos
- Compilador compatible con **C++17** (GCC 7+, Clang 5+ o MSVC 2017+).
- **CMake 3.15** o superior instalado en el sistema.

### Instrucciones Paso a Paso en la Terminal
Para limpiar la caché previa de CMake y ejecutar una compilación limpia desde cero, ejecuta la siguiente secuencia de comandos en la raíz de tu terminal:

```bash
# 1. Crear e ingresar al directorio de compilación
mkdir -p build && cd build

# 2. Eliminar cualquier residuo corrupto de configuraciones anteriores
rm -rf *

# 3. Generar los archivos nativos de construcción del proyecto
cmake ..

# 4. Compilar el ejecutable optimizado
cmake --build .

# 5. Ejecutar el benchmark para auditar las métricas en tiempo real
./ecommerce_runner
```

---

## 📊 Resumen Ejecutivo de Métricas Obtenidas

Los experimentos empíricos ejecutados sobre un catálogo simulado de **5,000 productos de alta dimensionalidad (64D)** arrojaron las siguientes conclusiones inmediatas de rendimiento:

- **Aceleración por Rango:** El **VP-Tree disminuyó un 90.7% las operaciones de distancia** (de 5,000 a 465), logrando resolver la consulta en apenas **218 microsegundos** frente a los 2,496 microsegundos del escaneo lineal por fuerza bruta.
- **Eficacia Textual del BK-Tree:** Ante cadenas con errores tipográficos críticos como *'Zapatilla Nike Nera'*, el índice discretizado aisló los duplicados homólogos (*'Zapatilla Nike Negra'* y *'Zapatila Nike Ngra'*) de forma instantánea mediante su distancia de edición Levenshtein.
