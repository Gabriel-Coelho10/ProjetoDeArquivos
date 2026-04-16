#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 1. As Estruturas
typedef struct _Endereco {
    char logradouro[72];
    char bairro[72];
    char cidade[72];
    char uf[72];
    char sigla[2];
    char cep[8];
    char lixo[2];
} Endereco;

// A NOVA estrutura de Índice (Leve e rápida)
typedef struct _Indice {
    char cep[8];
    long posicao; // Guarda a "página" (linha) no arquivo original
} Indice;

// Função para o qsort ordenar o nosso array de Índices pelo CEP
int compara_indice(const void *e1, const void *e2) {
    return strncmp(((Indice*)e1)->cep, ((Indice*)e2)->cep, 8);
}

int main(int argc, char** argv) {
    if(argc != 2) {
        fprintf(stderr, "USO: %s [CEP_PARA_BUSCAR]\n", argv[0]);
        return 1;
    }

    FILE *f_original, *f_indice;
    Endereco e;
    long totalRegistros, i;


    f_original = fopen("cep.dat", "rb");
    if(!f_original) {
        printf("Erro: Arquivo cep.dat nao encontrado!\n");
        return 1;
    }

    // Descobre quantas pessoas tem no arquivo
    fseek(f_original, 0, SEEK_END);
    totalRegistros = ftell(f_original) / sizeof(Endereco);
    rewind(f_original);

    // Aloca a memória SÓ para os índices (muito mais leve)
    Indice *array_indices = (Indice*) malloc(totalRegistros * sizeof(Indice));

    // Preenche o array de índices lendo o arquivo original
    for(i = 0; i < totalRegistros; i++) {
        fread(&e, sizeof(Endereco), 1, f_original);
        strncpy(array_indices[i].cep, e.cep, 8);
        array_indices[i].posicao = i; // Salva qual era a linha original dessa pessoa
    }

    qsort(array_indices, totalRegistros, sizeof(Indice), compara_indice);

    f_indice = fopen("cep_indice.dat", "wb");
    fwrite(array_indices, sizeof(Indice), totalRegistros, f_indice);
    
    fclose(f_indice);
    free(array_indices); // Limpa a RAM, o trabalho pesado acabou.

    // =========================================================================
    // ETAPA 4 e 5: Busca Binária no Índice e Recuperação dos Dados
    // =========================================================================
    
    f_indice = fopen("cep_indice.dat", "rb");
    
    long inicio = 0;
    long fim = totalRegistros - 1;
    long meio;
    Indice idx_lido;
    int achou = 0;

    // Busca binária acontecendo SÓ no arquivo de índice (pequeno)
    while(inicio <= fim) {
        meio = (inicio + fim) / 2;
        
        fseek(f_indice, meio * sizeof(Indice), SEEK_SET);
        fread(&idx_lido, sizeof(Indice), 1, f_indice);
        
        if(strncmp(argv[1], idx_lido.cep, 8) < 0) {
            fim = meio - 1;
        } 
        else if (strncmp(argv[1], idx_lido.cep, 8) > 0) {
            inicio = meio + 1;
        } 
        else {
            // ACHOU O CEP NO ÍNDICE!
            achou = 1;
            printf("CEP Encontrado no Indice! A posicao original e: %ld\n\n", idx_lido.posicao);
            
            // Passo 5: Recuperar no arquivo original usando o salto cirúrgico
            fseek(f_original, idx_lido.posicao * sizeof(Endereco), SEEK_SET);
            fread(&e, sizeof(Endereco), 1, f_original);
            
            printf("--- DADOS COMPLETOS RECUPERADOS ---\n");
            printf("Rua: %.72s\nBairro: %.72s\nCidade: %.72s\nUF: %.72s\n", 
                    e.logradouro, e.bairro, e.cidade, e.uf);
            break;
        }        
    }

    if(!achou) {
        printf("O CEP %s nao foi encontrado na base de dados.\n", argv[1]);
    }

    fclose(f_original);
    fclose(f_indice);
    
    return 0;
}
