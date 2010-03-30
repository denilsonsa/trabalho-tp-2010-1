/*
Disciplina: Teleprocessamento e Redes
Ano/Período: 2010/1
Prof.: Silvana Rossetto
Programa básico de um servidor UDP
*/
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h> 
#include <stdlib.h>

#define BUFFERSIZE 1024
 
int main(int argc, char *argv[]) {
  int sock;
  struct sockaddr_in addr; 
  char buffer[BUFFERSIZE];
  ssize_t recsize;
  socklen_t fromlen;
  
  // verifica se o numero da porta para o processo foi informado
  if (argc == 1) {
    printf("--Erro na inicializacao: server <local port>\n");
    exit(0);
  }

  // cria o descritor de socket para o servico entrega nao-confiavel
  if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
    printf("--Erro na criacao do socket\n");
    exit(0);
  }
  
  // configura a estrutura de dados com o endereco local
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(atoi(argv[1]));
 
  // associa o descritor de socket com o endereco local
  if (bind(sock,(struct sockaddr *)&addr, sizeof(struct sockaddr)) < 0) {
    printf("--Exit com erro no bind \n");
    close(sock);
    exit(0);
  } 
 
  // loop principal do servidor
  for (;;)  {
    memset(buffer, 0, sizeof(buffer));
    printf ("esperando mensagens....\n");
    recsize = recvfrom(sock, (void *) buffer, BUFFERSIZE, 0, (struct sockaddr *)&addr, &fromlen);
    if (recsize < 0) {
       printf("--Erro no recebimento \n");
    }
    printf("mensagem recebida: %s (%d bytes)\n",buffer,recsize);
  }
}
