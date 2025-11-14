//******************************** client **************************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved
//******************************************************************************
// File    : client.c
// Summary : Handle client side socket communication to send user commands to 
//           the server and display the received response.
// Note    : None
// Author  : Surya Santhosh
// Day     : 11/Nov/2025
//******************************************************************************

//**************************** Include Files ***********************************
#include "common.h"

//******************************* Local Types **********************************

//***************************** Local Constants ********************************

//**************************** Local Variables *********************************

//***************************** Local Functions ********************************
static bool clientSocket(uint16* punSocket);
static bool clientConnect(uint16* punSocket, uint16* punConnect);
static bool clientSetTimeout(uint16* punSocket, uint16* punTimeout);

//*****************************.mainFunction.***********************************
// Purpose : Handle client side socket communication to send user commands to 
//           the server and display the received response.
// Inputs  : none
// Outputs : none
// Return  : 0
// Notes   : None
//******************************************************************************
int main()
{
    uint16 unSocket = 0;
    uint16 unConnect = 0;
    uint16 unTimeout = 0;
    uint16 unBufferLength = 0;
    uint8 ucBuffer[MAX_CHAR_SIZE] = "";
    uint8 *pucRecievedBuffer = NULL;

    clientSocket(&unSocket);

    // To set receive timeout 
    clientSetTimeout(&unSocket, &unTimeout);
    clientConnect(&unSocket, &unConnect);
    
    // Read Querry string.
    int8 *pcMessage = getenv("QUERY_STRING");
    
    if (pcMessage != NULL) 
    {
        // Extract string from qurey string.
        sscanf(pcMessage, "msg=%s", ucBuffer);
    }

    send(unSocket, ucBuffer, sizeof(ucBuffer), 0);

    // Receive size of the server response.
    recv(unSocket, &unBufferLength, sizeof(unBufferLength), 0);

    // Dynamically allocate memory for receive buffer.
    // +1 for null terminator.
    pucRecievedBuffer = malloc(unBufferLength + 1);

    if (ERROR_CODE == recv(unSocket, pucRecievedBuffer, unBufferLength, 0))
    {
        close(unSocket);
    }

    pucRecievedBuffer[unBufferLength] = '\0';

    printf("%s\n", pucRecievedBuffer);

    // Deallocate dynamically allocated memory.
    free(pucRecievedBuffer);
    close(unSocket);

    return 0;
}

//*****************************.clientSocket.***********************************
// Purpose : Create socket.
// Inputs  : punSocket - Pointer to socket descriptor.
// Outputs : none
// Return  : blReturn
// Notes   : None
//******************************************************************************
static bool clientSocket(uint16* punSocket)
{
    bool blReturn = false;

    if (NULL != punSocket)
    {
        *punSocket = socket(AF_INET, SOCK_STREAM, 0);

        if (ERROR_CODE == *punSocket)
        {
            printf("Socket creation failed\n");
        }
        else
        {
            blReturn = true;
        }
    }

    return blReturn;
}

//*****************************.clientConnect.**********************************
// Purpose : Initiate a connection on a socket.
// Inputs  : punSocket - Pointer to socket descriptor.
//         : punConnect - Pointer to connected socket descriptor.
// Outputs : none
// Return  : blReturn
// Notes   : None
//******************************************************************************
static bool clientConnect(uint16* punSocket, uint16* punConnect)
{
    bool blReturn = false;

    // To hold the client address details.
    struct sockaddr_in stSocketAddress = {0};
    stSocketAddress.sin_family = AF_INET;
    stSocketAddress.sin_port = htons(PORT);
    stSocketAddress.sin_addr.s_addr = inet_addr(IP_ADDRESS);

    if (NULL != punSocket)
    {
        *punConnect = connect(*punSocket, (struct sockaddr *) &stSocketAddress, 
                               sizeof(stSocketAddress));

        if (ERROR_CODE == *punConnect)
        {
            printf("Socket connection failed\n");
        }
        else
        {
            blReturn = true;
        }
    }

    return blReturn;
}

//*****************************.clientConnect.**********************************
// Purpose : To set timeout for recieving data.
// Inputs  : punSocket - Pointer to socket descriptor.
//         : punTimeout - Pointer to store setsockopt() return value.
// Outputs : none
// Return  : blReturn
// Notes   : None
//******************************************************************************
static bool clientSetTimeout(uint16* punSocket, uint16* punTimeout)
{
    bool blReturn = false;

    // To set timeout 
    struct timeval timeout = {0}; 
    timeout.tv_sec = CLIENT_RECEIVE_TIMEOUT;

    if ((NULL != punSocket) && (NULL != punTimeout))
    {
        *punTimeout = setsockopt(*punSocket, SOL_SOCKET, SO_RCVTIMEO, &timeout, 
                                  sizeof(timeout));

        if (ERROR_CODE == *punTimeout)
        {
            printf("setsockopt failed\n");
        }
        else
        {
            blReturn = true;
        }
    }

    return blReturn;
}

// EOF