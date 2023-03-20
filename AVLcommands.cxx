#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include "json.hpp"

#include "AVL.h"


int main(int argc, char** argv)
{
    std::string filename = argv[1]; 
    std::ifstream commandFile (filename); 

    nlohmann::json AVLCOMMANDS  = nlohmann::json::parse(commandFile);
    //nlohmann::json OutputJSON;
    AVL tree; 

    for(nlohmann::json::iterator  it = AVLCOMMANDS.begin(); it!= AVLCOMMANDS.end(); it ++ )
    {
        if(it.key() != "metadata")
        {
            int value = AVLCOMMANDS[it.key()]["key"]; 
            std::cout<<"before inserting a vlaue in main"<<std::endl;
            tree.Insert(value); 
            std::cout<<"after insertinga value in main "<<std::endl;
      
        }
    }; 

    
    std::cout << tree.JSON() << std::endl; 

     
    

    return 0;
};