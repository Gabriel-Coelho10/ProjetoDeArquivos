#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

// 1. Sua função de comparação (qsort e Intercalação)
int compara(const void *e1, const void *e2)
{
	return strncmp(((Endereco*)e1)->cep, ((Endereco*)e2)->cep, 8);
}

// =========================================================================
// 2. intercala.c em funcao
// =========================================================================
void intercala_arquivos(const char *nome_a, const char *nome_b, const char *nome_saida)
{
	FILE *a = fopen(nome_a, "rb");
	FILE *b = fopen(nome_b, "rb");
	FILE *saida = fopen(nome_saida, "wb");

	if (!a || !b || !saida) return;

	Endereco ea, eb;

	fread(&ea, sizeof(Endereco), 1, a);
	fread(&eb, sizeof(Endereco), 1, b);

	// eof -> End Of File
	while(!feof(a) && !feof(b))
	{
		if(compara(&ea, &eb) < 0) // ea < eb
		{
			fwrite(&ea, sizeof(Endereco), 1, saida);
			fread(&ea, sizeof(Endereco), 1, a);
		}
		else // ea >= eb
		{
			fwrite(&eb, sizeof(Endereco), 1, saida);
			fread(&eb, sizeof(Endereco), 1, b);
		}
	}

	while(!feof(a))
	{
		fwrite(&ea, sizeof(Endereco), 1, saida);
		fread(&ea, sizeof(Endereco), 1, a);		
	}
	
	while(!feof(b))
	{
		fwrite(&eb, sizeof(Endereco), 1, saida);
		fread(&eb, sizeof(Endereco), 1, b);		
	}

	fclose(a);
	fclose(b);
	fclose(saida);
}


void ordenacao_externa(const char *arquivo_origem, int k_blocos) 
{
    FILE *f_orig = fopen(arquivo_origem, "rb");
    if (!f_orig) {
        printf("Arquivo %s nao encontrado!\n", arquivo_origem);
        return;
    }

    
    fseek(f_orig, 0, SEEK_END);
    long total_registros = ftell(f_orig) / sizeof(Endereco);
    rewind(f_orig);

    long registros_por_bloco = total_registros / k_blocos;
    char nomes_blocos[k_blocos][50]; // Guarda o nome dos arquivinhos
    
    printf("--- INICIANDO DIVISAO EM %d BLOCOS ---\n", k_blocos);

    for (int i = 0; i < k_blocos; i++) {
        
        long qtd_ler = (i == k_blocos - 1) ? (total_registros - (i * registros_por_bloco)) : registros_por_bloco;

        Endereco *ram_buffer = (Endereco*) malloc(qtd_ler * sizeof(Endereco));
        
        fread(ram_buffer, sizeof(Endereco), qtd_ler, f_orig);
        qsort(ram_buffer, qtd_ler, sizeof(Endereco), compara); // Ordena na RAM

    
        sprintf(nomes_blocos[i], "bloco_%d.dat", i);
        FILE *f_bloco = fopen(nomes_blocos[i], "wb");
        fwrite(ram_buffer, sizeof(Endereco), qtd_ler, f_bloco);
        fclose(f_bloco);
        
        free(ram_buffer);
    }
    fclose(f_orig);
    printf("Arquivos temporarios criados com sucesso!\n");

  
    printf("\n--- INICIANDO INTERCALACAO (MERGE) ---\n");
    int num_ativos = k_blocos;
    int rodada = 0;

    
    while (num_ativos > 1) {
        int novos_ativos = 0;

        // Pula de 2 em 2
        for (int i = 0; i < num_ativos; i += 2) {
            if (i + 1 < num_ativos) {
                
                char nome_saida[50];
                sprintf(nome_saida, "temp_r%d_b%d.dat", rodada, novos_ativos);
                
               
                intercala_arquivos(nomes_blocos[i], nomes_blocos[i+1], nome_saida);
                
               
                remove(nomes_blocos[i]);
                remove(nomes_blocos[i+1]);

               
                strcpy(nomes_blocos[novos_ativos], nome_saida);
                novos_ativos++;
            } else {
                
                strcpy(nomes_blocos[novos_ativos], nomes_blocos[i]);
                novos_ativos++;
            }
        }
        num_ativos = novos_ativos;
        rodada++;
        printf("Rodada %d concluida. Arquivos restantes: %d\n", rodada, num_ativos);
    }

    
    rename(nomes_blocos[0], "cep_ordenado_final.dat");
    printf("\nOrdenacao Externa Concluida! Arquivo salvo como 'cep_ordenado_final.dat'\n");
}

int main(int argc, char** argv) 
{
   
    ordenacao_externa("cep.dat", 8);
    
    return 0;
}