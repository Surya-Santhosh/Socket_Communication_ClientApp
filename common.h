//******************************* common ***************************************
// Copyright (c) 2025 Trenser Technology Solutions
// All Rights Reserved 
//****************************************************************************** 
// 
// Summary : Typedefs are included.
// Note    : None
// 
//******************************************************************************
#ifndef COMMON_H
#define COMMON_H 

//**************************** Include Files ***********************************
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <cjson/cJSON.h>
#include <unistd.h>
#include <limits.h>
#include <dirent.h>

//**************************** Global Types ************************************
typedef unsigned short uint16;
typedef unsigned char uint8;
typedef unsigned long uint32;
typedef char int8;
typedef signed short int16;

//************************* Global Constants ***********************************
#define UPLOAD_DIR                       "/var/www/uploads/"
#define FILE_LIST_PATH                   "/var/www/uploads/files.txt"
#define NUM_FILES                        (20)
#define FILENAME_SIZE                    (50)
#define FILE_PATH_SIZE                   (256)
#define FILE_CONTENT_SIZE                (512)
#define ERROR_CODE                       (-1)
#define IP_ADDRESS                       ("172.24.253.157")
#define PORT                             (8080)
#define EXTRA_BUFFER_SIZE                (20)
#define CLIENT_RECEIVE_TIMEOUT           (2)

//************************* Global Variables *********************************** 

//************************* Forward Declarations *******************************  

//************************ Inline Method Implementations *********************** 

#endif 

// COMMON_H 
// EOF
