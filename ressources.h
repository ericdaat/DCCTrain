#ifndef RESSOURCES_H_INCLUDED
#define RESSOURCES_H_INCLUDED

/*  Description du registre d'états : flags */
//                             H_OK      LCD_NUMBR                      MODE_TEST
//                            |----|      |----|                      |----------|
//   [ b15   b14   b13   b12 ][ b11   b10   b9   b8 ][  b7   b6   b5   b4 ][  b3   b2   b1   b0 ] 
//                      |----|      |----|     |-------------------|             |----||--------|
//                    CONNEC_OK     ACTU_ON           NB_LOCO              LOGIC_STATE COMPTEUR_CYCLE
//
   
/*                   1              1              1              0              1              0              1
                    |----------------|            |----------------|       |-----------|       |----------------| 
                         NB_LOCO                  MODE_TEST. Utilisé        LOGIC_STATE          COMPTEUR_CYCLE
                                                 pour changer le mode       Etat logique     Nombre de cycle restant
                                                 d'accès au tableau         actuellement     avant d'avoir fini
                                                     des trames             codé en DCC      le codage du LOGIC_STATE
*/

#define COMPTEUR_CYCLE  (0x3 & flags)           // Nombre de cycle restant avant d'avoir fini le codage du LOGIC_STATE
#define LOGIC_STATE     (0x4 & flags)           // Etat logique actuellement codé en DCC
#define NB_LOCO         ((0x1E0 & flags)>>5)    // Nombre de locomotives gérées sur le réseau
#define NB_LOCO_MAX         8                   // Nombre maximal de LOCO géré par le programme (8 optimisé pour l'afficheur 4 lignes)
#define NB_CHAR_LOCO_NAME   10
#define NB_CHAR_CONNEC_INFO 16
#define NB_LIGNE_LCD        4

#define MODE_TEST       (0x0018 & flags)        //Permet de tester le mode dans lequel on se trouve (exemple : if(MODE_TEST == INIT_MODE))

#define INIT_MODE   (0x8)   //Etat dans lequel on envoie les trames "standards", la copie est interdite, on sort de ce mode quand toutes les trames standards ont été envoyées
#define IDLE_MODE   (0x18)  //Etat dans lequel on envoie des trames idle, autorisant du même coup la copie
#define COPY_START  (0x10)  //Etat dans lequel on copie les registres (trames idle envoyées en parallèle)
#define COPY_DONE   (0x0)   //Etat dans lequel la copie est terminée et les registres sont à jour (on peut repasser en init mode)

//Pour passer d'un mode à l'autre (l'ordre ne peut pas être changé). Exemple d'utilisation : flags = SWITCH_TO_INIT_MODE
#define SWITCH_TO_INIT_MODE     (flags | 0x0008)
#define SWITCH_TO_IDLE_MODE     (flags | 0x0010)
#define SWITCH_TO_COPY_START    (flags & 0xFFF7)
#define SWITCH_TO_COPY_DONE     (flags & 0xFFE7)

//Permet de passer de l'affichage du LCD 0 à l'affichage de LCD 1 et vice versa
#define SWITCH_TO_LCD_0         (flags & 0xFDFF)
#define SWITCH_TO_LCD_1         (flags | 0x0200)

//Permet d'autoriser ou non l'actualisation de l'affichage
#define SWITCH_TO_ACTU_ON       (flags | 0x0400)
#define SWITCH_TO_ACTU_OFF      (flags & 0xFBFF)

//Permet de tester si le LCD 1 ou le LCD 0 est affiché
#define LCD_NUMBR               ((0x0200 & flags)>>9)
//Permet de tester si l'actualisation est activé (si TEST_ACTU_ON == 1, alors l'actualisation est activée)
#define ACTU_ON                 ((0x0400 & flags)>>10)

//si H_OK est à 1, le pont en H est activé, si H_OK est à 0
#define H_OK                    ((0x0800 & flags)>>11)
//si CONNEC_OK est à 1, le module est connecté à l'automate. Si CONNEC_OK est à 0, le module n'est pas connecté
#define CONNEC_OK               ((0x1000 & flags)>>12)

//Permettent de passer d'un état à l'autre pour le pont en H
#define SWITCH_TO_H_OK          (flags | 0x0800)
#define SWITCH_TO_H_NOT_OK      (flags & 0xF7FF)
//Permettent de passer d'un état à l'autre pour la connexion
#define SWITCH_TO_CONNEC_OK     (flags | 0x1000)     
#define SWITCH_TO_CONNEC_NOT_OK (flags & 0xEFFF)

//Informations Automate
#define ADRESSE_BASE_DATA_API   902
#define ADRESSE_MODBUS_API      1


//Eléments de l'afficheur LCD
#define C_AR            0
#define C_AV            1
#define C_PONTOK        2
#define C_PONTNOK       3
#define C_CONNECT       4
#define C_TRAIN         5

extern unsigned short flags;
extern unsigned short transactionID;
extern unsigned long long frameTab[NB_LOCO_MAX];

#endif // RESSOURCES_H_INCLUDED