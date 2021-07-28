#include <iostream>
#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/wait.h>
#include <csignal>
#include <cstring>

using namespace std;

//Variáveis globais relativas aos comandos inputados
string com1, com2, arg1[50], arg2[50];
int numArg1, numArg2;

//-----------------------------------------------------------------------------------------------------

//Função para fork e exec
void executa_comando(string nomeComando, string argComando[50], int numArgumentos) {
    
    char* args[numArgumentos];

    args[0] = (char*)nomeComando.c_str();
    for(int i = 1; i <= numArgumentos; i++) {
        args[i] = (char*)argComando[i-1].c_str(); //https://www.geeksforgeeks.org/convert-string-char-array-cpp/
    }
    args[numArgumentos+1] = NULL;

    pid_t pid = fork();

    //Erro
    if (pid < 0) {
        perror("Fork falhou, favor executar novamente.");
        exit(1);
    }
    //Filho
    else if (pid == 0) {
        execvp(args[0], args); //https://www.youtube.com/watch?v=O1UOWScmqxg
    }
    
    //Pai
    wait(NULL);
    cout << "-----------------------------------------" << endl; 
    cout << "Aguardando novo sinal..." << endl;
    cout << "-----------------------------------------" << endl;  

}

//-----------------------------------------------------------------------------------------------------

//Função para o handler
void handlersignal(int sinal) {

        if(sinal == SIGUSR1) {
            executa_comando(com1, arg1, numArg1);
        } else if(sinal == SIGUSR2) {
            executa_comando(com2, arg2, numArg2);
        } else if(sinal == SIGTERM) {
            cout << "Finalizando o disparador..." << endl;
            cout << "-----------------------------------------" << endl; 
            exit(sinal);
        }
}

//------------------------------------------------------------------------------------------------------

int main()
{
//Pega os comandos do usuário
    cout << "Digite o primeiro comando (deve existir dentro do diretório /bin): ";
    getline(cin, com1);

    //Verifica se o comando inserido existe dentro do /bin. Caso não exista, o programa é finalizado e deve ser reiniciado.
    string path1 = "/bin/" + com1;
    ifstream fin1(path1);
    if (fin1.fail()) {
        cout << "O comando inserido não existe dentro do diretório /bin. Favor rodar o programa novamente." << endl;
        exit(1);
    }

    cout << "Quantos argumentos serão digitados para o primeiro comando? (máximo de 50 argumentos) ";

    //Verifica se o valor inserido para o número de argumentos é valido
    while (!(cin >> numArg1)) {         //https://www.youtube.com/watch?v=YIX7UhIKEIk
        cout << "Por favor, digite um número válido: ";
        cin.clear();
        cin.ignore(50, '\n');
    }
    
    //Loop para pegar os argumentos
    for(int i = 1; i <= numArg1; i++) {
        cout << "Digite o " << i << "º argumento: ";
        cin >> arg1[i-1];
    }
    
    cout << "--------------------" << endl; //divisão para melhorar visualização do programa

    cout << "Digite o segundo comando (deve existir dentro do diretório /bin): ";
    cin.ignore();
    getline(cin, com2);

    string path2 = "/bin/" + com2;
    ifstream fin2(path2);

    if (fin2.fail()) {
        cout << "O comando inserido não existe dentro do diretório /bin. Favor rodar o programa novamente." << endl;
        exit(1);
    }

    cout << "Quantos argumentos serão digitados para o segundo comando? (máximo de 50 argumentos) ";

    while (!(cin >> numArg2)) {
        cout << "Por favor, digite um número válido: ";
        cin.clear();
        cin.ignore(100, '\n');
    }

    for(int i = 1; i <= numArg2; i++) {
        cout << "Digite o " << i << "º argumento: ";
        cin >> arg2[i-1];
    }

    cout << "-----------------------------------------" << endl;
    
    //Registra sinais e signal handler
    signal(SIGUSR1, handlersignal);
    signal(SIGUSR2, handlersignal);
    signal(SIGTERM, handlersignal);

    //Informa PID e aguarda sinal do usuário
    cout << "PID:" << getpid() << ". Aguardando sinal..." << endl;
    while(1) {
        sleep(1);
    }

    //Informa sobre erros no sinal inputado
    if(signal(SIGUSR1, handlersignal) == SIG_ERR) {
        cout << "SIGUSR1 não recebido" << endl;
    } else if(signal(SIGUSR2, handlersignal) == SIG_ERR) {
        cout << "SIGUSR2 não recebido" << endl;
    } else if(signal(SIGTERM, handlersignal) == SIG_ERR) {
        cout << "SIGTERM não recebido" << endl;
    }

    return 0;
}

