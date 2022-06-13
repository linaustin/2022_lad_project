#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>

int main(){
    int serial_port = open("/dev/ttyUSB0", O_RDWR);

    if(serial_port < 0){
        printf("error %i from open : %s \n", errno, strerror(errno));
    }
    else{
        //printf("open succeesfully!!!\n");
    }


    struct termios tty;

    if(tcgetattr(serial_port, &tty) != 0){
        printf("error %i from tcgetattr : %s\n", errno, strerror(errno));
    }
    else{
       // printf("tcgetattr successfully!!!\n");
    }

    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;

    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;

    tty.c_cflag &= ~CRTSCTS;

    tty.c_cflag |= CREAD | CLOCAL;


    tty.c_lflag &= ~ICANON;
    tty.c_lflag &= ~ECHO;
    tty.c_lflag &= ~ECHOE;
    tty.c_lflag &= ~ECHONL;
    tty.c_lflag &= ~ISIG;

    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL);

    tty.c_oflag &= ~OPOST; 
    tty.c_oflag &= ~ONLCR;

    tty.c_cc[VTIME] = 10;
    tty.c_cc[VMIN] = 0;

    cfsetispeed(&tty, B9600);
    cfsetospeed(&tty, B9600); 

    if(tcsetattr(serial_port, TCSANOW, &tty) != 0){
        printf("error %i from tcsetattr: %s\n", errno, strerror(errno));
    }
    else{
        //printf("tcsetattr successfully!!!\n");
    }

    uint8_t rx_Data[10];
    uint8_t tx_Data[10];
    
    for(int i = 0; i < 10; i++){
        rx_Data[i] = 0;
        tx_Data[i] = 0;
    }

    tx_Data[0] =  255;
    tx_Data[9] = 254;

    char cmd;

    while(1){

        printf("enter the command :");
        scanf("%c", &cmd);
        getchar();

        switch(cmd){
            case 'w':
                tx_Data[1] = 11;
                tx_Data[2] = 5;

                break;
            
            case 's':
                tx_Data[1] = 12;
                tx_Data[2] = 5;

                break;

            case 'a':
                tx_Data[1] = 13;
                tx_Data[2] = 1;

                break;

            case 'd':
                tx_Data[1] = 14;
                tx_Data[2] = 1;

                break;

            default:
                break;
        }

        write(serial_port, tx_Data, 10);

        for(int i = 0; i < 10; i++){
            rx_Data[i] = 0;
        }

        uint8_t rx_Temp;

        while(1){
            read(serial_port, &rx_Temp, 1);

            for(int i = 0; i < 9; i++){
                rx_Data[i] = rx_Data[i+1];
            }

            rx_Data[9] = rx_Temp;

            if(rx_Data[0] == 255 && rx_Data[9] == 254){
                break;
            }
        }
      
        printf("Received message:");

        for(int i = 0; i < 10; i++){
            printf("%u ",rx_Data[i]);
        }

        printf("\n");
    }
    


    return 0;
}