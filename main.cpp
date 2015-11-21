#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "ressources.h"
#include "mbed.h"
#include "modbus.h"
#include "frameBuilder.h"
#include "config.h"
#include "Text_LCD.h"

//Initialisation des variables globales
unsigned short flags = 0x8;     // décrit dans ressources.h
unsigned long long frameTab[NB_LOCO_MAX];
unsigned long long masqueParcours = 0x2000000000;
unsigned int indexTrame = 0;
unsigned short transactionID = 0;

//Configuration du port COM
RawSerial Com_pc(USBTX,USBRX);

DigitalOut cmdPont(p5);     // broche de commande du pont
Ticker T1;                  // Ticker de génération des trames de bits DCC vers le pont H (dccBinaryCoder)
Ticker T2;                  // Ticker de gestion et rafréchissement de l'afficheur LCD (LCD_Control)

//Fonction de debuggage
int printDebug(int debug){
    Com_pc.printf("%i\n",debug);
    return 0;
    }
//Fonction de debuggage   
int binaryConversion(unsigned long long target)
{
    //Cette fonction prend un nombre target en paramètre pour l'imprimer en binaire  
    
    int t[38] = {0};
    int k = 0;
    int b = 0;

    while(target !=0){
        b = target % 2;
        t[k]=b;
        target /= 2;
        k++;
    }
    //inversion de l'ordre des bits de target    
    for (k=37;k>=0;k--){
        //37 bits affichés. Modifier la valeur de i selon la longueur désirée
        Com_pc.printf("%i",t[k]);
    }
    Com_pc.printf("\n");

    return 0;
}

//Fonction servant au vidage du buffer pour l'interrogation de l'automate    
void printBuffer(char *buffer){
    Com_pc.printf("%s",buffer);
    }

void dccBinaryCoder()
{
    /*
    Cette fonction est appelée à chaque interruption à intervalles réguliers de 58us
    Elle gère les permissions de lecture et écriture, et code les bits des trames contenues dans frameTab
    
    COMPTEUR_CYCLE représente le nombre de cycles restant avant d'avoir fini de coder l'état logique
    Avant de coder un 0, on met cette variable à 2 pour rentrer 2 fois dans l'interruption et basculer ensuite
    Même chose pour coder un 1, en mettant la variable à 1
    La valeur de COMPTEUR_CYCLE est contenue dans deux bits de la variable flags
    
    LOGIC_STATE est l'état logique actuel de la sortie
    Il permet à la fonction appelée lors de l'interruption de se repérer
    
    masqueParcours est un masque comprenant un seul 1 et que des 0
    Le bit à 1 se décale pour permettre de transmettre chacun des bits de la trame
    */
    
    if((COMPTEUR_CYCLE) == 0){
        if ((masqueParcours & frameTab[indexTrame]) == 0){
            //codage d'un 0 logique : passer le nombre de cycles restant à 2
            flags = flags + 2;
        }
        else {
            //codage d'un 1 logique : 1 cycle restant
            flags = flags + 1;
        }
        
        if ((LOGIC_STATE) == 0){
            //permet de savoir si le basculement se fait vers l'état haut ou l'état bas
            flags = flags | 0x4; //mise à jour de l'information d'état logique codé dans la variable flags
            cmdPont = 1;
        }
        else {
            flags = flags & ~0x4;
            cmdPont = 0;
            masqueParcours = masqueParcours >> 1;
        }
    }
    
    //Gestion des autorisations d'accès
    if(masqueParcours == 0){
        //arrivée à la fin d'une trame
        masqueParcours = 0x2000000000; //remise au début du masque
        if((indexTrame==NB_LOCO-1)&&(MODE_TEST==INIT_MODE)){
            flags = SWITCH_TO_IDLE_MODE;//On passe en mode idle
            indexTrame++;
        }
        else if(indexTrame==NB_LOCO){ //si la trame IDLE est maintenant envoyée
            //si les registres sont à jour et si on a fini la copie
            if(MODE_TEST == COPY_DONE){
                 flags = SWITCH_TO_INIT_MODE;//on sort du mode idle
                 indexTrame = 0;//on repasse à la lectures des trames   
            }
            else{
            }  
        }
        else{
            indexTrame++;//sinon on incrémente et on passe à l'envoi de la trame suivante
        }     
    }    
    flags = flags - 1; //on décrémente le nombre de cycles restants
}

void LCD_Control()
{
    if(ACTU_ON == 0){
        flags = SWITCH_TO_ACTU_ON;
    }
    if((LCD_NUMBR == 0)&&(NB_LOCO > NB_LIGNE_LCD)){
        flags = SWITCH_TO_LCD_1;
    }
    else{
        flags = SWITCH_TO_LCD_0;
    }
}

void set_flags_NB_LOCO()
{
    short nb_loco_temp = 0;
    nb_loco_temp = locoCount();
    flags = flags | (nb_loco_temp << 5);  
}

int main(){       
    //Mise à 0 des sorties
    cmdPont = 0;
    
    //Initialisation des variables
    unsigned short adresseTab[NB_LOCO_MAX];
    char nameTab[NB_CHAR_LOCO_NAME][NB_LOCO_MAX];
    
    //Configuration du port Com
    Com_pc.format(8,SerialBase::None,1);
    Com_pc.baud(115200);
    
    //Connexion à l'automate
    TCPSocketConnection sock;
    modbus_connect(&sock);
    
    //Récupération des informations du fichier CONFIG
    set_flags_NB_LOCO();
    loadFromFile((char *)nameTab, adresseTab);
    
    //Construction du tableau (ne sera pas interrompu). Pour qu'au début de l'envoi des trames (au démarrage des interruptions) 
    //des trames idle soient envoyées
    for(int j = 0;j<NB_LOCO;j++){
        frameTab[j] = 0x3FF7F801FF;
    }
    frameTab[NB_LOCO] = 0x3FF7F801FF;//Mise en place de la trame idle en dernière place
    
    // Déclaration de la fonction de contrôle du LCD en interruption toutes les secondes 
    T1.attach(&LCD_Control,1);
    // Déclaration de la fonction de codage des trames en interruption toutes les 58 microseconds
    T2.attach_us(&dccBinaryCoder,58);

    while(1)
    { 
        frameBuilder(&sock, (char *) nameTab, adresseTab);
        wait(0.01);//Permet le bon fonctionnement des communications réseau
        //la fonction frameBuilder interroge les registres en permanence pour alimenter un tableau temporaire
        //ce tableau sera copié dans la variable globale frameTab lorsque l'autorisation sera donnée
    }
    
    return 0;
}