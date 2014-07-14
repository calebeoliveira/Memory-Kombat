/*******************************************************************************
Autor: Calebe Oliveira
Componente Curricular: Algoritmos I
Concluido em: 25/11/2012
Declaro que este código foi elaborado por mim de forma individual e não contém nenhum
trecho de código de outro colega ou de outro autor, tais como provindos de livros e
apostilas, e páginas ou documentos eletrônicos da Internet. Qualquer trecho de código
de outra autoria que não a minha está destacado com uma citação para o autor e a fonte
do código, e estou ciente que estes trechos não serão considerados para fins de avaliação.
******************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifndef _WIN32
    #include <curses.h> //Biblioteca para linux
    #define LIMPA_TELA system("clear")
    #define SEGURA getchar()
#else
    #include <conio.h> //Biblioteca para windows
    #define LIMPA_TELA system("cls")
    #define SEGURA getch()
#endif


//Caractere ASCII que representa uma carta no tabuleiro
#define carta 178
//Seta apontando para a direita, em ASCII
#define separador 175


void e_repete (int caractere, int vezes, int antes, int depois){
    //Função para reescrever um caractere ASCII um determinado número de vezes.
    int i;
    if(antes != 0)
        printf("%c", antes);

    for (i = 1; i < vezes; i++)
        printf("%c", caractere);

    if(depois != 0)
        printf("%c", depois);
}

void nome_decorado (){
    //Função que imprime o nome do jogo na tela.
    printf("\t\t\t");
    e_repete(205, 32, 201, 187);
    printf("\n\t\t\t%c  Bem vindos ao Memory Kombat  %c\n", 186, 186);
    printf("\t\t\t");
    e_repete(205, 32, 200, 188);
    printf("\n");
    e_repete(196, 23, 195, 0);
    printf("%c   Jogo da mem%cria multiplayer   %c", 26, 162, 27);
    e_repete(196, 22, 0, 180);
    printf("\n\n");

}

char rnd (){
    //Função que gera um caractere aleatório e o retorna.
    char data[36] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R',
                     'S','T','U','V','W','X','Y','Z','0','1','2','3','4','5','6','7','8','9'};
    return data[rand()%36];
}

void exibe_tabuleiro (char exibicao[10][10], int limite){
    //Função para imprimir um tabuleiro na tela, a partir de uma matriz.
    //É limitado em imprimir apenas o tamanho que vai ser usado na partida.
    int i, j;

    printf("\n     ");

    //Imprime o título das colunas
    for (i = 0; i < limite; i++)
        printf("%3cC%d", '\0', i+1);

    printf("\n       ");
    e_repete(196, limite * 5, 0, 0); //Repete o caractere "underline" 5x o tamanho do tabuleiro, pois cada título ocupa 5 espaços.
    printf("\n");

    //Imprime uma o título das linhas, com um espaçamento adequado.
    for (i = 0; i < limite; i++){
        if(i < 9)
            printf("%4cL%d%c", '\0',i+1, 179); //Espaçamento normal
        else
            printf("%3cL%d%c", '\0',i+1, 179); //Reduz um espaço, quando imprime a linha 10

        for (j = 0; j < limite; j++){
            printf(" %c   ", exibicao[i][j]); //Imprime a linha, com os caracteres espaçados.
        }

        //Afasta o tabuleiro da linha de pontuação
        if(i == 9){ //Se o tabuleiro for grande, dá apenas uma quebra de linha.
            printf("\n");
        } else if(i == limite-1){
            printf("\n\n"); //Se for a última linha de tabuleiros pequenos, dá duas quebras de linha.
        } else {
            printf("\n%6c%c\n", '\0', 179); //Caso contrário, imprime uma barra vertical afastada da margem.
        }
    }
}

void finaliza (int pontos[2], int jogadas){
    //Função que mostra na tela o resultado do jogo.
    //Recebe um vetor com as duas pontuações, e o número total de jogadas válidas.

    LIMPA_TELA;
    nome_decorado();

    if(pontos[0] > pontos[1]){
        printf("O Jogador 1 venceu a partida, com um total de %d pontos.\n\nA partida teve um total de %d jogadas.", pontos[0], jogadas);
    } else {
        printf("O Jogador 2 venceu a partida, com um total de %d pontos.\n\nA partida teve um total de %d jogadas.", pontos[1], jogadas);
    }
}

void PLAY_engine (char matriz[10][10], int limite){
    //Função que vai receber o tabuleiro e efetivar as jogadas.
    /*
    Variáveis:

        i, j -> linha e coluna
        n_player -> quando é incrementada, permuta o jogador de acordo com a paridade.
        id_player, pontos[2] -> a identificação do jogador atual, com suas respectivas pontuações.
        linha[2], coluna [2] -> recebe temporatiamente as posições dos dois pares da jogada atual.
        valida[10][10] -> memoriza as cartas já jogadas.

        po_item -> Faz a permutação entre as partes de leitura das posições das cartas e pontuação.
        p_jog[2][2] -> Memoriza as posições das últimas jogadas.
        jogadas -> Número de jogadas válidas.
        valida_pos[2] -> Memoriza se as posições existem no tabuleiro, ou não.

        exibicao[10][10] -> armazena o que será exibido ao jogador, no tabuleiro.
        saida[2] -> armazena temporariamente os valores do par de cartas.
        historico[2] -> armazena as últimas jogadas para pontuar corretamente com o bônus.
    */
    int i, j, final = 0, n_player = 0, id_player, pontos[2] = {0,0}, linha[2], coluna[2], valida[10][10];

    int po_item = 0, p_jog[2][2], jogadas = 0, valida_pos[2] = {0, 0};
    char exibicao[10][10], saida[2], historico[2] = {'#','#'};

    //Zera as matrizes
    for (i = 0; i < limite; i++){
        for (j = 0; j < limite; j++){
            exibicao[i][j] = carta;
            valida[i][j] = 0;
        }
    }

    //Núcleo da função, que é repetido até que o tabuleiro seja completado.
    while (final < ((limite * limite) / 2)){

        LIMPA_TELA;

        //Mostra o tabuleiro na tela.
        exibe_tabuleiro(exibicao, limite);

        //Identifica o jogador atual, baseado na paridade do valor de n_player
        if(n_player%2 == 0)
            id_player = 0;
        else
            id_player = 1;

        //Imprime o status atual das jogadas
        printf("\nVez: Jogador %d               Pontos: |Jogador 1%c %d|     |Jogador 2%c %d|\n ", id_player+1, 26, pontos[0], 26, pontos[1]);

        //Lê as entradas dos valores do primeiro caractere.
        if(po_item == 0 ){

            printf("\nDigite o n%cmero da linha do primeiro item: ", 163);
            scanf("%d", &linha[0]);
            printf("Digite o n%cmero da coluna do primeiro item: ", 163);
            scanf("%d", &coluna[0]);

            linha[0]--;
            coluna[0]--;

            //Transfere o caracter solicitado do nível da matriz original para o nível da matriz de exibição, se a posição existir.
            if(linha[0] < limite && linha[0] >= 0 && coluna[0] < limite && coluna[0] >= 0){
                exibicao[linha[0]][coluna[0]] = matriz[linha[0]][coluna[0]];
                saida[0] = matriz[linha[0]][coluna[0]];
                p_jog[0][0] = linha[0];
                p_jog[0][1] = coluna[0];
                valida_pos[0] = 1;
            } else {
                valida_pos[0] = 0;
            }

            po_item++;

        //Lê as entradas dos valores do segundo caractere.
        } else if(po_item == 1) {
            printf("\nDigite o n%cmero da linha do segundo item: ", 163);
            scanf("%d", &linha[1]);
            printf("Digite o n%cmero da coluna do segundo item: ", 163);
            scanf("%d", &coluna[1]);

            linha[1]--;
            coluna[1]--;

            //Transfere o caracter solicitado do nível da matriz original para o nível da matriz de exibição, se a posição existir.
            if(linha[1] < limite && linha[1] >= 0 && coluna[1] < limite && coluna[1] >= 0){
                exibicao[linha[1]][coluna[1]] = matriz[linha[1]][coluna[1]];
                saida[1] = matriz[linha[1]][coluna[1]];
                p_jog[1][0] = linha[1];
                p_jog[1][1] = coluna[1];

                valida_pos[1] = 1;
            } else {
                valida_pos[1] = 0;
            }

            po_item++;

        } else {
            po_item = 0;
            //Verifica se as posições existem no tabuleiro.
            if(valida_pos[0] == 0 || valida_pos[1] == 0){
                printf("\nJogada inv%clida! As posi%c%ces n%co existem no tabuleiro.", 160, 135, 228, 198);

                //Validação para evitar acessos a lugares não permitidos na memória para a primeira carta.
                if(valida[p_jog[0][0]][p_jog[0][1]] == 0 && (p_jog[0][0] < limite || p_jog[0][0] >= 0 || p_jog[0][1] < limite || p_jog[0][1] >= 0))
                    exibicao[p_jog[0][0]][p_jog[0][1]] = carta;
                //Validação para evitar acessos a lugares não permitidos na memória para a segunda carta.
                if(valida[p_jog[1][0]][p_jog[1][1]] == 0 && (p_jog[1][0] < limite || p_jog[1][0] >= 0 || p_jog[1][1] < limite || p_jog[1][1] >= 0))
                    exibicao[p_jog[1][0]][p_jog[1][1]] = carta;

                SEGURA;

            //Se os caracteres forem iguais...
            } else if(saida[0] == saida[1]){
                //Valida pra saber se as posições das cartas são iguais, ou se elas já não foram jogadas.
                if((p_jog[0][0] == p_jog[1][0] && p_jog[0][1] == p_jog[1][1]) || valida[p_jog[0][0]][p_jog[0][1]] == 1 || valida[p_jog[1][0]][p_jog[1][1]] == 1){
                    printf("\nJogada inv%clida! As posi%c%ces s%co iguais, ou j%c foram jogadas.", 160, 135, 228, 198, 160);

                    //Oculta o que for necessário em caso de jogada inválida.
                    if(valida[p_jog[0][0]][p_jog[0][1]] == 0)
                        exibicao[p_jog[0][0]][p_jog[0][1]] = carta;
                    if(valida[p_jog[1][0]][p_jog[1][1]] == 0)
                        exibicao[p_jog[1][0]][p_jog[1][1]] = carta;
                } else {
                    //Posições válidas e caracteres iguais...
                    //Pontua com bônus de arroba
                    if(saida[0] == '@'){
                        printf("\nVoc%c ganhou o b%cnus de 4 pontos!", 136, 147);
                        pontos[id_player] += 4;

                        //Consulta no histórico a jogada anterior e troca de jogador, caso já tenha ganhado bônus uma vez.
                        if(historico[id_player] != '#'){
                            historico[id_player] = '#';
                            n_player++;
                        }

                        //Guarda no histórico a jogada atual.
                        historico[id_player] = saida[0];
                        jogadas++;

                    //Pontua com bônus de cifrão.
                    } else if(saida[0] == '$'){
                        printf("\nVoc%c ganhou o b%cnus de 7 pontos!", 136, 147);
                        pontos[id_player] += 7;

                        if(historico[id_player] != '#'){
                            historico[id_player] = '#';
                            n_player++;
                        }

                        //Guarda no histórico a jogada atual.
                        historico[id_player] = saida[0];
                        jogadas++;

                    //Pontua com par comum.
                    } else {
                        printf("\nVoc%c ganhou 2 pontos!", 136);

                        pontos[id_player] += 2;
                        n_player++;
                        jogadas++;
                    }

                    //Marca na matriz de validação que as cartas já foram usadas.
                    valida[p_jog[0][0]][p_jog[0][1]] = 1;
                    valida[p_jog[1][0]][p_jog[1][1]] = 1;
                    final++;
                }

                SEGURA;

                //Se o jogador perdeu a jogada...
                } else {
                    //Verifica se as posições de jogada são diferentes e válidas.
                    if((p_jog[0][0] == p_jog[1][0] && p_jog[0][1] == p_jog[1][1]) || valida[p_jog[0][0]][p_jog[0][1]] == 1 || valida[p_jog[1][0]][p_jog[1][1]] == 1){
                        //Exibe mensagem de erro.
                        printf("\nJogada inv%clida! As posi%c%ces s%co iguais, ou j%c foram jogadas.", 160, 135, 228, 198, 160);

                    } else {

                        printf("\nVoc%c perdeu", 136);

                        //Incrementa o número de jogadas.
                        jogadas++;

                        //Decrementa pontuação para o caso de encotrar um @ na primeira carta.
                        if(saida[0] == '@'){
                            pontos[id_player] -= 4;
                            printf(" 4 pontos");
                        }

                        //Decrementa pontuação para o caso de encotrar um $ na primeira carta.
                        if(saida[0] == '$'){
                            pontos[id_player] -= 7;
                            printf(" 7 pontos");
                        }

                        printf("!");

                        //Troca de jogador
                        n_player++;
                }

                //Esconde novamente o par de cartas errado.
                if(valida[p_jog[0][0]][p_jog[0][1]] == 0)
                    exibicao[p_jog[0][0]][p_jog[0][1]] = carta;

                if(valida[p_jog[1][0]][p_jog[1][1]] == 0)
                    exibicao[p_jog[1][0]][p_jog[1][1]] = carta;

                SEGURA;
            }
        }
    }

    //Transfere os valores obtidos para a função que exibe os resultados da partida.
    finaliza(pontos, jogadas);
}

void manual_MK_engine (int padrao_tam){
    //Função que recebe o preenchimento manual do tabuleiro.

    int i, j;
    char final[10][10];

    LIMPA_TELA;
    printf("Preencha o tabuleiro:\n\n\n");

    //Lê os valores para o tabuleiro
    for (i = 0; i < padrao_tam; i++){
        for (j = 0; j < padrao_tam; j++){
            printf("%c Digite o caractere da linha %d e da coluna %d: ", 26, i+1, j+1);
            scanf(" %c", &final[i][j]);
        }
        printf("\n");
    }

    //Passa a matriz e o limite de uso da matriz para a função PLAY.
    PLAY_engine(final, padrao_tam);
}

void auto_MK_engine (int padrao_tam){
    //No modo automático, cria uma matriz com pares de caracteres em posições aleatórias.

    /*
    Variáveis:
        matrix[10][10] -> é uma matriz que será copiada, de forma embaralhada, para a matriz final[10][10]
        final[10][10] -> retorno da função
        i, j, ia, ja, itemp, jtemp, acount -> memorizam posições de linhas, colunas e contadores dentro dos laços.
        auxiliar[10][10] -> memoriza as posições de matrix[10][10] já permutadas para final[10][10]
        padrao_pares -> quantidade de pares de cada bônus
        padrao_total -> quadrado de padrao_pares

    */

    char matrix[10][10], final[10][10];
    int i, j, ia, ja, acount, itemp, jtemp;
    int auxiliar[10][10];
    int padrao_pares, padrao_total;

    // Bloco que dá as diretrizes para o funcionamento do núcleo, com as quant
    switch(padrao_tam){
        case 4: padrao_pares = 2; break;
        case 6: padrao_pares = 4; break;
        case 8: padrao_pares = 8; break;
        case 10: padrao_pares = 12; break;
    }

    padrao_total = (padrao_tam * padrao_tam);

    //Zera as matrizes
    for (i = 0; i < 10; i++){
        for (j = 0; j < 10; j++){
            matrix[i][j] = '#';
            final[i][j] = '#';
            auxiliar[i][j] = 0;
        }
    }

    //Gera uma semente
    srand(time(0));

    printf("Iniciando modo de jogo [OK]\n\nGerando caracteres especiais ");

    //Preenche o tabuleiro com a quantidade específica de arrobas.
    acount = 0;
    while (acount < padrao_pares){

        ia = rand()%padrao_tam;
        ja = rand()%padrao_tam;

        if (matrix[ia][ja] == '#'){
            matrix[ia][ja] = '@';
            acount++;
        }
    }

    //Preenche o tabuleiro com a quantidade específica de cifrões.
    acount = 0;
    while (acount < padrao_pares){

        ia = rand()%padrao_tam;
        ja = rand()%padrao_tam;

        if (matrix[ia][ja] == '#'){
            matrix[ia][ja] = '$';
            acount++;
        }
    }

    printf("[OK]\nGerando letras e n%cmeros ", 163);

    //Preenche o resto do tabuleiro com pares de caracteres aleatórios.
    acount = 2;
    for (i = 0; i < padrao_tam; i++){
        for (j = 0; j < padrao_tam; j++){
            //Verifica se a posição atual da matriz está vazia
            if (matrix[i][j] == '#'){
                //Se o contador estiver par, coloca no lugar um caractere aleatório e guarda.
                if (acount%2 == 0){
                    matrix[i][j] = rnd();
                    itemp = i;
                    jtemp = j;
                //Se o contador estiver ímpar, pega o caractere guardado e coloca na posição.
                } else {
                    matrix[i][j] = matrix[itemp][jtemp];
                }
                //Incrementa o contador.
                acount++;
            }
        }
    }

    printf("[OK]\nEmbaralhando...\n");

    //Depois de muita cafeína, a parte que vai embaralhar a matriz.
    acount = 0;
    i = j = 0;
    while (acount < padrao_total){
        //Gera uma posição aleatória no tabuleiro.
        ia = rand()%padrao_tam;
        ja = rand()%padrao_tam;

        //Verifica se a posição gerada está vazia.
        if (auxiliar[ia][ja] == 0){
            //Ocupa a posição.
            auxiliar[ia][ja] = 1;

            /* Transfere o caractere de uma posição aleatória da matriz preenchida pela função anterior,
            para uma posição conhecida da matriz final. */
            final[i][j] = matrix[ia][ja];

            //Limita a parte em uso do tabuleiro dentro do laço.
            j++;
            if(j == padrao_tam){
                j = 0;
                i++;
            }
            //Incrementa para chegar mais perto do fim do loop.
            acount++;
        }
    }

    //Passa a matriz e o limite de uso da matriz para a função PLAY.
    PLAY_engine(final, padrao_tam);
}

void modos_jogo (int modo){
    //Segundo menu na tela. O usuário escolhe o tamanho do tabuleiro.

    int opcao = 0, retorno;

    do{
        LIMPA_TELA;
        nome_decorado();
        printf("%c Digite o n%cmero da op%c%co correspondente ao tamanho do tabuleiro:\n\n", 16, 163, 135, 198);
        printf("  1 %c 4x4\n", separador);
        printf("  2 %c 6x6\n", separador);
        printf("  3 %c 8x8\n", separador);
        printf("  4 %c 10x10\n\n > ", separador);

        //Lê a entrada do usuário.
        scanf("%d", &opcao);

    } while (opcao != 1 && opcao != 2 && opcao != 3 && opcao != 4);

    //De acordo com o modo de jogo, recebido da fução anterior, executa a equação que unirá os resultados dos dois menus.
    if (modo == 1){
        //Automático
        retorno = (2 * opcao + 2);
        //Parâmetro para o modo automático.
        auto_MK_engine(retorno);
    } else {
        //Manual
        retorno = (2 * opcao + 2);
        //Parâmetro para o modo manual.
        manual_MK_engine(retorno);
    }
}

void instrucao (){
    //Função para mostrar as instruções na tela.
    LIMPA_TELA;
    nome_decorado();

    printf("Instru%c%ces:\n\n1. Selecione o modo de preenchimento do tabuleiro (manual ou autom%ctico).\n\n", 135, 228, 160);
    printf("2. Selecione um dos quatro tamanhos de tabuleiro dispon%cveis. ", 161);
    printf("Caso tenha\nescolhido o modo manual,  preencha o tabuleiro do seu jeito. N%co esque%ca dos\ncaraceteres especiais de b%cnus '$' e '@'!\n\n", 198, 135, 147);
    printf("3. As jogadas ser%co alternadas entre os jogadores. Caso encontre o b%cnus, voc%c\npoder%c jogar mais uma rodada. ", 198, 147, 136, 160);
    printf("Voc%c s%c ganhar%c uma nova jogada se n%co tiver\nganhado um b%cnus na jogada anterior.\n\n", 136, 162, 160, 198, 147);
    printf("4. Cada tabuleiro gerado automaticamente tem 25%% dos pares com cartas b%cnus.\n\n", 147);
    printf("5. Se a primeira carta de um par for um b%cnus, e a segunda n%co for, o jogador\nperder%c os pontos correspondentes ao b%cnus encontrado.\n\n", 147, 198, 160, 147);
    printf("\nPressione qualquer tecla para voltar ao menu principal... ");
    SEGURA;
}

int main (){
   int opcao;

    do{
        LIMPA_TELA;
        nome_decorado();
        printf("%c Digite o n%cmero correspondente ao modo de jogo:\n\n", 16, 163);
        printf("  1 %c Jogar no modo autom%ctico\n", separador, 160);
        printf("  2 %c Jogar no modo manual\n", separador);
        printf("  3 %c Instru%c%ces\n\n > ", separador, 135, 228);
        scanf("%d", &opcao);

        //Verifica se deve mostrar as instruções na tela.
        if(opcao == 3)
            instrucao();

    } while ((opcao != 1 && opcao != 2 && opcao != 3) || opcao == 3);

    //Chama o menu para escolher o modo de jogo.
    modos_jogo(opcao);

    printf("\n\nObrigado por jogar :)\nPressione qualquer tecla para encerrar o jogo... ");
    SEGURA;
	return (0);
}