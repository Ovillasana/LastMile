
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



const unsigned char frame1[] = {0x7E, 0x00, 0x2A, 0x10, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFE, 0x00, 0x00, 0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x57, 0x6F, 0x72, 0x6C, 0x64, 0x2E, 0x20, 0x54, 0x68, 0x69, 0x73, 0x20, 0x69, 0x73, 0x20, 0x72, 0x6F, 0x75, 0x74, 0x65, 0x72, 0x31, 0x01};

/*
 *  ======== mainThread ========
 */
void *mainThread(void *arg0)
{
    char        input;

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

    const char starts[] = "\nStarting  loop\r\n";
    const char starts2[] = "\nSending packet now\r\n";
    const char lineBreak[] = "\r\n";
    const char reading[] = "\nReading now:\r\n";
          char messageInput[] = "";


    /* Write to UART0 & 1 */
    while (1) {
        //write to console
        UART_write(uart0, (const void *)starts, sizeof(starts));
        UART_write(uart0, (const void *)starts2, sizeof(starts2));
        UART_write(uart0, (const void *)frame1, sizeof(frame1));
        UART_write(uart0, (const void *)lineBreak, sizeof(lineBreak));

        //write to zigbee
        UART_write(uart1, (const void *)frame1, sizeof(frame1));

        //write to console
        UART_write(uart0, (const void *)reading, sizeof(reading));
        sleep(1);

        if(UART_read(uart1, &input, sizeof(input))){

        }

        while(1){
            //read from zigbee
            UART_read(uart1, &input, sizeof(input)); //uart to uart1
            //write to console received
            UART_write(uart0, &input, sizeof(input));
            UART_write(uart0, (const void *)lineBreak, sizeof(lineBreak));
            strcat(messageInput, input);
            printf("%s", messageInput);

           // sleep(5);
           // break;
        }



    }//while
}//main
