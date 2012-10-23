#include <iostream>
#include "SCMLpp.h"
#include "main.h"

using namespace std;



int main(int argc, char* argv[])
{
    if(!init(800, 600))
        return 1;
    
    main_loop();
    
    quit();
    return 0;
}
