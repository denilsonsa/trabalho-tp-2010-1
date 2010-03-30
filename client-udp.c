/*
Disciplina: Teleprocessamento e Redes
Ano/Período: 2010/1
Prof.: Silvana Rossetto
Programa básico de um cliente UDP
*/

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h> 

#define BUFFERSIZE 100
 
int main(int argc, char *argv[]) {
  int sock;
  struct sockaddr_in addr;
  int bytes_sent, buffer_length;
  char buffer[BUFFERSIZE];
  char *servIP;                
  unsigned short servPort;   
 
  // verifica se o IP e numero da porta do processo servidor foi informado
  if (argc != 3) {
      printf("--Erro na inicializacao: client <remote server IP> <remote server Port>\n");
      exit(0);
  }
  servIP = argv[1];
  servPort = atoi(argv[2]);

  // cria o descritor de socket para o servico entrega nao-confiavel
  if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
    printf("--Erro na criacao do socket\n");
    exit(0);
  }
 
  // configura a estrutura de dados com o endereco local 
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr(servIP); 
  addr.sin_port = htons(servPort);
 
  printf("digite uma mensagem: \n"); 
  scanf(" %[^\n]",buffer);
  bytes_sent = sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr*)&addr, sizeof (struct sockaddr_in));
  if (bytes_sent < 0) {
     printf("--Erro no recebimento \n");
  }
  close(sock); 
  return 0;
}
