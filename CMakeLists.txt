cmake_minimum_required(VERSION 3.15)
project(ECommerceMetricCatalog VERSION 1.0.0 LANGUAGES CXX)

# Forzar C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Opciones de compilación (Warnings como errores para garantizar código limpio)
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Werror -pedantic)
endif()

# Asegurar la inclusión de tu carpeta de cabeceras
include_directories(include)

# Compilar únicamente el ejecutable actual de la Actividad de E-Commerce
add_executable(ecommerce_runner 
    src/main.cpp
)

