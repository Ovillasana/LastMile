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

/*
 *  ======== mainThread ========
 */

void *mainThread(void *arg0) {
    char input;
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
    const char coord[] = "==========================================\n\r\tCOORDINATOR\n\r==========================================";
    const char wegot[] = "outside while loop";

    char message[255] = {0x00};

    int i = 0;
    int j = 0;
    int byte_count = 1;
    int length = 0;

    UART_write(uart0, &coord, sizeof(coord));
    UART_write(uart0, &lineBreak, sizeof(lineBreak));

    while (1) {

        for (j = 0; j < 255; j++) {
             message[j] = 0x00;
            }

        while (UART_read(uart1, &input, sizeof(input))) {
            byte_count++;
            if (byte_count-1 == 3) {
                length = input + 4;
            }
            if (byte_count-1 >= 16) {
                //UART_write(uart0, &input, sizeof(input));
                message[i] = input;
                i++;
                if (byte_count-1 == length - 1) {
                    break;
                }
            }
        }

        UART_write(uart0, &lineBreak, sizeof(lineBreak));
        UART_write(uart0, &message, sizeof(message));
        UART_write(uart0, &lineBreak, sizeof(lineBreak));
        i = 0;
        j = 0;
        byte_count = 0;
        length = 0;


        /***
         * KORY THIS IS THE CODE FOR THE GATEWAY NODE
         * WE SHOULD JUST POST DATA BEING SENT FROM THE ROUTERS TO THE HTTP SERVER YOU CREATED,
         * THE COORDINATOR SHOULD NOT SEND MESSAGES BACK AS IT WILL GET VERY MESSY
         * THE MESSAGE IS IN THE CHAR ARRAY MESSAGE[]
         *
         */

    }//while
}//main
