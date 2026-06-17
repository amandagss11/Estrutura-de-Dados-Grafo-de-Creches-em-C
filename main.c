
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_CRECHES 20

// Matriz estÃ¡tica de adjacÃªncias (indicando 0 ou 1)
int matrizAdjacencia[MAX_CRECHES][MAX_CRECHES];
int totalCreches = 0;

// Estrutura para armazenar a distÃ¢ncia de uma conexÃ£o especÃ­fica
typedef struct Conexao {
    int idDestino;
    float distancia;
    struct Conexao* proximo;
} Conexao;

// Estrutura da Lista Simplesmente Encadeada para as Creches
typedef struct Creche {
    int id;
    char nome[50];
    Conexao* conexoes; // Sublista encadeada com as distÃ¢ncias para os vizinhos
    struct Creche* proximo;
} Creche;

Creche* headCreches = NULL;

// --- FUNÃ‡Ã•ES DA LISTA ENCADEADA ---

// Busca uma creche pelo nome e retorna seu ID. Se nÃ£o existir, insere na lista.
int obterOuInserirCreche(const char* nome) {
    Creche* atual = headCreches;
    Creche* anterior = NULL;

    while (atual != NULL) {
        if (strcmp(atual->nome, nome) == 0) {
            return atual->id;
        }
        anterior = atual;
        atual = atual->proximo;
    }

    // Se nÃ£o encontrou, cria uma nova creche
    Creche* nova = (Creche*)malloc(sizeof(Creche));
    nova->id = totalCreches++;
    strcpy(nova->nome, nome);
    nova->conexoes = NULL;
    nova->proximo = NULL;

    if (anterior == NULL) {
        headCreches = nova;
    } else {
        anterior->proximo = nova;
    }

    return nova->id;
}

// Retorna o nome da creche a partir do ID
const char* obterNomePorId(int id) {
    Creche* atual = headCreches;
    while (atual != NULL) {
        if (atual->id == id) return atual->nome;
        atual = atual->proximo;
    }
    return "Desconhecido";
}

// Retorna o ponteiro da creche a partir do ID
Creche* obterCrechePorId(int id) {
    Creche* atual = headCreches;
    while (atual != NULL) {
        if (atual->id == id) return atual;
        atual = atual->proximo;
    }
    return NULL;
}

// Insere uma nova conexÃ£o (distÃ¢ncia) na sublista de uma creche de forma ordenada (crescente)
void inserirConexaoEncadeada(int idOrigem, int idDestino, float distancia) {
    Creche* creche = obterCrechePorId(idOrigem);
    if (!creche) return;

    Conexao* nova = (Conexao*)malloc(sizeof(Conexao));
    nova->idDestino = idDestino;
    nova->distancia = distancia;
    nova->proximo = NULL;

    // InserÃ§Ã£o ordenada por distÃ¢ncia (Crescente)
    Conexao* atual = creche->conexoes;
    Conexao* anterior = NULL;

    while (atual != NULL && atual->distancia < distancia) {
        anterior = atual;
        atual = atual->proximo;
    }

    if (anterior == NULL) {
        nova->proximo = creche->conexoes;
        creche->conexoes = nova;
    } else {
        nova->proximo = atual;
        anterior->proximo = nova;
    }
}

// Adiciona uma nova conexÃ£o completa no sistema (Matriz + Lista Encadeada)
void adicionarConexao(const char* origem, const char* destino, float distancia) {
    int idOrigem = obterOuInserirCreche(origem);
    int idDestino = obterOuInserirCreche(destino);

    // Atualiza a matriz estÃ¡tica com 1 (existe conexÃ£o)
    matrizAdjacencia[idOrigem][idDestino] = 1;
    matrizAdjacencia[idDestino][idOrigem] = 1;

    // Atualiza as listas encadeadas de distÃ¢ncias (Ida e Volta - Grafo NÃ£o Dirigido)
    inserirConexaoEncadeada(idOrigem, idDestino, distancia);
    inserirConexaoEncadeada(idDestino, idOrigem, distancia);
}

// --- REQUISITOS DO PROJETO ---

// 1. Ler os dados do grafo a partir de um arquivo TXT
void lerArquivoGrafo(const char* nomeArquivo) {
    FILE* arquivo = fopen(nomeArquivo, "r");
    if (!arquivo) {
        printf("Erro ao abrir o arquivo %s. Certifique-se de que ele existe.\n", nomeArquivo);
        return;
    }

    char origem[50], destino[50];
    float distancia;

    // Formato esperado no TXT: Origem Destino Distancia
    // Exemplo: JoanaTimoteo AmaroCavalcante 2.9
    while (fscanf(arquivo, "%s %s %f", origem, destino, &distancia) != EOF) {
        adicionarConexao(origem, destino, distancia);
    }

    fclose(arquivo);
    printf("Dados do grafo carregados com sucesso!\n");
}

// 2. Informar o nÃºmero de conexÃµes possÃ­veis partindo de cada creche (usando a Matriz EstÃ¡tica)
void numConexoesPorCreche() {
    printf("\n--- NUMERO DE CONEXOES POR CRECHE (Via Matriz Estatica) ---\n");
    Creche* atual = headCreches;
    while (atual != NULL) {
        int cont = 0;
        for (int j = 0; j < totalCreches; j++) {
            if (matrizAdjacencia[atual->id][j] == 1) {
                cont++;
            }
        }
        printf("Creche: %-25s | Conexoes: %d\n", atual->nome, cont);
        atual = atual->proximo;
    }
}

// 3. Listar para uma dada creche as conexÃµes em ordem crescente de distÃ¢ncia (via Lista Encadeada)
void listarConexoesOrdenadas(const char* nomeCreche) {
    Creche* atual = headCreches;
    while (atual != NULL) {
        if (strcmp(atual->nome, nomeCreche) == 0) {
            printf("\nConexoes partindo de '%s' (Ordem Crescente de Distancia):\n", nomeCreche);
            Conexao* con = atual->conexoes;
            if (!con) {
                printf("  Nenhuma conexao cadastrada.\n");
                return;
            }
            while (con != NULL) {
                printf("  -> %-25s : %.1f Km\n", obterNomePorId(con->idDestino), con->distancia);
                con = con->proximo;
            }
            return;
        }
        atual = atual->proximo;
    }
    printf("Creche '%s' nao encontrada.\n", nomeCreche);
}

// 4. Informar a distÃ¢ncia entre duas creches informadas, se existir
void informarDistancia(const char* origem, const char* destino) {
    Creche* atual = headCreches;
    int idDestino = -1;

    // Descobre o ID do destino primeiro
    while (atual != NULL) {
        if (strcmp(atual->nome, destino) == 0) {
            idDestino = atual->id;
            break;
        }
        atual = atual->proximo;
    }

    if (idDestino == -1) {
        printf("Uma ou ambas as creches nao foram encontradas.\n");
        return;
    }

    // Busca na lista encadeada da origem
    atual = headCreches;
    while (atual != NULL) {
        if (strcmp(atual->nome, origem) == 0) {
            // Verifica na matriz primeiro se hÃ¡ conexÃ£o
            if (matrizAdjacencia[atual->id][idDestino] == 0) {
                printf("Nao existe conexao direta entre %s e %s.\n", origem, destino);
                return;
            }

            // Busca o valor real da distÃ¢ncia na lista encadeada
            Conexao* con = atual->conexoes;
            while (con != NULL) {
                if (con->idDestino == idDestino) {
                    printf("A distancia entre %s e %s eh de: %.1f Km\n", origem, destino, con->distancia);
                    return;
                }
                con = con->proximo;
            }
        }
        atual = atual->proximo;
    }
    printf("Uma ou ambas as creches nao foram encontradas.\n");
}

// --- FUNÃ‡ÃƒO PRINCIPAL / INTERFACE COM USUÃRIO ---
// --- FUNÇÃO PRINCIPAL / INTERFACE COM USUÁRIO ---

int main() {
    // Inicializa matriz de adjacências com zeros
    for (int i = 0; i < MAX_CRECHES; i++) {
        for (int j = 0; j < MAX_CRECHES; j++) {
            matrizAdjacencia[i][j] = 0;
        }
    }

    // Carrega o arquivo txt com base nos dados do PDF
    lerArquivoGrafo("grafo.txt");

    int opcao;
    char op1[50], op2[50];
    float dist;

    do {
        printf("\n============================================\n");
        printf("         SISTEMA DE MERENDAS - CRECHES       \n");
        printf("============================================\n");
        printf("1. Exibir numero de conexoes de cada creche\n");
        printf("2. Listar conexoes de uma creche (Ordem Crescente)\n");
        printf("3. Consultar distancia entre duas creches\n");
        printf("4. Incluir nova conexao manual\n");
        printf("0. Sair\n");
        printf("Escolha uma opcao: ");
        
        // Espaço antes do %d ajuda a ignorar quebras de linha residuais no menu
        if (scanf(" %d", &opcao) != 1) {
            printf("Entrada invalida!\n");
            // Limpa buffer se o usuário digitar uma letra por engano
            while (getchar() != '\n');
            opcao = -1;
            continue;
        }

        switch (opcao) {
            case 1:
                numConexoesPorCreche();
                break;
            case 2:
                printf("Digite o nome da creche: ");
                scanf("%s", op1);
                listarConexoesOrdenadas(op1);
                break;
            case 3:
                printf("Digite o nome da creche de Origem: ");
                scanf("%s", op1);
                printf("Digite o nome da creche de Destino: ");
                scanf("%s", op2);
                informarDistancia(op1, op2);
                break;
            case 4:
                printf("Nome da Creche A: ");
                scanf("%s", op1);
                printf("Nome da Creche B: ");
                scanf("%s", op2);
                printf("Distancia em Km: ");
                scanf("%f", &dist);
                
                adicionarConexao(op1, op2, dist);
                printf("Nova conexao incluida com sucesso!\n");
                
                // Limpa o buffer do teclado para o menu não passar direto
                while (getchar() != '\n'); 
                break;
                
            case 0:
                printf("Encerrando o programa.\n");
                break;
            default:
                printf("Opcao invalida!\n");
        }
    } while (opcao != 0);

    return 0;
}
