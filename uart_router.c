
#include <unistd.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

/* Driver Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/UART.h>

/* Example/Board Header files */
#include "Board.h"


unsigned char frame1[] = {0x7E, 0x00, 0x2A, 0x10, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFE, 0x00, 0x00, 0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x57, 0x6F, 0x72, 0x6C, 0x64, 0x2E, 0x20, 0x54, 0x68, 0x69, 0x73, 0x20, 0x69, 0x73, 0x20, 0x72, 0x6F, 0x75, 0x74, 0x65, 0x72, 0x31, 0x01};

/*
 *  ======== mainThread ========
 */


void *mainThread(void *arg0, unsigned char frameMain[])
{

    UART_Handle uart0;
    UART_Handle uart1;

    UART_Params uartParams;
    UART_Params uartParams1;


    /* Call driver init functions */
    UART_init();

    /* Create a UART with the parameters below. */
    UART_Params_init(&uartParams);
    uartParams.writeMode = UART_MODE_BLOCKING;
    uartParams.readMode = UART_MODE_BLOCKING;
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.baudRate = 115200;
    uartParams.readEcho = UART_ECHO_OFF;

    UART_Params_init(&uartParams1);
    uartParams1.writeMode = UART_MODE_BLOCKING;
    uartParams1.writeDataMode = UART_DATA_BINARY;
    uartParams1.readDataMode = UART_DATA_BINARY;
    uartParams1.readReturnMode = UART_RETURN_FULL;
    uartParams1.readEcho = UART_ECHO_OFF;
    uartParams1.baudRate = 115200;


    uart0 = UART_open(Board_UART0, &uartParams);
    uart1 = UART_open(Board_UART1, &uartParams1);

    if (uart0 == NULL) {
        /* Error creating UART0 */
        while (1);
    }

    if (uart1 == NULL) {
        /* Error creating UART1 */
        while (1);
    }

    /* Stop the receive to go into low power state */
    UART_control(uart0, UART_CMD_RXDISABLE, NULL);

    const char lineBreak[] = "\r\n";

    /* Write to UART0 & 1 */
    while (1) {
        /*KORY ADD YOUR CODE HERE, WHEN YOU RECEIVE A TEXT FROM
         * THE USER PASS THAT INTO THE TEXT CHAR[]
         *
         *
         * */

    unsigned char text[]  = "Hello world testing custom packet"; // Kory here is where you add the packet
    unsigned char startDelimiter[1] = {0x7E};
    unsigned char length[2] = {0x00, 0x0E}; //need to calculate this
    unsigned char frameTypeAndID[2] = {0x10, 0x01};
    unsigned char destAddresses[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0XFE};
    unsigned char broadcastRadiusAndOptions[2] = {0x00, 0x00};

    int textSize = strlen(text);
    unsigned char rfData[(int) textSize + 1]; // plus 1 for the checksum
    strcpy(rfData, text);

    //calculate length
    int lengthSize = 0x0E + (int) textSize;
    length[1] = lengthSize;

    int byteCount = 18 + (int)textSize;

    //calculate checksum
    int sum = 0x10 + 0x01 + 0x00 + 0x00 + 0x00 + 0x00 + 0x00 + 0x00 + 0x00 + 0x00 + 0xFF + 0xFE;
    int i;
    for (i = 0; i < textSize; i++) {
        sum += (int) rfData[i];
    }
    int low8bits = sum & 0xFF;
    int checksumInt = 0xFF - low8bits;
    rfData[textSize] = checksumInt;

    unsigned char everythingButData[] = {0x7E, 0x00, 0x0E, 0x10, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                         0xFF, 0XFE, 0x00, 0x00};

    everythingButData[2] = length[1];
    int total_length = 17 + textSize + 1;

    unsigned char totalPacket[total_length];

    for (i = 0; i < 17; i++) {
        totalPacket[i] = everythingButData[i];
    }

    int j = 0;
    for (i = 17; i < byteCount; i++) {
        totalPacket[i] = rfData[j];
        j++;
    }

	while(
        //write to console
        UART_write(uart0, totalPacket, sizeof(totalPacket));
        UART_write(uart1, totalPacket, sizeof(totalPacket));
        UART_write(uart0, lineBreak, sizeof(lineBreak));

        UART_write(uart0, (void *)frame1, sizeof(frame1));
        UART_write(uart1, (void *)frame1, sizeof(frame1));
        UART_write(uart0, lineBreak, sizeof(lineBreak));

        sleep(1);



    }//while
}//main
