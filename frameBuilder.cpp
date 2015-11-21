#include "ressources.h"
#include "modbus.h"
#include "frameBuilder.h"
#include "LCD.h"
#include "config.h"
#include "Text_LCD.h"


int frameBuilder(TCPSocketConnection *sock, char *nameTab, unsigned short *adresseTab)
{
    //Déclaration des variables de vitesse et de direction
    unsigned short vitesseTab[NB_LOCO_MAX];
    unsigned short directionTab[NB_LOCO_MAX];      
    //Création du tableau de stockage temporaire des trames nouvellement construites
    unsigned long long frameTabTemp[10];
    //Création des variables temporaires servant à la construction des trames
    unsigned long long adresse = 0;
    unsigned short vitesse = 0;
    unsigned short direction = 0;
    //Création d'un tableau pour le stockage des registres
    unsigned short registerTab[50] = {0};

    //Lecture des registres dans l'automate
    modbus_read(ADRESSE_BASE_DATA_API,NB_LOCO*2,sock,registerTab);
    
    //récupération des infos dans les tableaux
    for(int cop = 0 ; cop < NB_LOCO*2 ; cop+=2){
        if(registerTab[cop + 1]<15){
            vitesseTab[cop/2] = registerTab[cop + 1];
        }
        else{
            vitesseTab[cop/2] = 15;
        }
        directionTab[cop/2] = registerTab[cop] & 0x1; 
    }
    
    //Affichage des informations fraichement récupérées
    lcd_actu(adresseTab,(char*)nameTab,directionTab,vitesseTab);
    
    for(int count = 0 ; count < NB_LOCO ; count++){    
        //adresse DCC locomotive
        adresse = adresseTab[count];
        vitesse = vitesseTab[count];
        direction = directionTab[count];
        
        //initialisation de la trame finale à transmettre
        unsigned long long trame_finale = 0;
        //initialisation du masque contenant des zeros aux emplacements à mettre à jour
        unsigned long long masque_initial = 0x3FF0000001;
        //initialisation des bits de direction/vitesse
        unsigned long long octetVitesseDirection = 0;
        //initialisation de l'octet de vérification
        short verification = 0;
        //creation masque mise a jour flags
    
        //creation de l'octet contenant les informations de vitesse et direction
        //décalage du bit de direction
        direction = direction << 5;
        //ajout du 1 permettant le codage en 14/28 pas et du bit de commande des feux (toujours à 1)
        direction = direction | 0x50;
        //ajout des bits de vitesse
        octetVitesseDirection = direction | vitesse;
        //fabrication de l'octet de vérification à partir de direction et vitesse
        verification = octetVitesseDirection ^ adresse;
    
        //décalage de l'adresse
        adresse = adresse << 19;
        //décalage de la vitesse/direction
        octetVitesseDirection = octetVitesseDirection << 10;
        //décalage de la vérification
        verification = verification << 1;
    
        //rassemblement des octets décalés
        trame_finale = masque_initial | octetVitesseDirection | adresse | verification;
        //insertion de la trame dans le tableau des trames pour les différentes locomotives
        frameTabTemp[count] = trame_finale;
    }
    
    //si on est dans le mode idle et si les registres ne sont pas à jour, on peut lancer la copie
    if(MODE_TEST == IDLE_MODE){
        flags = SWITCH_TO_COPY_START;
        //copie du tableau
        for(int t = 0; t<NB_LOCO; t++){
            frameTab[t] = frameTabTemp[t];
        }
        flags = SWITCH_TO_COPY_DONE; //La copie est terminée
        //Les registres sont à jour  
    }
    return 0;
}
