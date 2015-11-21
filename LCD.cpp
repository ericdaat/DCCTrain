#include "Text_LCD.h"
#include "ressources.h"

TextLCD lcd(p15, p16, p17, p18, p19, p20,TextLCD::LCD20x4); // rs, e, d4-d7  (R/W bloqué à W=0)

char trainTab[NB_LOCO_MAX][NB_CHAR_LOCO_NAME];
char C_pontOk[]={0x14,0x1C,0x14,0x00,0x01,0x02,0x14,0x8};       // Hv
char C_pontNotOk[]={0x14,0x1C,0x14,0x00,0x05,0x02,0x05,0x00};   // Hx
char C_Av[]={0x0C,0x12,0x1E,0x12,0x00,0x05,0x05,0x02};          // Av
char C_Ar[]={0x0C,0x12,0x1E,0x12,0x00,0x03,0x04,0x04};          // Ar
char C_ConnectOk[]={0x00,0x02,0x1F,0x02,0x08,0x1F,0x08,0x00};   // <->
char C_train[]={0x00,0x0E,0x1F,0x11,0x1F,0x15,0x1F,0x0A};       // loco

int lcd_clear(){
    lcd.cls();
    return 0;
}

// Affichage sur LCD pour debuggage
int lcd_printf(int column,int row,char *message){
    lcd.locate(column,row);lcd.printf("%-s",message);
    return 0;
}

// Actualisation des données des locomotives (sur 4 lignes)
int lcd_actu(unsigned short *adresseTab, char *nameTab, unsigned short *directionTab, unsigned short *vitesseTab){
       
    if(ACTU_ON == 1){
        if(NB_LOCO>NB_LIGNE_LCD){   
            lcd_clear();
        }
        // Transfert les noms des locomotives dans trainTab
        for(int i = 0;i<NB_LOCO_MAX;i++){
            strcpy(trainTab[i],&nameTab[i*NB_CHAR_LOCO_NAME]);
        }
        
        // Affichage du premier écran
        if(LCD_NUMBR == 0){
            for(int p = 0; (p < NB_LIGNE_LCD)&&(p<NB_LOCO) ;p++){
                lcd.locate(0,p); lcd.putc(C_TRAIN); 
                lcd.locate(2,p); lcd.printf("%2u",adresseTab[p]); 
                lcd.locate(5,p); lcd.printf("%-8s",trainTab[p]);
                lcd.locate(15,p); lcd.printf("%c",directionTab[p]);
                if(directionTab[p] == 0){lcd.printf("%c",C_AR);}
                else{lcd.printf("%c",C_AV);}
                lcd.locate(16,p); lcd.printf("%2u",vitesseTab[p]);
            } 
        }
        // Affichage du deuxième écran
        else if (LCD_NUMBR == 1){
            for(int p = NB_LIGNE_LCD; (p < NB_LIGNE_LCD*2)&&(p<NB_LOCO);p++){
                lcd.locate(0,p); lcd.putc(C_TRAIN); 
                lcd.locate(2,p - NB_LIGNE_LCD); lcd.printf("%2u",adresseTab[p]); 
                lcd.locate(5,p - NB_LIGNE_LCD); lcd.printf("%-8s",trainTab[p]);
                lcd.locate(15,p - NB_LIGNE_LCD);
                if(directionTab[p] == 0){lcd.printf("%c",C_AR);}
                else{lcd.printf("%c",C_AV);}
                lcd.locate(16,p - NB_LIGNE_LCD); lcd.printf("%2u",vitesseTab[p]);
            } 
        }
        flags = SWITCH_TO_ACTU_OFF;
    }
    return 0;
}

// Création des caractères graphiques
int lcd_init()
{   
    lcd.createChar(C_AR,C_Ar);
    lcd.createChar(C_AV,C_Av);
    lcd.createChar(C_PONTOK,C_pontOk);
    lcd.createChar(C_PONTNOK,C_pontNotOk);
    lcd.createChar(C_CONNECT,C_ConnectOk);
    lcd.createChar(C_TRAIN,C_train);
    return 0;
}

// Actualisation des affichages des services en cours
int lcd_actu_services(){       
        if(CONNEC_OK == 1){
            lcd.locate(19,1); lcd.putc(C_CONNECT);      // Connexion socket Ok
        }
        else{
            lcd.locate(19,1); lcd.putc('?');            // Pb Connexion socket
        }
        if(H_OK == 1){
            lcd.locate(19,3); lcd.putc(C_PONTOK);       // Pont H validé
        }
        else{
            lcd.locate(19,3); lcd.putc(C_PONTNOK);      // Pont H dévalidé
        }
        return 0;
}