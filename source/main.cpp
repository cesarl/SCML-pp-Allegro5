#include <iostream>
#include "SCMLpp.h"
#include "main.h"

using namespace std;



int main(int argc, char* argv[])
{
    SCML::Data data("samples/monster/Example.SCML");
    //SCML::Data data("samples/knight/knight.scml");
    data.log();
    
    if(!init(800, 600))
        return 1;
    
    main_loop(&data);
    
    quit();
    data.clear();
    
    return 0;
}
