#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <math.h>

#define MAX_LIN 200
#define MAX_COL 200
#define LIMITE 99999

void gerar_mundo();
void mostrar_mundo();
void mover(char comando);
void mover_adversario();
int calcular_distancia(int x1, int y1, int x2, int y2);
void calcular_pontos();
void exibir_ranking();
void ordenar_ranking();

char mundo[MAX_LIN][MAX_COL];
int pos_x, pos_y, lin = 0, col = 0;
int adv_x, adv_y, obj_x, obj_y;
int nivel_dificuldade = 1;
int fase;
int vida, idx, contador_passos;
char escolha;

typedef struct{
    int x, y;
    int g, h, f;
    int pai_x, pai_y;
} Celula;

struct Participante{
    int score;
    char sigla[3];
};

struct Participante participantes[10] = {0};

int main(){
    srand(time(NULL));

    char comando;
    idx = 0;
    while(1){
        fase = 1;
        vida = 1;

        printf("\nEscolha o nível de dificuldade: \n");
        printf("1 - Fácil.");
        printf("\n2 - Médio.");
        printf("\n3 - Difícil.\n");
        do{
            scanf("%d", &nivel_dificuldade);
        }while(nivel_dificuldade < 1 || nivel_dificuldade > 3);

        while(fase <= 10){
            contador_passos = 0;
            gerar_mundo();

            while(1){
                mostrar_mundo();
                do{
                    scanf(" %c", &comando);
                    while(getchar() != '\n');
                    comando = tolower(comando);
                }while(comando != 'w' && comando != 'a' && comando != 's' && comando != 'd');

                mover(comando);

                if(pos_x == obj_x && pos_y == obj_y){
                    mundo[pos_x][pos_y] = 'V';
                    mostrar_mundo();
                    printf("Você encontrou o tesouro. Parabéns!\n\n");
                    calcular_pontos();
                    fase++;
                    break;
                }

                if(pos_x == adv_x && pos_y == adv_y){
                    if(vida > 1){
                        printf("\nO inimigo te achou! Boa sorte na próxima tentativa!");
                        vida--;
                        gerar_mundo();
                    }else{
                        printf("\nInimigo foi mais esperto! Fim de jogo!\n");
                        vida--;
                        break;
                    }
                }

                mover_adversario();

                if(pos_x == adv_x && pos_y == adv_y){
                   
                        printf("\nVocê foi encontrado pelo inimigo! Game over!\n");
                        vida--;
                        break;
                }

                system("cls");

                if(vida == 0){
                    break;
                }
            }

            if(vida == 0){
                break;
            }
        }

        exibir_ranking();
        idx++;
        printf("\n\nDeseja continuar? (s/n): \n");
        scanf(" %c", &escolha);
        if(tolower(escolha) == 'n')
            break;
    }

}

void gerar_mundo(){
    char caminho[50];
    sprintf(caminho, "fase%d.txt", fase);

    FILE *arquivo = fopen(caminho, "r");
    if (!arquivo) {
        printf("Erro ao abrir o arquivo do mapa!\n");
        return;
    }

    for (int i = 0; i < MAX_LIN; i++) {
        for (int j = 0; j < MAX_COL; j++) {
            mundo[i][j] = '\0';
        }
    }

    lin = 0;
    while (fgets(mundo[lin], MAX_COL, arquivo)) {
        int len = 0;
        while (mundo[lin][len] != '\0' && mundo[lin][len] != '\n') {
            len++;
        }
        if (len > col) {
            col = len;
        }
        lin++;
    }
    fclose(arquivo);

    int pos_livres[MAX_LIN * MAX_COL][2];
    int total_pos = 0;

    for (int i = 0; i < lin; i++) {
        for (int j = 0; j < col; j++) {
            if (mundo[i][j] == '.') {
                pos_livres[total_pos][0] = i;
                pos_livres[total_pos][1] = j;
                total_pos++;
            }
        }
    }

    if (total_pos == 0) {
        printf("Erro: Nenhuma posição livre encontrada no mapa.\n");
        return;
    }

    int pos_jogador = rand() % total_pos;
    pos_x = pos_livres[pos_jogador][0];
    pos_y = pos_livres[pos_jogador][1];
    mundo[pos_x][pos_y] = 'P';

    int pos_tesouro;
    do {
        pos_tesouro = rand() % total_pos;
    } while (pos_tesouro == pos_jogador);

    obj_x = pos_livres[pos_tesouro][0];
    obj_y = pos_livres[pos_tesouro][1];
    mundo[obj_x][obj_y] = 'X';

    if (nivel_dificuldade > 1) {
        int pos_inimigo;
        do {
            pos_inimigo = rand() % total_pos;
        } while (pos_inimigo == pos_jogador || pos_inimigo == pos_tesouro);

        adv_x = pos_livres[pos_inimigo][0];
        adv_y = pos_livres[pos_inimigo][1];
        mundo[adv_x][adv_y] = 'E';
    }
}

void mostrar_mundo(){
    printf("\t\t:::FASE-%d:::\n", fase);
    for(int i = 0; i < lin; i++){
        printf("%s", mundo[i]);
    }
    printf("\nDigite uma das entradas para movimentar-se no mapa: ");
    printf("\nw - Para cima.");
    printf("\na - Para a esquerda.");
    printf("\nd - Para a direita.");
    printf("\ns - Para baixo.\n");
}

void mover(char comando) {
    contador_passos++;

    if (comando == 'w' && pos_x > 0 && mundo[pos_x - 1][pos_y] != '#' && pos_x - 1 < MAX_LIN) {
        mundo[pos_x][pos_y] = '.';
        pos_x--;
        mundo[pos_x][pos_y] = 'P';
    }

    else if (comando == 's' && pos_x < lin - 1 && mundo[pos_x + 1][pos_y] != '#' && pos_x + 1 < MAX_LIN) {
        mundo[pos_x][pos_y] = '.';
        pos_x++;
        mundo[pos_x][pos_y] = 'P';
    }

    else if (comando == 'a' && pos_y > 0 && mundo[pos_x][pos_y - 1] != '#' && pos_y - 1 < MAX_COL) {
        mundo[pos_x][pos_y] = '.';
        pos_y--;
        mundo[pos_x][pos_y] = 'P';
    }

    else if (comando == 'd' && pos_y < col - 1 && mundo[pos_x][pos_y + 1] != '#' && pos_y + 1 < MAX_COL) {
        mundo[pos_x][pos_y] = '.';
        pos_y++;
        mundo[pos_x][pos_y] = 'P';
    }
}

void mover_adversario() {
    if (nivel_dificuldade == 2) {
        mundo[adv_x][adv_y] = '.';

        if (pos_x < adv_x && mundo[adv_x - 1][adv_y] != '#' && (adv_x - 1 != obj_x || adv_y != obj_y)) {
            adv_x--;
        } else if (pos_x > adv_x && mundo[adv_x + 1][adv_y] != '#' && (adv_x + 1 != obj_x || adv_y != obj_y)) {
            adv_x++;
        } else if (pos_y < adv_y && mundo[adv_x][adv_y - 1] != '#' && (adv_x != obj_x || adv_y - 1 != obj_y)) {
            adv_y--;
        } else if (pos_y > adv_y && mundo[adv_x][adv_y + 1] != '#' && (adv_x != obj_x || adv_y + 1 != obj_y)) {
            adv_y++;
        }

        mundo[adv_x][adv_y] = 'E';

    } else if (nivel_dificuldade == 3) {
        int visitados[MAX_LIN][MAX_COL] = {0};
        Celula aberto[MAX_LIN * MAX_COL];
        int inicio = 0, fim = 0;

        Celula inicial = {adv_x, adv_y, 0, calcular_distancia(adv_x, adv_y, pos_x, pos_y), 0, -1, -1};
        inicial.f = inicial.g + inicial.h;
        aberto[fim++] = inicial;

        Celula melhor = inicial;

        while (inicio < fim) {
            int melhor_indice = inicio;
            for (int i = inicio + 1; i < fim; i++) {
                if (aberto[i].f < aberto[melhor_indice].f) {
                    melhor_indice = i;
                }
            }

            Celula atual = aberto[melhor_indice];
            aberto[melhor_indice] = aberto[inicio];
            aberto[inicio++] = atual;

            visitados[atual.x][atual.y] = 1;

            if (atual.x == pos_x && atual.y == pos_y) {
                melhor = atual;
                break;
            }

            int dx[] = {-1, 1, 0, 0};
            int dy[] = {0, 0, -1, 1};

            for (int i = 0; i < 4; i++) {
                int nx = atual.x + dx[i];
                int ny = atual.y + dy[i];

                if (nx >= 0 && nx < MAX_LIN && ny >= 0 && ny < MAX_COL &&
                    mundo[nx][ny] != '#' && !visitados[nx][ny] &&
                    !(nx == obj_x && ny == obj_y)) {

                    if (fim < MAX_LIN * MAX_COL) {
                        Celula vizinho = {nx, ny, atual.g + 1, calcular_distancia(nx, ny, pos_x, pos_y), 0, atual.x, atual.y};
                        vizinho.f = vizinho.g + vizinho.h;
                        aberto[fim++] = vizinho;
                    }
                }
            }
        }

        if (melhor.x == adv_x && melhor.y == adv_y) {
            return;
        }

        Celula passo = melhor;
        while (passo.pai_x != adv_x || passo.pai_y != adv_y) {
            int encontrou = 0;
            for (int i = inicio - 1; i >= 0; i--) {
                if (aberto[i].x == passo.pai_x && aberto[i].y == passo.pai_y) {
                    passo = aberto[i];
                    encontrou = 1;
                    break;
                }
            }
            if (!encontrou) break;
        }

        mundo[adv_x][adv_y] = '.';
        adv_x = passo.x;
        adv_y = passo.y;
        mundo[adv_x][adv_y] = 'E';
    }
}

int calcular_distancia(int x1, int y1, int x2, int y2) {
    return abs(x1 - x2) + abs(y1 - y2);
}

void calcular_pontos(){
    int base[10] = {65, 90, 115, 165, 215, 165, 215, 115, 215, 165};
    int difi[3] = {1, 2, 3};

    if(fase >= 1 && fase <= 10){
        if(contador_passos <= base[fase-1]){
            int pontos = (base[fase-1] - contador_passos) * difi[nivel_dificuldade-1];
            participantes[idx].score += pontos + (10 * difi[nivel_dificuldade-1]);
        }else
            participantes[idx].score += 10;
    }
}

void exibir_ranking() {
    printf("Digite os 3 caracteres de suas iniciais: \n");
    scanf("%3s", participantes[idx].sigla);
    getchar();

    printf("\n%s, sua pontuação foi: %d\n", participantes[idx].sigla, participantes[idx].score);

    ordenar_ranking();

    printf("\n\n\t\t::: CLASSIFICAÇÃO :::\n");
    printf("\nPosição  |  Iniciais  |  Pontuação\n");
    //printf("----------------------------------\n");

    for (int i = 0; i <= idx; i++) {
        printf("#%d       |   %s       |  %d\n", i + 1, participantes[i].sigla, participantes[i].score);
    }
}

void ordenar_ranking() {
    for (int i = 0; i < idx; i++) {
        for (int j = i + 1; j <= idx; j++) {
            if (participantes[i].score < participantes[j].score) {
                struct Participante temp = participantes[i];
                participantes[i] = participantes[j];
                participantes[j] = temp;
            }
        }
    }
}