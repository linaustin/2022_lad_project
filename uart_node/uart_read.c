#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>

int main(){
    int serial_port = open("/dev/ttyUSB0", O_RDWR);
/*
    if(serial_port < 0){
        printf("error %i from open : %s \n", errno, strerror(errno));
    }
    else{
        printf("open succeesfully!!!\n");
    }
*/

    struct termios tty;

    if(tcgetattr(serial_port, &tty) != 0){
        printf("error %i from tcgetattr : %s\n", errno, strerror(errno));
    }
    else{
        printf("tcgetattr successfully!!!\n");
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
        printf("tcsetattr successfully!!!\n");
    }

    char read_buffer[20];

    memset(&read_buffer, '\0', sizeof(read_buffer));

    while(1){
        memset(&read_buffer, '\0', sizeof(read_buffer));

        int num_bytes = read(serial_port, &read_buffer, sizeof(read_buffer));

        if (num_bytes < 0) {
            printf("Error reading: %s\n", strerror(errno));
        }

        printf("Read %i bytes. Received message: %s\n", num_bytes, read_buffer);
        /*
        printf("raw msg is : ");
        for(int i = 0; i < sizeof(read_buffer); i++){
            printf("%u ",read_buffer[i]);
        }
        printf("\n");
        */
    }
    


    return 0;
}