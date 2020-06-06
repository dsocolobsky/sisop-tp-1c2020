#ifndef CHM_CPP
#define CHM_CPP

#include <iostream>
#include <fstream>
#include <pthread.h>
#include <mutex>
#include <thread>
#include <string>

#include "HashMapConcurrente.hpp"

HashMapConcurrente::HashMapConcurrente() {
    for (unsigned int i = 0; i < HashMapConcurrente::cantLetras; i++) {
        tabla[i] = new ListaAtomica<hashMapPair>();
    }
}

unsigned int HashMapConcurrente::hashIndex(std::string clave) {
    return (unsigned int)(clave[0] - 'a');
}

void HashMapConcurrente::incrementar(std::string clave) {
    // Completar (Ejercicio 2)
    unsigned int idx = HashMapConcurrente::hashIndex(clave);
    std::mutex *mtx = &(hash_mutex[idx]);
    while (true) {
        hashMapPair *tupla = HashMapConcurrente::buscar(clave);
        if (tupla) {
            while(true) {
                if (mtx->try_lock()) { //TODO esto hay que verlo POR BUCKET
                    tupla->second++;
                    mtx->unlock();
                    return;
                }
                // Caso contrario, no pudimos agarrar el lock => seguimos intentando
            }
        } else {
            // No existe la tupla
            ListaAtomica<hashMapPair> *lista = tabla[idx];
            if (mtx->try_lock()) {
                lista->insertar(hashMapPair{clave, 1});
                mtx->unlock();
                return;
            }
            // Si el mtx esta bloqueado, se va a insertar la tupla tarde o temprano
            // en ese caso, no entramos al if y volvemos al inicio del loop, eventualmente
            // vamos a encontrar la tupla y a entrar a incrementarla.
        }
    }
}

std::vector<std::string> HashMapConcurrente::claves() {
    // Completar (Ejercicio 2)
    // NOTA: Nos fue dicho por Discord que en esta función debe ignorarse
    // el requerimiento de que sea no bloqueante y libre de espera (wait-free).
    std::vector<std::string> _claves;
    // Intento lockear todos los mutex
    unsigned int letra = 0;
    while(letra < HashMapConcurrente::cantLetras)
    {
        std::mutex *mtx = &(hash_mutex[letra]);
        if(mtx->try_lock())
        {
            ++letra;
        }
    }
    // Agregar mutex de "está insertando"
    for (unsigned int i = 0; i < HashMapConcurrente::cantLetras; i++) {
        ListaAtomica<hashMapPair> * lista = tabla[i];
        unsigned int longitud = lista->longitud();
        for(unsigned int j = 0; j < longitud; ++j) {
            _claves.push_back(lista->iesimo(j).first);
        }
    }
    // Desbloqueo todos los mutex
    for(unsigned int i = 0; i < HashMapConcurrente::cantLetras; ++i)
    {
        std::mutex *mtx = &(hash_mutex[i]);
        mtx->unlock();
    }
    return _claves;
}

unsigned int HashMapConcurrente::valor(std::string clave) {
    // Completar (Ejercicio 2)
    hashMapPair *tupla = HashMapConcurrente::buscar(clave);
    return tupla != nullptr ? tupla->second : 0;
}

hashMapPair HashMapConcurrente::maximo() {
    hashMapPair max{};
    max.second = 0;

    // Intento lockear todos los mutex
    unsigned int letra = 0;
    while(letra < HashMapConcurrente::cantLetras)
    {
        std::mutex *mtx = &(hash_mutex[letra]);
        if(mtx->try_lock())
        {
            ++letra;
        }
    }

    //Busco el máximo sin permitir inserciones en el medio
    for (unsigned int index = 0; index < HashMapConcurrente::cantLetras; index++) {
        for (
            auto it = tabla[index]->crearIt();
            it.haySiguiente();
            it.avanzar()
        ) {
            if (it.siguiente().second > max.second) {
                max.first = it.siguiente().first;
                max.second = it.siguiente().second;
            }
        }
    }

    // Desbloqueo todos los mutex
    for(unsigned int i = 0; i < HashMapConcurrente::cantLetras; ++i)
    {
        std::mutex *mtx = &(hash_mutex[i]);
        mtx->unlock();
    }

    return max;
}

void HashMapConcurrente::maximoPorThread(thread_ctx *ctx) {
    int letra;
    hashMapPair maximoLocal;

    while (ctx->letras->load() >= 0) {
        letra = ctx->letras->fetch_sub(1, std::memory_order_relaxed);
        // Buscar el máximo de esa lista
        // Guardar el máximo de la lista en un puntero a "máximo" de maximoParalelo (atómico para que no se pise)

        if(tabla[letra]->longitud() == 0) continue;

        // Si la lista no está vacía, el máximo local es el primero (por ahora)
        maximoLocal = tabla[letra]->iesimo(0);

        // Itero desde el segundo elemento buscando otro potencial máximo local
        for (int i = 1; i < tabla[letra]->longitud(); i++)
        {
            hashMapPair par = tabla[letra]->iesimo(i);
            if (par.second > maximoLocal.second)
            {
                maximoLocal = par;
            }
        }

        // Intentar acceder al mutex de maximo, y si es mayor el local => cambiarlo
        while(!ctx->mtx_max->try_lock()) { /* Espero */ };

        if (maximoLocal.second > ctx->maximoGlobal->second) {
            *(ctx->maximoGlobal) = maximoLocal;
        }
        
        ctx->mtx_max->unlock();
    }
    delete ctx;
}

hashMapPair HashMapConcurrente::maximoParalelo(unsigned int cantThreads) {
    // Completar (Ejercicio 3)

    std::mutex max_mutex;

    // Intento lockear todos los mutex
    unsigned int letra = 0;
    while(letra < HashMapConcurrente::cantLetras)
    {
        std::mutex *mtx = &(hash_mutex[letra]);
        if(mtx->try_lock())
        {
            ++letra;
        }
    }

    // Contexto de threads
    std::atomic<int> letras_disponibles{25};
    
    hashMapPair max_seen{"aaa", 1};

    std::thread threads[cantThreads];
    for (unsigned int i = 0; i < cantThreads; i++) {
        threads[i] = std::thread(&HashMapConcurrente::maximoPorThread,
            this, new thread_ctx{&letras_disponibles, &max_mutex, &max_seen});
    }

    // Esperar a que terminen todos los hilos
    for (unsigned int i = 0; i < cantThreads; i++) {
        threads[i].join();
    }

    // Desbloqueo todos los mutex
    for(unsigned int i = 0; i < HashMapConcurrente::cantLetras; ++i)
    {
        std::mutex *mtx = &(hash_mutex[i]);
        mtx->unlock();
    }

    return max_seen;
}


hashMapPair* HashMapConcurrente::buscar(std::string clave) {
    ListaAtomica<hashMapPair> *lista = tabla[hashIndex(clave)];
    for(unsigned int i = 0; i < lista->longitud(); ++i) {
        hashMapPair *tupla = &(lista->iesimo(i));
        if(tupla->first == clave) {
            return tupla;
        }
    }
    return nullptr;
}

#endif
