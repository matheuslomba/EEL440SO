#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

using namespace std;

int numAlunos;
bool control = true;
std::mutex m;
std::condition_variable cv1, cv2;

//----------------------------------------------------------------------------------------------------

//Loop em que a cada ocorrência saem 5 alunos da sala e é verificado se numAlunos é igual a zero.
//Se for zero, o loop finaliza e o diretor pode sair da sala.
//Se não for zero, numAlunos vai diminuindo até a sala esvaziar.

void alunoSai () {

    std::unique_lock<std::mutex> lk(m);

    while (control) {
        numAlunos -= 5;

        if (numAlunos <= 0) {
            numAlunos = 0;
            cv2.notify_one();
            cv1.wait(lk);
        }

        cout << "Ainda há " << numAlunos << " alunos na sala." << endl;
        sleep(1);
        cv2.notify_one();
        cv1.wait(lk);
    }
}

void diretorSai () {

    std::unique_lock<std::mutex> lk(m);

    while(control) {
        if (numAlunos == 0) {
            cout << "Todos os alunos foram embora e, portanto, o Diretor também está saindo." << endl;
            std::abort();

        }else {
            cv1.notify_one();
            cv2.wait(lk);
        }
    }
}

//----------------------------------------------------------------------------------------------------

void alunoEntra () {

    //Reseta o número escolhido na última vez que o programa rodou
    srand(time(NULL));
    //Gera um número aleatório de alunos dentro da sala (entre 0 e 79)
    numAlunos = rand() % 80;

    //Caso o número de alunos esteja entre 0 e 50, o diretor não pode entrar na sala e, portanto, o programa finaliza para rodar novamente.
    if (numAlunos > 0 && numAlunos < 50) {
        cout << "O Diretor não pode entrar na sala no momento pois há " << numAlunos << " alunos." << endl;
        cout << "Rode o programa novamente para um novo caso." << endl;
        exit(1);
    }
}

void diretorEntra () {
    int t = 3;

    //0 alunos na sala -> o diretor pode entrar para realizar uma busca e, depois de um tempo, sai da sala e o programa finaliza.
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

    //50 ou mais alunos na sala -> o diretor pode entrar na sala para finalizar a festa e deve ficar lá até todos os alunos saírem.
    else if(numAlunos > 49) {
        cout << "Há " << numAlunos << " alunos na sala. O Diretor está entrando para finalizar a festa." << endl;
    }
}

//----------------------------------------------------------------------------------------------------

int main () {
    
    alunoEntra();
    diretorEntra();

    std::thread t1(alunoSai);
    std::thread t2(diretorSai);

    t1.join();
    t2.join();

    return 0;
}
