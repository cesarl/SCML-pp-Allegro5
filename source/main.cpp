#include <iostream>
#include "SCMLpp.h"
#include "renderer.h"

using namespace std;



int main(int argc, char* argv[])
{
    SCML::Data data("monster/Example.SCML");
    data.log();
    
    if(!init(800, 600))
        return 1;
    
    main_loop(&data);
    
    quit();
    data.clear();
    
    return 0;
}
