/* SCHEMATIC:                                                            */
/* 3 Push button connected to PC0, PC1 and PC2                          */
/* Build in PA5 LED (hold GREEN LED), and External PA6 (hold BLUE LED) */


#include "stm32f4xx.h"


void GPIOAGPIOC_init(void);
void USART2_init(void);
void USART2_write(int ch);
void delayMs(int);
const byte bufsiz = 32;

char buf[bufsiz]; /* allocate space of 3 char in memory */

/*----------------------------------------------------------------------------------------------------------------------------
  MAIN FUNCTION
------------------------------------------------------------------------------------------------------------------------------*/
int main(void) 
{
 
  GPIOAGPIOC_init();
  USART2_init(); /* Initialize USART2 */

  char message [] = "you have less then 30s to enter the password:\r\n";
  int i; 
  for (i = 0; i < 45; i++)
  {
    USART2_write(message[i]);
  }
  delayMs(10); 

  clicked();
  
}



void clicked (void)
{
int timer = 0;
int firstbuttontime = 0;        /* used to capture the timer value once button1 is pressed*/
int secondbuttontime = 0;      /* used to capture the timer value once button2 is pressed*/
int thirdbuttontime = 0;      /* used to capture the timer value once button3 is pressed*/

bool buttonstate1 = false;   /* used to capture the state of button1 once button1 is pressed*/
bool buttonstate2 = false;  /* used to capture the state of button2 once button2 is pressed*/
bool buttonstate3 = false; /* used to capture the state of button3 once button3 is pressed*/
int i;

for (i=0 ; i <10000000; i++) /* you have less then 30 second to enter the password */
{
 timer++;
 if (GPIOC->IDR & 0x1)       /*if PC0 is HIGH */
     {           
     GPIOA->BSRR = 0x00000020;  /*turn on GREEN LED */
     buttonstate1 = true; 
     firstbuttontime = timer;
     GPIOA->BSRR = 0x00200000;  /*turn off GREEN LED */   
     }

 else if (GPIOC->IDR & 0x2)       /*if PC1 is HIGH */
     {
     GPIOA->BSRR = 0x00000020;  /*turn on GREEN LED */
     buttonstate2 = true; 
     secondbuttontime = timer;    
     GPIOA->BSRR = 0x00200000;  /*turn off GREEN LED */
     }  

 else if (GPIOC->IDR & 0x4)          /*if PC2 is HIGH */
     {
     GPIOA->BSRR = 0x00000020;  /*turn on GREEN LED */
     buttonstate3 = true; 
     thirdbuttontime = timer;    
     GPIOA->BSRR = 0x00200000;  /*turn off GREEN LED */
     } 
  
 if (buttonstate1 == true && buttonstate2 == true && buttonstate3 == true)
     {
    //buttonstate1 = false;
    // buttonstate2 = false;
     //buttonstate3 = false;
     pressarrangement (firstbuttontime, secondbuttontime, thirdbuttontime);
     break;
      }
}


}


void GPIOAGPIOC_init(void) 
{

  /* PC0 (hold number 1), PC1 (hold number 2), PC2 (hold number 3) and PC3 (hold number 4) used for user password input */
  /* We activated PC3 maybe you need to use 4 button instead of 3 to enter the password*/
  /* PA5 (hold GREEN LED), PA6 (hold BLUE LED) */
  RCC->AHB1ENR |= 4;                /*enable GPIOC clock */
  RCC->AHB1ENR |= 1;               /*enable GPIOA clock */
  GPIOA->MODER &= ~0x00003C00;    /* clear pin mode on PA5 and PA6 */
  GPIOA->MODER |=  0x00001400;  /* set ouput mode on PA5 and PA6 */
  GPIOC->MODER &= ~0x000000FF;  /* clear pin mode on PC0, PC1, PC2 and PC3 */
  GPIOC->MODER |=  0x00000000;   /* set input mode on PC0, PC1, PC2 and PC3 */
  GPIOC->PUPDR &= ~0x000000FF;  /* clear pull-up and pull-down bits for PC0, PC1, PC2 and PC3 */
  GPIOC->PUPDR |=  0x000000AA;  /* set pull down bits for PC0, PC1, PC2 and PC3  */
}


void USART2_init(void)
{
  RCC->AHB1ENR |= 1;               /*enable GPIOA clock */
  RCC->APB1ENR |= 0x20000;        /*Enable USART2 clock */ 
  
  /* configure PA2  for UART2_TX 8*/
  GPIOA->AFR[0] &= ~0x0F00;
  GPIOA->AFR[0]  |= 0x0700;      /* alt7 for USART2 */
  GPIOA->MODER  &= ~0x0030;
  
  GPIOA->MODER  |= 0x0020;     /* enable alternate function for PA2 */ 
  USART2->BRR = 0x0683;     /* 9600 baud rate 16 MHz */
  USART2->CR1 = 0x0008;    /* enable TX, 8-bit data */
  USART2->CR2 = 0x0000;   /* 1 stop bit */
  USART2->CR3 = 0x0000;  /*no flow control */
  USART2->CR1 |= 0x2000; /* enable USART2 */
}


void USART2_write (int ch)
{
	while (!(USART2->SR & 0x0080)) {} /*wait until Tx buffer empty*/
	USART2->DR = (ch & 0xFF);
}

/* 16 Mhz Frequency */
void delayMs (int n) {
	int i;
	for (; n > 0; n--)
	    for (i = 0; i < 3195; i++);
}

void pressarrangement (int firstbuttontime, int secondbuttontime, int thirdbuttontime)
{
int x= firstbuttontime;
int y = secondbuttontime;
int z = thirdbuttontime;

/* First combination of entered password could be: 123 or 132 or 111 . Note 111 wasn't be taking into consideration in my algorithmic way in the Function Click (need improvement)*/
if (x < y && y < z)
{
  buf[0]='1';
  buf[1]='2';
  buf[2]='3';
 
}

if (x < z && y > z)
{
  buf[0]='1';
  buf[1]='3';
  buf[2]='2';
  
}

/* Second combination of entered password could be: 213 or 231 or 222. Note 222 wasn't be taking into consideration in my algorithmic way in the Function Click (need improvement) */
if (y < x && x < z)
{
  buf[0]='2';
  buf[1]='1';
  buf[2]='3';
  
}

if (y < z &&  z < x)
{
  buf[0]='2';
  buf[1]='3';
  buf[2]='1';
  
}

/* Third combination of entered password could be: 312 or 321 or 333. Note 333 wasn't be taking into consideration in my algorithmic way in the Function Click (need improvement) */
if (z < x &&  z < y)
{
  buf[0]='3';
  buf[1]='1';
  buf[2]='2';
  
}

if (z < y &&  y < x)
{
  buf[0]='3';
  buf[1]='2';
  buf[2]='1';
  
}

validate();

}

void validate(){
 
  if (strcmp(buf, "213") == 0)
  { 
  GPIOA->BSRR = 0x00000040;

  char message [] = "Password Correct \r\n";
  int i; 
  for (i = 0; i < 18; i++)
  {
    USART2_write(message[i]);  
  }
  
  delayMs(20); 
  GPIOA->BSRR = 0x00400000;
  delayMs(1000); 
  }

  else 
  {
  char message [] = "Password Incorrect \r\n";
  int i; 
  for (i = 0; i < 20; i++)
  {
    USART2_write(message[i]);  
  }
  
  delayMs(20); 
  GPIOA->BSRR = 0x00400000;
  buf[0] = 0;
  buf[1] = 0; 
  buf[2] = 0;
  }

}


