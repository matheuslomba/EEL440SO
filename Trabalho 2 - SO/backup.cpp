#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

using namespace std;

std::condition_variable cv;
std::mutex mu;

int numAlunos;
bool sairSala = false;

//----------------------------------------------------------------------------------------------------

void alunoSai () {
    //Função que esvazia a sala e faz o Diretor esperar até não ter nenhum aluno para que ele possa sair.
    cout << "Os alunos estão indo embora e o Diretor está esperando o local esvaziar para poder sair." << endl;
    while (1) {
        if (numAlunos > 0){
            cout << "Ainda há " << numAlunos << " alunos na sala." << endl;
            sleep(1);
        } else {
            cout << "Todos os alunos foram embora e, portanto, o Diretor também está saindo." << endl;
            exit(1);
        }
        numAlunos -= 5;
    }
}

//----------------------------------------------------------------------------------------------------

void alunoEntra () {
    std::lock_guard<std::mutex> lk(mu);
    
    //Reseta o número escolhido na última vez que o programa rodou
    srand(time(NULL));
    //Gera um número aleatório de alunos dentro da sala (entre 0 e 99)
    numAlunos = rand() % 80;

    if (numAlunos > 0 && numAlunos < 50) {
        cout << "O Diretor não pode entrar na sala no momento pois há " << numAlunos << " alunos." << endl;
        cout << "Rode o programa novamente para um novo caso." << endl;
        exit(1);
    }

    cv.notify_one();
}

void diretorEntra () {
    /*
    Se 0 alunos:
        Diretor entra
        cout << "Realizando busca"
        Diretor sai
    Se +50:
        Diretor entra cout << "Terminando a festa"
        Espera alunos saírem -> fazer um while em q a cada segundo umas 5 pessoas saem até sobrar 0 (so pela interatividade)
        Diretor sai
    Se 1 < alunos < 49:
        Diretor espera numAlunos ser =0 ou >50
    */

    int t = 3;
    std::unique_lock<std::mutex> lk(mu);
    cv.wait (lk, []{return (numAlunos == 0 || numAlunos > 49) ? true : false; });

    //0 alunos na sala
    if (numAlunos == 0){
        cout << "A sala está vazia, então o Diretor pode entrar para realizar a busca no local." << endl;
        while(t > 0) {
            cout << "Realizando busca..." << endl;
            sleep(1);
            t -= 1;
        }
        cout << "A busca foi realizada e o diretor está saindo da sala." << endl;
        exit(1);
    }

    //50 ou mais alunos na sala
    else if(numAlunos > 49) {
        cout << "Há " << numAlunos << " alunos na sala. O Diretor está entrando para finalizar a festa." << endl;
        alunoSai();
    }
}

//----------------------------------------------------------------------------------------------------

int main () {
    std::thread t1(alunoEntra);
    std::thread t2(diretorEntra);

    t1.join();
    t2.join();

    return 0;
}
