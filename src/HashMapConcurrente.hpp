#ifndef HMC_HPP
#define HMC_HPP

#include <atomic>
#include <string>
#include <vector>
#include <mutex>

#include "ListaAtomica.hpp"


typedef std::pair<std::string, unsigned int> hashMapPair;

struct thread_ctx {
    std::atomic<int> *letras;
    std::mutex *mtx_max;
    hashMapPair *maximoGlobal;
};

class HashMapConcurrente {
 public:
    static const unsigned int cantLetras = 26;

    HashMapConcurrente();

    HashMapConcurrente(const HashMapConcurrente & otro){}

    void incrementar(std::string clave);
    std::vector<std::string> claves();
    unsigned int valor(std::string clave);

    hashMapPair maximo();
    hashMapPair maximoParalelo(unsigned int cantThreads);

 private:
    ListaAtomica<hashMapPair> * tabla[HashMapConcurrente::cantLetras];

    hashMapPair* buscar(std::string clave);

    static unsigned int hashIndex(std::string clave);

    void maximoPorThread(thread_ctx *ctx);
    
    std::mutex hash_mutex[cantLetras];
    
    std::mutex mutex_insercion;
};

#endif  /* HMC_HPP */