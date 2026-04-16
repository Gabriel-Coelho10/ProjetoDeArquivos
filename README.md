# Busca de CEP em C (Sequencial vs Binária) 📍

Este projeto foi desenvolvido em **C** com o objetivo de demonstrar na prática a manipulação de ficheiros binários grandes e a drástica diferença de desempenho (performance) entre o algoritmo de **Busca Sequencial** e o de **Busca Binária**.

O programa realiza a leitura de um ficheiro de base de dados (`.dat`) contendo centenas de milhares de registos de endereços brasileiros e encontra as informações completas de um CEP específico inserido pelo utilizador.

## 🚀 O Problema e a Solução

O ficheiro de base de dados utilizado (`cep_ordenado.dat`) contém mais de **383.000 registos**. Cada registo é uma `struct` com exatamente 300 bytes.

* **Busca Sequencial (`BuscaCEP.c`):** O algoritmo lê o ficheiro linha a linha desde o início. No pior dos casos (se o CEP não existir ou for o último), o programa realiza **383.614 leituras** no disco, o que consome muito tempo de processamento.
* **Busca Binária (`BuscaCEPbinaria.c`):** O algoritmo tira proveito do facto de o ficheiro estar ordenado e divide o espaço de busca pela metade a cada iteração, utilizando os comandos `fseek` e `fread` para "saltar" diretamente para a posição calculada. O mesmo CEP é encontrado (ou descartado) em um máximo de **20 leituras**.

## 💻 Estrutura de Dados

A estrutura utilizada para ler os blocos exatos de 300 bytes do ficheiro é a seguinte:

```c
struct _Endereco
{
    char logradouro[72];
    char bairro[72];
    char cidade[72];
    char uf[72];
    char sigla[2];
    char cep[8];
    char lixo[2]; // Espaço no final da linha + quebra de linha
};
