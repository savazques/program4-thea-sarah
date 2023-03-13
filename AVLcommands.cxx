#include <iostream>
#include <string>
#include <fstream>
#include "json.hpp"
#include "AVL.h"

int main(int argc, char** argv)
{
    std::string filename = argv[1]; 
    std::ifstream commandFile (filename);

    nlohmann::json AVLCOMMANDS  = nlohmann::json::parse(commandFile);
    nlohmann::json OutputJSON;
    nlohmann::json info; 
    AVL tree; 

    for(nlohmann::json::iterator  it = AVLCOMMANDS.begin(); it!= AVLCOMMANDS.end(); it ++ )
    {
        if(it.key() != "metadata")
        {
            int value = info[it.key()]["key"]; 
            tree.Insert(value); 

        }
    }; 
    return 0;
};