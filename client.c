#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MSG_SIZE 256

typedef enum {
    MSG_INIT, // Boas vindas
    MSG_ACTION_REQ, // Solicitacao de acao
    MSG_ACTION_RES, // Cliente responde com acao escolhida
    MSG_BATTLE_RESULT, // Resultado do turno
    MSG_INVENTORY, // Atualizacao de inventario
    MSG_GAME_OVER, // Fim de jogo
    MSG_ESCAPE // Jogador fugiu
} MessageType;

typedef struct {
    int type;
    int client_action;
    int server_action;
    int client_hp;
    int server_hp;
    int client_torpedoes;
    int client_shields;
    char message [MSG_SIZE];
} BattleMessage;

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;
typedef struct sockaddr_in6 sockaddr_in6;
typedef struct in_addr in_addr;
typedef struct in6_addr in6_addr;

char messages[5][MSG_SIZE] = {
    "Voce disparou um Laser!",
    "Voce disparou um Photon Torpedo!",
    "Voce ativou os Escudos!",
    "Voce ativou Cloaking!",
    "Voce acionou o Hyper Jump!\nSua nave escapou para o hiperespaco"
};

void msg_print(BattleMessage* bm) {
    printf("%s\n", bm->message);
}

void msg_action_req(BattleMessage* bm, int sockfd) {
    BattleMessage bm1 = {
        MSG_ACTION_RES, -1, bm->server_action,
        bm->client_hp, bm->server_hp,
        bm->client_torpedoes, bm->client_shields,
        ""
    };

    while(1) {
        printf("%s", bm->message);
        scanf("%d", &bm1.client_action);

        if(bm1.client_action < 0 || bm1.client_action > 4) {
            printf("Erro: escolha invalida\n");
            printf("Por favor escolha um valor entre 0 e 4\n");
            continue;
        }

        printf("%s\n", messages[bm1.client_action]);
        if(bm1.client_action == 4) bm1.type = MSG_ESCAPE;
        break;
    }

    send(sockfd, &bm1, sizeof(BattleMessage), 0);
}

void msg_game_over(int sockfd) {
    BattleMessage bm = {MSG_GAME_OVER, -1, -1, -1, -1, -1, -1, ""};
    send(sockfd, &bm, sizeof(BattleMessage), 0);
}

void repl(int sockfd, sockaddr* addr, socklen_t addrlen) {
    BattleMessage* bm = (BattleMessage*)malloc(sizeof(BattleMessage));
    int game_over = 0;

    while(!game_over) {
        recv(sockfd, bm, sizeof(BattleMessage), 0);
        
        switch(bm->type) {
            case MSG_INIT:
            case MSG_ACTION_RES:
            case MSG_BATTLE_RESULT:
                msg_print(bm);
                break;
            
            case MSG_ACTION_REQ:
                msg_action_req(bm, sockfd);
                break;
            
            case MSG_INVENTORY:
            case MSG_ESCAPE: {
                msg_print(bm);
                msg_game_over(sockfd);
                game_over = 1;
                break;
            }

        }
    }

    free(bm);
}

void init_client_v4(int port) {
    char* host = "127.0.0.1";
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    sockaddr_in address = {0};
    sockaddr* addr = (sockaddr*)&address;
    socklen_t addrlen = sizeof(address);
    address.sin_family = AF_INET;
    address.sin_port = port;
    
    if(sockfd == -1) {
        perror("erro no socket (construtor)");
        exit(1);
    }
    if (inet_pton(AF_INET, host, &address.sin_addr) != 1) {
        perror("erro no inet pton");
        exit(1);
    }
    if(connect(sockfd, addr, addrlen) == -1) {
        perror("erro no connect");
        exit(1);
    }
    
    repl(sockfd, addr, addrlen);
    close(sockfd);
}

void init_client_v6(int port) {
    char* host = "::1";
    int sockfd = socket(AF_INET6, SOCK_STREAM, 0);

    sockaddr_in6 address = {0};
    sockaddr* addr = (sockaddr*)&address;
    socklen_t addrlen = sizeof(address);
    address.sin6_family = AF_INET6;
    address.sin6_port = port;
    
    if(sockfd == -1) {
        perror("erro no socket (construtor)");
        exit(1);
    }
    if (inet_pton(AF_INET6, host, &address.sin6_addr.s6_addr) != 1) {
        perror("erro no inet pton");
        exit(1);
    }
    if(connect(sockfd, addr, addrlen) == -1) {
        perror("erro no connect");
        exit(1);
    }
    
    repl(sockfd, addr, addrlen);
    close(sockfd);
}

int main(int argc, char** argv) {
    if(argc != 3) {
        perror("numero invalido de argumentos");
        exit(1);
    }
    
    int port = htons(atoi(argv[2]));
    int is_v4 = strcmp(argv[1], "127.0.0.1") == 0;
    int is_v6 = strcmp(argv[1], "::1") == 0;

    if(is_v4) init_client_v4(port);
    else if(is_v6) init_client_v6(port);
    else {
        perror("endereco invalido");
        exit(1);
    }
}