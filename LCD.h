#ifndef LCD_H_INCLUDED
#define LCD_H_INCLUDED

int lcd_init();
int lcd_clear();
int lcd_actu(unsigned short *adresseTab,char *nameTab, unsigned short *directionTab,unsigned short *vitesseTab);
int lcd_printf(int column,int row,char *message);
int lcd_actu_services();

#endif // LCD_H_INCLUDED