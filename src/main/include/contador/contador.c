#include "contador.h"


double get_time() 
{
    time_t current_time;

    time(&current_time);
    return difftime(current_time, 0);
}

double duration(double ant)
{
    return get_time() - ant;
}