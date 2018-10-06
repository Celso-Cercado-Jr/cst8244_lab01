#include <stdlib.h>
#include <stdio.h>
#include <process.h>
#include <limits.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>
#include "calc_message.h"

int main(int argc, char *argv[]) {

	int index; // index
	client_send_t request; //request from the client
	server_response_t response; //response from the server
	pid_t serverPID; // server PID
	int c;
	char br = 1;// to change the

	for(index = 1; index < argc && br; ++index) {
		if(argv[index] == NULL)
			break;
		switch (index) {
		case 1:
			serverPID = atoi(argv[index]);
			break;
		case 2:
			if(atol(argv[index]) <= INT_MAX) {
				request.left_hand = atoi(argv[index]);
			} else {
				printf("Entered value is too large. Please try again..\n");
				br = 0;
			}
			break;
		case 3:
			switch(argv[index][0]) {
			case '+':
			case '/':
			case 'x':
			case 'X':
			case '- ':
				request.operator = argv[index][0];
				break;
			default:
				printf("Unrecognized operator. Please try again..\n");
				br = 0;
			}
			break;
		case 4:
			if(atol(argv[index]) <= INT_MAX) {
				request.right_hand = atoi(argv[index]);
			} else {
				printf("Entered value was too large.. Please try again..\n");
				br = 0;
			}
			break;
		}
	}

	if(br) {

		c = ConnectAttach(ND_LOCAL_NODE, serverPID, 1, _NTO_SIDE_CHANNEL, 0);
		if(c == -1) {
			fprintf (stderr, "Couldn't Connect to the server..Please try again.\n");
			perror (NULL);
			exit (EXIT_FAILURE);
		}

		//Send the message
		if(MsgSend(c, &request, sizeof(client_send_t), &response, sizeof(server_response_t)) == -1){
			fprintf (stderr, "Can't send the message.. Please try again..\n");
			perror (NULL);
			exit (EXIT_FAILURE);
		}

		switch(response.error) {
		case SRVR_UNDEFINED:
			printf(response.errorMsg);
			break;
		case SRVR_INVALID_OPERATOR: //
			printf(response.errorMsg);
			break;
		case SRVR_OVERFLOW: //
			printf(response.errorMsg);
			break;
		case SRVR_OK :
			printf("The server has calculated the result of %d %c %d as %.2f \n", request.left_hand, request.operator, request.right_hand, response.answer);
		}
	} else {
		printf("Parameters weren't correct, unable to process..\n");
	}

	return 0;
}
