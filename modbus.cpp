#include "ressources.h"
#include "modbus.h"
#include "mbed.h"
#include "LCD.h"
#include "config.h"

DigitalOut ledConnected(LED1);
DigitalOut enablePont(p6);

//Fonction de connexion à l'automate
void modbus_connect(TCPSocketConnection *sock)
{
    lcd_init();
    lcd_clear();   
    enablePont = 0;
    EthernetInterface eth;
    sock->set_blocking(false,100);
    
    char connectionTab[5][NB_CHAR_CONNEC_INFO];
    loadConnectionInfos((char *)connectionTab);
    
    lcd_printf(0,0,"Connexion a :");
    lcd_printf(0,1,IPAUTOMATE);
    lcd_printf(0,2,"Depuis :");

    if(DHCP){
        lcd_printf(0,3,"DHCP address");
        eth.init();
        }
    else{
        lcd_printf(0,3,IPCARTE);
        eth.init(IPCARTE,MASQUE,PASSERELLE);
        }    
    eth.connect();
    
    // ouverture d'une socket avec l'IP de l'automate sur le port 502 dédié Modbus
    while(sock->connect(IPAUTOMATE, 502))
    {
        wait(0.1);
    }
    if(sock->is_connected()){
        ledConnected = 1;
        enablePont = 1;
        lcd_clear();
        flags = SWITCH_TO_CONNEC_OK;
        flags = SWITCH_TO_H_OK;
        lcd_actu_services();
    }
}

//Fonction de séparation d'un Short (2 octets) en deux Char (2x1 octet)
int splitShort(unsigned short shortToSplit,char *part1,char *part2){
    //part 1 = octet de poids faible
    char p1 = (shortToSplit & 0xFF00)>>8;
    
    //part 2 = octet de poids fort
    char p2 = shortToSplit & 0x00FF;
    
    *part1=p1;
    *part2=p2;
    
    return 0;
    }

//Fonction de fusion de deux Char (2x1 octet) en un Short (2 octets)
int mergeChar(char char1, char char2, short *mergedShort){
    short temp = 0;
    temp = char1;
    temp = temp<<8;
    
    temp += char2;
    *mergedShort = temp;
    
    return 0;
    }

//Fonction de lecture des registres
void modbus_read(short first_register,short nb_registers, TCPSocketConnection *sock, unsigned short* registerTab)
{
    //initialisation
    char buffer[100]={0};
    int count = 0;
    
    if(transactionID==255)
    {
        transactionID=0;
    }
    else
    {
        transactionID++;
    }
    
    //elements de la trame modbus
    char msg_length = 6;
    char unit_id = ADRESSE_MODBUS_API;
    char function_code = 3;
    
    short mergedShort = 0;
    
    char first_reg_part1=0;
    char first_reg_part2=0;
    splitShort(first_register,&first_reg_part1,&first_reg_part2);
    
    char nb_reg_part1=0;
    char nb_reg_part2=0;
    splitShort(nb_registers,&nb_reg_part1,&nb_reg_part2);
    
    char transID_part1=0;
    char transID_part2=0;
    splitShort(transactionID,&transID_part1,&transID_part2);
    
    //reconstitution d'une trame modbus
    char modbus_cmd[12] = {transID_part1,transID_part2,00,00,00,msg_length,unit_id,
    function_code,first_reg_part1,first_reg_part2,nb_reg_part1,nb_reg_part2};
    
    int sendTest = sock->send_all(modbus_cmd, sizeof(modbus_cmd));    
    int receiveTest = sock->receive(buffer, sizeof(buffer));
    
    //Si l'envoie des trames ou la reception du message a échoué, on entre dans une
    //boucle de gestion des erreurs de connexion
    while( ((receiveTest==-1)||(sendTest==-1)) && (count < 100) ){
        //on indique que la connexion est défaillante
        ledConnected = 0;
        flags = SWITCH_TO_CONNEC_NOT_OK;
        if(count == 10){
            //si 10 échanges de suite sont fails, on coupe la puissance
            enablePont = 0;
            flags = SWITCH_TO_H_NOT_OK;
        }
        //actualisation de l'affichage
        lcd_actu_services();
        //on retente l'échange
        sendTest = sock->send_all(modbus_cmd, sizeof(modbus_cmd));
        receiveTest = sock->receive(buffer, sizeof(buffer));
        count++;
        wait(0.1);
    }
    //si toutes les tentatives de connexion ont été infructeuses, on reset le module
    if(count >= 100){
        mbed_reset();
    }
    if(count > 0){
        //si la connexion a été rétabli après un problème, on réactive la puissance
        //et on l'indique à l'utilisateur
        ledConnected = 1;
        enablePont = 1;
        flags = SWITCH_TO_CONNEC_OK;
        flags = SWITCH_TO_H_OK;
        lcd_actu_services();
    }
    count = 0;
    
    //impression du contenu du tableau servant au vidage du buffer
    buffer[receiveTest] = '\0';
    printBuffer(buffer);
    
    //regroupe les octets deux à deux pour reconstituer le contenu des registres
    //et copie dans le tableau registerChar
    for(int k=9;k<2*nb_registers+9;k+=2){
        mergeChar(buffer[k],buffer[k+1],&mergedShort);
        registerTab[(k-9)/2] = mergedShort;
    }
    
}
    


