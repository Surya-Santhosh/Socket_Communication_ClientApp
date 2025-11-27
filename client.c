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
static bool clientSocket(int16* punSocket);
static bool clientConnect(int16* punSocket, int16* punConnect);
static bool clientSetTimeout(int16* punSocket, int16* punTimeout);
static bool clientFileUpload(uint8* pucBuffer);
static bool clientRequestHandler(int8* pcMethod, uint8** ppucBuffer);

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
    int16 unSocket = 0;
    int16 unConnect = 0;
    uint16 unTimeout = 0;
    uint8 ucValidation = 0;
    uint16 unReceivedBufferLength = 0;
    uint8 *pucSendBuffer = NULL;
    uint8 *pucReceivedBuffer = NULL;

    printf("Content-type: application/json\r\n\r\n");
    clientSocket(&unSocket);

    // To set receive timeout 
    clientSetTimeout(&unSocket, &unTimeout);
    clientConnect(&unSocket, &unConnect);

    int8 *pcMethod = getenv("REQUEST_METHOD");

    if (NULL != pcMethod)
    {
        // 
        clientRequestHandler(pcMethod, &pucSendBuffer);
        send(unSocket, pucSendBuffer, strlen((char*)pucSendBuffer), 0);

        // Receive size of the server response.
        recv(unSocket, &unReceivedBufferLength, sizeof(unReceivedBufferLength), 
             0);

        // Dynamically allocate memory for receive buffer.
        // +1 for null terminator.
        pucReceivedBuffer = malloc(unReceivedBufferLength + 1);

        if (ERROR_CODE == recv(unSocket, pucReceivedBuffer, 
                               unReceivedBufferLength, 0))
        {
            close(unSocket);
        }

        pucReceivedBuffer[unReceivedBufferLength] = '\0';
        printf("%s", pucReceivedBuffer);

        // Deallocate dynamically allocated memory.
        free(pucSendBuffer);
        free(pucReceivedBuffer);
    }
    else
    {
        printf("Invalid Request");
    }

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
static bool clientSocket(int16* punSocket)
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
static bool clientConnect(int16* punSocket, int16* punConnect)
{
    bool blReturn = false;

    // To hold the client address details.
    struct sockaddr_in stSocketAddress = {0};
    stSocketAddress.sin_family = AF_INET;
    stSocketAddress.sin_port = htons(PORT);
    stSocketAddress.sin_addr.s_addr = inet_addr(IP_ADDRESS);

    if ((NULL != punSocket) && (NULL != punConnect))
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
static bool clientSetTimeout(int16* punSocket, int16* punTimeout)
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

//**************************.clientRequestHandler.******************************
// Purpose : Handle POST/GET request and prepare the request buffer to be sent
//           to the server.
// Inputs  : pcMethod - Pointer to the Request method.
//         : ppucBuffer - Pointer to the buffer to be sent to the server.
// Outputs : none
// Return  : blReturn
// Notes   : None
//******************************************************************************
static bool clientRequestHandler(int8 *pcMethod, uint8 **ppucBuffer)
{
    bool blReturn = false;
    uint16 unSendBufferLength = 0;

    if ((NULL != pcMethod) && (NULL != ppucBuffer))
    {
        // Get request
        if (0 == strcmp(pcMethod, "GET")) 
        {
            int8 *pcMessage = getenv("QUERY_STRING");

            if (NULL != strstr(pcMessage, "msg=list"))
            {
                *ppucBuffer = malloc(FILENAME_SIZE);
                memset(*ppucBuffer, 0, FILENAME_SIZE);
                strcpy(*ppucBuffer, "List");
            }
            else
            {
                unSendBufferLength = strlen(pcMessage) + EXTRA_BUFFER_SIZE;
                *ppucBuffer = malloc(unSendBufferLength);
                snprintf((char*)*ppucBuffer, unSendBufferLength, "type=GET&%s", 
                         pcMessage);
            }
        }
        // Post request
        else if (0 == strcmp(pcMethod, "POST"))
        {
            int8 *pcMessage = getenv("QUERY_STRING");

            if (NULL != strstr(pcMessage, "msg=upload"))
            {
                *ppucBuffer = malloc(FILENAME_SIZE);
                memset(*ppucBuffer, 0, FILENAME_SIZE);
                clientFileUpload(*ppucBuffer);
            }
            else
            {
                char *pcLength = getenv("CONTENT_LENGTH");

                if (NULL != pcLength)
                {
                    uint16 unLength = atoi(pcLength);
                    uint8 *pcData = malloc(unLength + 1);
                    fread(pcData, 1, unLength, stdin);
                    pcData[unLength] = '\0';
                    unSendBufferLength = unLength + EXTRA_BUFFER_SIZE;
                    *ppucBuffer = malloc(unSendBufferLength);
                    snprintf(*ppucBuffer, unSendBufferLength, "type=POST&%s", 
                             pcData);
                    free(pcData);
                }
                else
                {
                    printf("Invalid Request");
                }
            }
        }
        else
        {
            printf("Invalid Request Method");
        }

        blReturn = true;
    }

    return blReturn;
}

//****************************.clientFileUpload.********************************
// Purpose : File upload from the POST request extract filename ,save it locally
//         : and forward filename to the server.
// Inputs  : pucBuffer - Pointer to the buffer to be sent to the server.
// Outputs : none
// Return  : blReturn
// Notes   : None
//******************************************************************************
static bool clientFileUpload(uint8 *pucBuffer) 
{
    bool blReturn = false;
    uint16 unFullPathLength = 0;
    FILE *pstFile = NULL;
    int8 *pcContentLength = getenv("CONTENT_LENGTH");
    int8 *pcContentType  = getenv("CONTENT_TYPE");

    if ((NULL != pucBuffer) && (NULL != pcContentLength) && 
        (NULL != pcContentType))
    {
        uint16 unLength = atoi(pcContentLength);
        int8 *pcData = malloc(unLength + 1);

        fread(pcData, 1, unLength, stdin);

        pcData[unLength] = '\0';

        // Extract file name.
        int8 *pcFileNameStart = strstr(pcData, "filename=\"");
        pcFileNameStart += strlen("filename=\"");
        int8 *pcFileNameEnd = strchr(pcFileNameStart, '"');
        uint8 ucFileNameLength = pcFileNameEnd - pcFileNameStart;
        int8 *pcFilename = malloc(ucFileNameLength + 1);

        snprintf(pcFilename, ucFileNameLength +1, "%s", pcFileNameStart);

        // Extract file content.
        int8 *pcFileContentStart = strstr(pcFileNameEnd, "\r\n\r\n");
        pcFileContentStart += 4; 
        int8 *pcBoundaryStart = strstr(pcContentType, "boundary=");
        pcBoundaryStart += strlen("boundary=");
        int8 *pcBoundary = malloc(strlen(pcBoundaryStart) + 3);

        sprintf(pcBoundary, "%s", pcBoundaryStart);

        int8 *pcBoundaryEnd = strstr(pcFileContentStart, pcBoundary);
        int8 *pcFileContentEnd = pcBoundaryEnd;

        if ((pcFileContentEnd - 2 >= pcFileContentStart) && 
            (pcFileContentEnd[-2] == '\r') && (pcFileContentEnd[-1] == '\n'))
        {
            pcFileContentEnd -= 2;
        }

        uint32 ulFileSize = pcFileContentEnd - pcFileContentStart;

        // Save file locally.
        unFullPathLength = strlen(UPLOAD_DIR) + strlen(pcFilename);
        int8 *pcFullPath = malloc(unFullPathLength + 1);

        sprintf(pcFullPath, "%s%s", UPLOAD_DIR, pcFilename);
        sprintf(pucBuffer, "FilePath:%s", pcFullPath);

        pstFile = fopen(pcFullPath, "wb");

        if (NULL != pstFile) 
        {
            fwrite(pcFileContentStart, 1, ulFileSize, pstFile);
        }

        printf("{\"filename\":\"%s\",\"path\":\"%s\"}\n", pcFilename, 
               pcFullPath);
        free(pcFilename);
        free(pcBoundary);
        free(pcFullPath);
        free(pcData);

        blReturn = true;
    }

    return blReturn;
}

// EOF