#include <iostream>
#include <string>
#include <fstream>
#include "json.hpp"
#include "AVL.cpp"

int main(int argc, char** argv)
{
    std::string filename = argv[1]; 
    std::ifstream commandFile (filename);

    nlohmann::json AVLCOMMANDS  = nlohmann::json::parse(commandFile);
    nlohmann::json OutputJSON;

    for(nlohmann::json::iterator  it = AVLCOMMANDS.begin(); it!= AVLCOMMANDS.end(); it ++ )
    {
        
    }; 
    return 0;
};