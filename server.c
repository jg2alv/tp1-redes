#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <time.h>

#define MSG_SIZE 256

typedef enum {
    MSG_INIT, // Boas vindas
    MSG_ACTION_REQ, // S o l i c i t a c a o de acao
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
    "Servidor disparou um Laser!",
    "Servidor disparou um Photon Torpedo!",
    "Servidor ativou os Escudos!",
    "Servidor ativou Cloaking!",
    "Servidor acionou o Hyper Jump!\nA nave do servidor escapou para o hiperespaco."
};

void msg_action_res(BattleMessage* bm, int newsockfd) {
    int server_action = rand() % 5;
    int client_action = bm->client_action;
    char* message[MSG_SIZE];

    BattleMessage bm1 = {
        MSG_BATTLE_RESULT, bm->client_action, server_action,
        bm->client_hp, bm->server_hp,
        bm->client_torpedoes, bm->client_shields,
        ""
    };
    
    strcpy(message, messages[server_action]);

    if(server_action == 4) {
        strcpy(bm1.message, messages[server_action]);
        send(newsockfd, &bm1, sizeof(bm1), 0);
        msg_inventory(newsockfd, bm, 0, 1);
        return;
    }
    
    // laser vs laser
    if(server_action == 0 && client_action == 0) {
        bm1.client_hp -= 20;
        bm1.server_hp -= 20;
        sprintf(
            bm1.message,
            "%s\nResultado: Ambos receberam 20 de dano.\nPlacar: Voce %d x %d Inimigo",
            message, bm1.client_hp, bm1.server_hp
        );
    // laser vs torpedo
    } else if(server_action == 0 && client_action == 1) {
        bm1.server_hp -= 20;
        bm1.client_torpedoes++;
        sprintf(
            bm1.message,
            "%s\nResultado: Acerto! Nave inimiga perdeu 20 HP.\nPlacar: Voce %d x %d Inimigo",
            message, bm1.client_hp, bm1.server_hp
        );
    // laser vs escudos
    } else if(server_action == 0 && client_action == 2) {
        bm1.client_shields++;
        sprintf(
            bm1.message,
            "%s\nResultado: Ataque inimigo bloqueado!\nPlacar: Voce %d x %d Inimigo",
            message, bm1.client_hp, bm1.server_hp
        );
    // laser vs cloacking
    } else if(server_action == 0 && client_action == 3) {
        sprintf(
            bm1.message,
            "%s\nResultado: Voce recebeu 20 de dano.\nPlacar: Voce %d x %d Inimigo",
            message, bm1.client_hp, bm1.server_hp
        );
    // torpedo vs laser
    } else if(server_action == 1 && client_action == 0) {
        bm1.client_hp -= 20;
        sprintf(
            bm1.message,
            "%s\nResultado: Voce recebeu 20 de dano.\nPlacar: Voce %d x %d Inimigo",
            message, bm1.client_hp, bm1.server_hp
        );
    // torpedo vs torpedo
    } else if(server_action == 1 && client_action == 1) {
        bm1.client_hp -= 20;
        bm1.server_hp -= 20;
        bm1.client_torpedoes++;
        sprintf(
            bm1.message,
            "%s\nResultado: Ambos receberam 20 de dano.\nPlacar: Voce %d x %d Inimigo",
            message, bm1.client_hp, bm1.server_hp
        );
    // torpedo vs escudos
    } else if(server_action == 1 && client_action == 2) {
        bm1.client_shields++;
        sprintf(
            bm1.message,
            "%s\nResultado: Ataque inimigo bloqueado.\nPlacar: Voce %d x %d Inimigo",
            message, bm1.client_hp, bm1.server_hp
        );
    // torpedo vs cloaking
    } else if(server_action == 1 && client_action == 3) {
        sprintf(
            bm1.message,
            "%s\nResultado: Ataque inimigo falhou!\nPlacar: Voce %d x %d Inimigo",
            message, bm1.client_hp, bm1.server_hp
        );
    // escudos vs laser
    } else if(server_action == 2 && client_action == 0) {
        sprintf(
            bm1.message,
            "%s\nResultado: Seu ataque foi bloqueado.\nPlacar: Voce %d x %d Inimigo",
            message, bm1.client_hp, bm1.server_hp
        );
    // escudos vs torpedo
    } else if(server_action == 2 && client_action == 1) {
        bm1.client_torpedoes++;
        sprintf(
            bm1.message,
            "%s\nResultado: Seu ataque foi bloqueado.\nPlacar: Voce %d x %d Inimigo",
            message, bm1.client_hp, bm1.server_hp
        );
    // escudos vs escudos
    } else if(server_action == 2 && client_action == 2) {
        bm1.client_shields++;
        sprintf(
            bm1.message,
            "%s\nResultado: Ambos ataques foram bloqueados.\nPlacar: Voce %d x %d Inimigo",
            message, bm1.client_hp, bm1.server_hp
        );
    // escudos vs cloaking
    } else if(server_action == 2 && client_action == 3) {
        sprintf(
            bm1.message,
            "%s\nResultado: Ambos ataques foram bloqueados.\nPlacar: Voce %d x %d Inimigo",
            message, bm1.client_hp, bm1.server_hp
        );
    // cloaking vs laser
    } else if(server_action == 3 && client_action == 0) {
        bm1.server_hp -= 20;
        sprintf(
            bm1.message,
            "%s\nResultado: Acerto! Nave inimiga perdeu 20 HP.\nPlacar: Voce %d x %d Inimigo",
            message, bm1.client_hp, bm1.server_hp
        );
    // cloaking vs torpedo    
    } else if(server_action == 3 && client_action == 1) {
        bm1.client_torpedoes++;
        sprintf(
            bm1.message,
            "%s\nResultado: Seu ataque falhou!\nPlacar: Voce %d x %d Inimigo",
            message, bm1.client_hp, bm1.server_hp
        );
    // cloaking vs escudos
    } else if(server_action == 3 && client_action == 2) {
        bm1.client_shields++;
        sprintf(
            bm1.message,
            "%s\nResultado: Ambos ataques falharam!\nPlacar: Voce %d x %d Inimigo",
            message, bm1.client_hp, bm1.server_hp
        );
    // cloaking vs cloaking
    } else if(server_action == 3 && client_action == 3) {
        sprintf(
            bm1.message,
            "%s\nResultado: Ambos ataques falharam!\nPlacar: Voce %d x %d Inimigo",
            message, bm1.client_hp, bm1.server_hp
        );
    }

    send(newsockfd, &bm1, sizeof(bm1), 0);
    if(bm1.client_hp == 0 || bm1.server_hp == 0)
        msg_inventory(newsockfd, &bm1, 0, 0);
    else
        msg_action_req(newsockfd, -1, -1, bm1.client_hp, bm1.server_hp, bm1.client_torpedoes, bm1.client_shields);
}

void msg_escape(BattleMessage* bm, int newsockfd) {
    msg_inventory(newsockfd, bm, 1, 0);
}

void msg_init(int newsockfd) {
    BattleMessage bm = {
        MSG_INIT, -1, -1,
        100, 100, 0,
        0,
        "Conectado ao servidor.\nSua nave: SS-42 Voyager (HP: 100)"
    };
    send(newsockfd, &bm, sizeof(bm), 0);
}

void msg_action_req(int newsockfd, int client_action, int server_action, int client_hp, int server_hp, int client_torpedoes, int client_shields) {
    BattleMessage bm = {
        MSG_ACTION_REQ, client_action, server_action,
        client_hp, server_hp, client_torpedoes,
        client_shields,
        "Escolha sua acao:\n0- Laser Attack\n1- Photon Torpedo\n2- Shields Up\n3- Cloaking\n4- Hyper Jump\n> "
    };
    send(newsockfd, &bm, sizeof(bm), 0);
}

void msg_inventory(int newsockfd, BattleMessage* bm0, int client_escaped, int server_escaped) {
    char message[MSG_SIZE] = {""};
    BattleMessage bm = {
        MSG_INVENTORY, -1, -1,
        bm0->client_hp, bm0->server_hp, bm0->client_torpedoes,
        bm0->client_shields,
        ""
    };

    if(!client_escaped && !server_escaped)
        strcat(message, "Fim de jogo!\n");
    else
        bm.type = MSG_ESCAPE;
    
    sprintf(bm.message,
        "%s\nInventario final:\n- HP restante: %d\n- Torpedos usados: %d\n- Escudos usados: %d\n",
        message,
        bm0->client_hp,
        bm0->client_torpedoes,
        bm0->client_shields
    );

    if(bm0->client_hp == 0) strcat(bm.message, "Sua nave foi destruida!");
    else if(bm0->server_hp == 0) strcat(bm.message, "Voce destruiu a frota inimiga!");
    else strcat(bm.message, "Obrigado por jogar!");

    send(newsockfd, &bm, sizeof(bm), 0);
}

void repl(int sockfd, sockaddr* addr, socklen_t addrlen) {
    int newsockfd = accept(sockfd, addr, &addrlen);
    if(newsockfd == -1) {
        perror("erro no accept");
        exit(1);
    }
    srand(time(NULL));
    
    BattleMessage* bm = (BattleMessage*)malloc(sizeof(BattleMessage));
    int client_action = -1,
        server_action = -1,
        client_hp = 100,
        server_hp = 100,
        client_torpedoes = 0,
        client_shields = 0,
        turns = 0,
        game_over = 0;

    msg_init(newsockfd);
    msg_action_req(newsockfd, -1, -1, 100, 100, 0, 0);

    while(!game_over) {
        recv(newsockfd, bm, sizeof(BattleMessage), 0);

        switch(bm->type) {
            case MSG_ACTION_RES:
                msg_action_res(bm, newsockfd);
                break;
            case MSG_ESCAPE:
                msg_escape(bm, newsockfd);
                break;
            case MSG_GAME_OVER: {
                game_over = 1;
                break;
            }
        }

    }
    
    free(bm);
    close(newsockfd);
}

void init_server_v4(int port) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1) {
        perror("erro no socket (construtor)");
        exit(1);
    }

    sockaddr_in address;
    sockaddr* addr = (sockaddr*)&address;
    socklen_t addrlen = sizeof(address);

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = port;
    
    if (bind(sockfd, addr, addrlen) == -1) {
        perror("erro no bind");
        exit(1);
    }
    if (listen(sockfd, 3) == -1) {
        perror("erro no listen");
        exit(1);
    }
    repl(sockfd, addr, addrlen);
    close(sockfd);
}

void init_server_v6(int port) {
    int sockfd = socket(AF_INET6, SOCK_STREAM, 0);
    if(sockfd == -1) {
        perror("erro no socket (construtor)");
        exit(1);
    }

    sockaddr_in6 address;
    sockaddr* addr = (sockaddr*)&address;
    socklen_t addrlen = sizeof(address);

    address.sin6_family = AF_INET6;
    address.sin6_addr = in6addr_any;
    address.sin6_port = port;

    if (bind(sockfd, addr, addrlen) == -1) {
        perror("erro no bind");
        exit(1);
    }
    if (listen(sockfd, 3) == -1) {
        perror("erro no listen");
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

    char* protocol = argv[1];
    int port = htons(atoi(argv[2]));

    int is_v4 = strcmp(protocol, "v4") == 0;
    int is_v6 = strcmp(protocol, "v6") == 0;

    if(is_v4) init_server_v4(port);
    else if(is_v6) init_server_v6(port);
    else {
        perror("endereco invalido");
        exit(1);
    }
}