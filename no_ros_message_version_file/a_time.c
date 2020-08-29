#include <stdio.h>
#include <time.h>
#include <stdlib.h>
void main (void){

    time_t          s; 
    time_t          s_last = 0; 
    struct timespec spec;
    int counter = 0;
    while(1)
	{
        clock_gettime(CLOCK_REALTIME, &spec);
        s  = spec.tv_sec;
        if (s_last != s)
        {
            system("echo 1 > /sys/class/gpio/gpio26/value");
            counter = 0;
	        printf("nano sec: %d  \n",spec.tv_nsec);
        }
        s_last = s;
        counter++;
        if (counter==100000)
        {
            system("echo 0 > /sys/class/gpio/gpio26/value");
        }
    }
}