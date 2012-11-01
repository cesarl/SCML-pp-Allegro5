#include "main.h"

int main(int argc, char* argv[])
{
    if(!init(800, 600))
        return 1;
    
    main_loop();
    
    quit();
    return 0;
}
