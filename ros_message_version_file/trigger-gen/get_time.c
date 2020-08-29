#include <stdio.h>
#include <time.h>
#include <stdlib.h>
int main (void){

    time_t          s; 
    time_t          s_last; 
    struct timespec spec;

    time_t          pps_s; 
    time_t          pps_ns; 
    int counter = 0;
    clock_gettime(CLOCK_REALTIME, &spec);
    s_last = spec.tv_sec;
    int is_trig = 0;
    while(1)
    {
        clock_gettime(CLOCK_REALTIME, &spec);
        s  = spec.tv_sec;
        if (s_last != s)
        {
            system("echo 1 > /sys/class/gpio/gpio26/value");
            counter = 0;
            is_trig = 1;
	        pps_s = s;
            pps_ns = spec.tv_nsec;
            printf("%lds %ldns\n",pps_s,pps_ns);
        }
        s_last = s;
        if (is_trig) counter++;
        if (counter==100000)
        {
            system("echo 0 > /sys/class/gpio/gpio26/value");
            is_trig = 0;
	        break;
        }
        

    }
    
    return 0;

}