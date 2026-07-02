#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

typedef struct _Endereco Endereco;

struct _Endereco
{
	char logradouro[72];
	char bairro[72];
	char cidade[72];
	char uf[72];
	char sigla[2];
	char cep[8];
	char lixo[2]; // Ao Espaço no final da linha + quebra de linha
};

int main(int argc, char**argv)
{
	FILE *entrada,*saida;
	Endereco e;
	entrada = fopen ("cep.dat","rb");
	saida = fopen("cep_80_porcento2.dat","wb");
	srand(time(NULL));
  while( fread(&e,sizeof(Endereco),1,entrada) ==1 ) {
		float random_num = 1.0*rand()/RAND_MAX;
        if(random_num < 0.8) {
            fwrite(&e, sizeof(Endereco), 1, saida);
        }
    }
	fclose(entrada);
	fclose(saida);
	return 0;
}

    