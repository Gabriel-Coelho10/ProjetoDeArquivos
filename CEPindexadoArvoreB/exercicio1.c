#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Inclui o cabeçalho da tua biblioteca real
#include "ArvoreB.h" 

typedef struct _Endereco Endereco;

struct _Endereco
{
    char logradouro[72];
    char bairro[72];
    char cidade[72];
    char uf[72];
    char sigla[2];
    char cep[8];
    char lixo[2];
};

int main(int argc, char** argv)
{
    // Verifica os argumentos
    if (argc != 5) {
        printf("Uso correto: %s <arquivo_a1.dat> <arquivo_a2.dat> <arquivo_saida.dat> <indice.idx>\n", argv[0]);
        printf("Exemplo: %s cep_80_porcento.dat cep_80_porcento2.dat intersecao.dat arvore.idx\n", argv[0]);
        return 1;
    }

    FILE *arq_a1, *arq_a2, *arq_saida;
    Endereco e;

    /* ==========================================================
       FASE 2: INDEXAÇÃO DO ARQUIVO A2
       ========================================================== */
    printf("-> A iniciar a FASE 2: Indexacao do Ficheiro 2...\n");
    
    arq_a2 = fopen(argv[2], "rb");
    if (arq_a2 == NULL) {
        printf("Erro ao abrir o ficheiro a2.\n");
        return 1;
    }

    // Usando a funcao real do teu ArvoreB.h
    ArvoreB* arvore = ArvoreB_Abre(argv[4]); 
    if (arvore == NULL) {
        printf("Erro ao inicializar a Arvore B.\n");
        fclose(arq_a2);
        return 1;
    }

    long posicao = 0; 
    
    // Lê o arquivo a2 e insere na árvore
    while (fread(&e, sizeof(Endereco), 1, arq_a2) == 1) {
        // Envia o e.cep DIRETAMENTE porque a tua arvore aceita char[8]
        ArvoreB_Insere(arvore, e.cep, posicao);
        posicao++;
    }
    fclose(arq_a2);
    printf("   Indexacao concluida! %ld registos indexados.\n\n", posicao);


    /* ==========================================================
       FASE 3: JUNÇÃO (INTERSECÇÃO) DOS ARQUIVOS
       ========================================================== */
    printf("-> A iniciar a FASE 3: Juncao dos ficheiros (Interseccao)...\n");
    
    arq_a1 = fopen(argv[1], "rb");
    if (arq_a1 == NULL) {
        printf("Erro ao abrir o ficheiro a1.\n");
        return 1;
    }

    arq_saida = fopen(argv[3], "wb");
    if (arq_saida == NULL) {
        printf("Erro ao criar o ficheiro de saida.\n");
        fclose(arq_a1);
        return 1;
    }

    int total_interseccao = 0;
    int total_lido_a1 = 0;

    // Para cada endereço no arquivo a1...
    while (fread(&e, sizeof(Endereco), 1, arq_a1) == 1) {
        total_lido_a1++;
        
        // ...vamos procurar o CEP na árvore do arquivo a2.
        // A tua função devolve -1 se o CEP nao existir na arvore.
        long offset_encontrado = ArvoreB_Busca(arvore, e.cep);
        
        if (offset_encontrado != -1) {
            // O CEP está em ambos os ficheiros! 
            // Grava o endereço no ficheiro de saída
            fwrite(&e, sizeof(Endereco), 1, arq_saida);
            total_interseccao++;
        }
    }

    // Fechar tudo usando as tuas funcoes
    fclose(arq_a1);
    fclose(arq_saida);
    ArvoreB_Fecha(arvore);

    // Feedback final
    printf("   Juncao concluida com sucesso!\n");
    printf("   - Lidos no ficheiro a1: %d\n", total_lido_a1);
    printf("   - Total de CEPs em comum (Interseccao): %d\n", total_interseccao);

    return 0;
}