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
    //nlohmann::json OutputJSON;
    AVL tree; 

    for(nlohmann::json::iterator  it = AVLCOMMANDS.begin(); it!= AVLCOMMANDS.end(); it ++ )
    {
        if(it.key() != "metadata")
        {
            int value = AVLCOMMANDS[it.key()]["key"]; 
            std::cout << value << std::endl; 
            tree.Insert(value); 
            std::cout << "this is the root = " << tree.root_ << std::endl; 
        }
    }; 
    std::cout<<tree.JSON()<<std::endl; 
    return 0;
};