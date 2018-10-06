#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/neutrino.h>
#include "calc_message.h"

int main(int argc, char *argv[]) {

	int receiveid;
	int chid;
	long test;
	double testAnswer;
	client_send_t request;
	server_response_t response; //

	static const char* UNDERFLOW = " Possible underflow \n"; // underflow
	static const char* LOP = " Loss of precision.\n"; // precision loss
	static const char* DIVBY0 = " Undefined.\n"; // division by zero
	static const char* OVERFLOW = " Overflow \n"; // overflow
	static const char* UNRECOGNIZED = " Unrecognized operand.\n"; // Unrecognized operand

	/*create a channel*/
	chid = ChannelCreate(0);
	if (chid == -1)// check for channel
	{
		perror("Failed to create the channel.\n");
		exit (EXIT_FAILURE);
	}

	printf("Server PID is %d\n", getpid()); // print the server PID

	while(1) {

		receiveid = MsgReceive(chid, &request, sizeof(client_send_t), NULL);

		response.error = SRVR_OK; // check for response error

		switch(request.operator) {
		case '+': // check for addition
			response.answer = (float) request.left_hand + (float) request.right_hand;
			test = request.left_hand + request.right_hand;

			if(response.answer != test) {
				response.error = SRVR_UNDEFINED;// check for undefined
				strcpy(response.errorMsg, LOP);//
			}
			break;
		case '/': // check for devision
				if(request.right_hand == 0) {
					response.error = SRVR_UNDEFINED;//
					strcpy(response.errorMsg, DIVBY0);
				} else {
					response.answer = (float) request.right_hand / (float) request.left_hand;
					testAnswer = request.right_hand / request.left_hand;

					response.answer = (float) request.left_hand / (float) request.right_hand;
					testAnswer = request.left_hand / request.right_hand;

					if(response.answer != testAnswer|| (request.left_hand > 0 && request.right_hand > 0 && response.answer < 0)) {
						response.error = SRVR_UNDEFINED;//
						strcpy(response.errorMsg, UNDERFLOW);
					}
				}
				break;
		case 'x':// check for multiplication
		case 'X':
			response.answer = (float) request.left_hand * (float) request.right_hand;
			testAnswer = request.left_hand * request.right_hand;

			if(response.answer != testAnswer || (request.left_hand > 0 && request.right_hand > 0 && response.answer < 0)) {
				response.error = SRVR_UNDEFINED;
				strcpy(response.errorMsg, OVERFLOW);
			}
			break;
		case '-': // check for subtraction
			response.answer = (float) request.left_hand - (float) request.right_hand;
			test = request.left_hand - request.right_hand;

			if(response.answer != test) {
				response.error = SRVR_UNDEFINED;
				strcpy(response.errorMsg, LOP);
			}
			break;
		default:
			response.error = SRVR_UNDEFINED; // error
			strcpy(response.errorMsg, UNRECOGNIZED);
		}

		//send response
		MsgReply(receiveid, EOK, &response, sizeof(server_response_t));
	}

	ChannelDestroy(chid);
}
