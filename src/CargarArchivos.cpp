#ifndef CHM_CPP
#define CHM_CPP

#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <pthread.h>
#include <thread>

#include "CargarArchivos.hpp"

int cargarArchivo(
    HashMapConcurrente &hashMap,
    std::string filePath
) {
    std::fstream file;
    int cant = 0;
    std::string palabraActual;

    // Abro el archivo.
    file.open(filePath, file.in);
    if (!file.is_open()) {
        std::cerr << "Error al abrir el archivo '" << filePath << "'" << std::endl;
        return -1;
    }
    while (file >> palabraActual) {
        // Completar (Ejercicio 4)
        hashMap.incrementar(palabraActual);
        cant++;
    }
    // Cierro el archivo.
    if (!file.eof()) {
        std::cerr << "Error al leer el archivo" << std::endl;
        file.close();
        return -1;
    }
    file.close();
    return cant;
}

void cargarArchivoDeUnThread(HashMapConcurrente &hashMap, std::atomic<int> &pool, std::vector<std::string> &filePaths) {
    int idx;
    while (pool.load() >= 0) {
        idx = pool.fetch_sub(1, std::memory_order_relaxed);
        cargarArchivo(hashMap, filePaths[idx]);
    }
}


void cargarMultiplesArchivos(
    HashMapConcurrente &hashMap,
    unsigned int cantThreads,
    std::vector<std::string> filePaths
) {
    // Borramos cualquier tipo de fichero duplicado que nos hayan pasado.
    filePaths.erase( std::unique( filePaths.begin(), filePaths.end() ), filePaths.end() );


    std::atomic<int> cantFiles{filePaths.size() - 1};

    
    std::thread threads[cantThreads];
    for (unsigned int i = 0; i < cantThreads; i++) {
        threads[i] = std::thread(&cargarArchivoDeUnThread, std::ref(hashMap), std::ref(cantFiles), std::ref(filePaths));
    }

    // Esperar a que terminen todos los hilos
    for (unsigned int i = 0; i < cantThreads; i++) {
        threads[i].join();
    }
}

#endif
