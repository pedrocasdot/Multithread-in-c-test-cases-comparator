#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#define MAX_STR 255
#define MAX_CASES 998
#define compasso_de_espera(x) sleep(x)

pthread_mutex_t pthread_mutex;

/*
    Estrutura criada para a partilha de dados entre tarefas,
    afim de apresentar no final da execução de todas as tarefas
*/
typedef struct {
    long int matriz_erros[MAX_CASES][5];//Matriz para armazenar os casos de teste errado
    int casos_errados; //Variavel para contar o numero de casos de teste errados
    int numero_de_casos; // Variavel para contar o numero de casos de teste no ficheiro <casos.txt>
}partilha;
long int matriz_casos[MAX_CASES][3];//Matriz para armazenar todos os casos de teste
char  a[MAX_STR], b[MAX_STR], c[MAX_STR];// n, r, valor esperado
int teste_i;

/*
    Funcao para testar os casos de teste
*/
long int combinacoes(long int n, long int r)
{
    int i;
    long int combinacoes;
    if (n < r || r < 1)
        return 0;
    i = 1;
    combinacoes = 1;
    while (i <= r)
    {
        combinacoes *= (n - r + i);
        combinacoes /= i;
        i++;
    }
    return combinacoes;
}

void bloquear_tarefa(pthread_mutex_t thread_mutex){
    pthread_mutex_lock(&pthread_mutex);
}
void desbloquear_tarefa(pthread_mutex_t thread_mutex){
    pthread_mutex_unlock(&pthread_mutex);
}

void* testar_casos(void *args)
{
    long int retorno_combinacoes = 0;
    for( ; teste_i <((partilha *)args)->numero_de_casos; )
    {
        /*
            Impor limite de acesso aos casos de testes quando estiverem a ser
            executadas multiplas tarefas
        */
        bloquear_tarefa(pthread_mutex);
        //Retorno da funcao Combinacao ao passar os valores do arquivo <casos.txt>
        long int r = matriz_casos[teste_i][0];
        long int n = matriz_casos[teste_i][1];
        long int valor_esperado = matriz_casos[teste_i][2];//Valor esperado
        retorno_combinacoes = combinacoes(r, n);// Valor obtido
        //Se valor_esperado eh diferente de retorn_combinacoes entao caso de teste esta errado
        if(retorno_combinacoes != valor_esperado){
            //Entrou significa que tem algum caso errado
            /*
                Contar casos errados
            */
            ((partilha *)args)->casos_errados++;
             
            ((partilha *)args)->matriz_erros[teste_i][0] = matriz_casos[teste_i][0];
            ((partilha *)args)->matriz_erros[teste_i][1] = matriz_casos[teste_i][1];
            ((partilha *)args)->matriz_erros[teste_i][2] = matriz_casos[teste_i][2];
            ((partilha *)args)->matriz_erros[teste_i][3] = retorno_combinacoes;
            ((partilha *)args)->matriz_erros[teste_i][4] = 1;
            
        }
        
        //Quando os casos de teste terminarem retorna NULL e pode desbloquear as tarefas
        if(teste_i == ((partilha *)args)->numero_de_casos-1){
            desbloquear_tarefa(pthread_mutex);
            return NULL;    
        }
        teste_i++;
        //Tarefa(s) desbloqueada
        desbloquear_tarefa(pthread_mutex);
        /*
            Este compasso_de_espera serve simplesmente para notar a
            perfomance ao alterar o numero de tarefas, e sabe-se que quanto
            mais próximo a qtd_tarefas estiver da qtd_casos_de_teste mais rápido
            serão testados os casos de teste.

            Isto porque as tarefas podem ser sincronizadas para trabalharem mutuamente
        */
        compasso_de_espera(2);
    }
    return NULL;
}

void apresentar_resultados(int qtd_casos, partilha resultados){
    
    printf("Resultados: \n");
    printf("Sucesso: %d\n", qtd_casos-resultados.casos_errados);
    printf("Falhas: %d\n", resultados.casos_errados);
    printf("Total casos: %d.\n", qtd_casos);
    int i = 0;
    long int n, r, valor_obtido, valor_esperado, caso_err;
    while(i < qtd_casos){
        n =resultados.matriz_erros[i][0];
        r =resultados.matriz_erros[i][1];
        valor_esperado =resultados.matriz_erros[i][2];
        valor_obtido =resultados.matriz_erros[i][3];
        caso_err =resultados.matriz_erros[i][4];
        if(caso_err){
            printf("Erro no caso %d: input (%ld, %ld) esperado/observado: %ld / %ld.\n", i, n, r, valor_esperado, valor_obtido);
        }
        i++;
    }
}

int main(int argc, char **argv)
{
    if (argc != 3 || atoi(argv[1]) <= 0) {
        if(atoi(argv[1]) <= 0)
            puts("O numero de tarefas deve ser >= que 0");
        printf("Att: pteste <ntarefas> <casos.txt>\n");
        return 0;
    }
    int qtd_casos,qtd_tarefas, indice_tarefa;  
    int reader = 0, i;
    partilha resultados;
    //Zerar a matriz
    for(i = 0; i<MAX_CASES; i++){
        (resultados.matriz_erros[i][0]) = 0;
        (resultados.matriz_erros[i][1]) = 0;
        (resultados.matriz_erros[i][2]) = 0;
        (resultados.matriz_erros[i][3]) = 0;
        (resultados.matriz_erros[i][4]) = 0;
    }
    resultados.casos_errados = 0;
    resultados.numero_de_casos = 0;
    
    /*
        Quantidade de tarefas passados por argumento
    */
    qtd_tarefas = atoi(argv[1]);
    
    pthread_mutex_init(&pthread_mutex,NULL);
    /*
        Declaracao de Multitarefa
    */
    pthread_t tarefas[qtd_tarefas];
    
    /*
        Acessar o ficheiro de casos de teste
    */
    FILE* fp = fopen(argv[2], "rt");
    if(fp != NULL){
        // Ler o ficheiro casos.txt    
        while ((reader = fscanf(fp, "%s %s %s", a, b, c)) != EOF) {
            if (reader == 3) {
                //Guardar todos os casos de teste em uma matriz
                // n r resultado_esperado
                matriz_casos[qtd_casos][0] =atol(a); //n 
                matriz_casos[qtd_casos][1] =atol(b); //r
                matriz_casos[qtd_casos][2] =atol(c); //valor esperado
                qtd_casos++;//Contar caso de testes
            }
        }
        fclose(fp);
        resultados.numero_de_casos = qtd_casos;
        /* 
            Loop para o numero de tarefas a serem executados para testar os casos de teste
        */
        while(indice_tarefa < qtd_tarefas){
            pthread_create(&tarefas[indice_tarefa], NULL, (void *)testar_casos, &resultados);
            indice_tarefa++;
        }
        /*
            Agudar pelo termino da thread em execucao
        */
        indice_tarefa = 0;
        while(indice_tarefa < qtd_tarefas){
            pthread_join(tarefas[indice_tarefa], NULL);
            indice_tarefa++;
        }
        
        //Apresentar os resultados depois de todas tarefas serem executadas
        apresentar_resultados(qtd_casos, resultados);
        
    }
    else{
        printf("Falha ao acessar os casos de teste.\n");
    }

    return 0;
}