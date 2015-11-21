#include "config.h"
#include "ressources.h"
#include <iostream>
#include <fstream>
#include <stdio.h>

LocalFileSystem local("local");

//
int loadFromFile(char *nameTab,unsigned short *address)
{
    //permet de relever les infos relatives aux trains, à savoir les noms et adresses
    ifstream  file = fopen("/local/config.ini", "r");
    string line;
    string name_temp;
    int pos=0; //pour savoir ou est la virgule qui délimite le nom de l'adresse
    short nb_loco=0;
      
    int address_temp;
    if(file){
        while(getline(file,line)){
            //ne prend pas les informations de connexion
            if (line.substr(0,1) != "*"){
                pos = line.find(",");
                //stockage des noms et adresses individuels dans des tableaux temporaires après séparation
                address_temp = atoi((line.substr(pos + 1)).c_str());
                address[nb_loco] = address_temp;
    
                name_temp = line.substr(0,pos);
                strcpy(&nameTab[nb_loco*NB_CHAR_LOCO_NAME],name_temp.c_str());
                
                nb_loco++;
                }
            }
            file.close();
        }
    return 0; 
}

short locoCount(){
    //retourne le nombre de locomotive en fonction du nombre de lignes concernées dans le fichier texte
    int locoCount = 0;
    string temp = "";
    ifstream  file = fopen("/local/config.ini", "r");
    while(getline(file,temp)){
        if (temp.substr(0,1) != "*"){//ne considère que  les lignes relatives aux trains
            locoCount++;
        }
    }
    return locoCount;
}        

void loadConnectionInfos(char *connectionTab){
    //permet d'importer les informations de connexion à l'automate
    //à savoir l'utilisation ou non du DHCP, la passrelle, le masque, les IP carte et automate
    ifstream  file = fopen("/local/config.ini", "r");
    
    string line;
    string info_temp;
    short pos =0;
    short row = 0;
    

    while(getline(file,line)){
        if (line.substr(0,1) == "*"){//ne considère que les lignes relatives aux infos de connexion
            pos = line.find(" ");
            info_temp = line.substr(pos+1);
            strcpy( &connectionTab[row*NB_CHAR_CONNEC_INFO] , info_temp.c_str() );
            row++;
            }
    }
}

int loadFromFile(unsigned short *address)
{
    //fonction permettant d'obtenir les adresses des trains sans les noms
    //cette fonction n'est plus utilisée
    ifstream  file = fopen("/local/config.ini", "r");
    vector <int> address_local;
    string line="";
    int pos=0;
    short nb_loco=0;
    int address_temp=0;
    
    if(file){
        while(getline(file,line)){
            pos = line.find(",");
            address_temp = atoi( (line.substr( pos+1,line.size() )).c_str() ) ;
            
            address_local.push_back(address_temp);
            address[nb_loco]=address_local[nb_loco];
            nb_loco++;
            }
        }      
    return 0; 
}