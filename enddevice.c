#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stdlib.h>
#define MAX_STR 50
#define BAUD 9600
#include <math.h>   
#include <string.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
volatile unsigned char rx_buffer[MAX_STR] = {0};
volatile unsigned char current_size = 0;
bool isReady = false;
char buffer[sizeof(int)*8+1];
char buffer2[sizeof(int)*8+1];

long long sumatoria;
int n=0;
float curr_2;
ISR(USART_RX_vect){
   unsigned char ch = UDR0;
   if( ch == '\r' || ch == '\n'){
     rx_buffer[current_size] = 0;
     isReady = true;
   }
   else if( ch == '\b' && current_size>0){
     rx_buffer[--current_size] = 0;
   }
   else{
     rx_buffer[current_size++] = ch;
   }
}

/******************************************************************************************************************************/
/*                                                               Funciones públicas de la libreria						*/
/******************************************************************************************************************************/
/////////////////////////////////////////////
//inicialización del módulo USART AVR modo asíncrono
//en una función, a 8bits,a 9600 baudios, sin bit de paridad
//1 bit de parada
/////////////////////////////////////////////////////
void serial_begin(){
	cli();
	float valor_UBBR0 = 0;
	UCSR0A=0b00000010;	//el bit 1 (U2X0) se pone en uno para duplicar la velocidad y poder utilizar frecuencias desde 1MHz
	UCSR0B=0b10011000;	//habilitar interrupcion por recepcion / transmisiÃ³n y recepciÃ³n habilitados a 8 bits
	UCSR0C=0b00000110;	//asÃ­ncrono, sin bit de paridad, 1 bit de parada a 8 bits
	valor_UBBR0 = F_CPU/(16.0*BAUD);	//Definir la constante BAUD al inicio del cÃ³digo
        if(UCSR0A & (1<<U2X0)) valor_UBBR0 *= 2;
	UBRR0=valor_UBBR0 - 1;
        sei();
}
///////////////////////////////////////////////
//recepción de datos del módulo USART AVR modo asíncrono
////////////////////////////////////////////////////////////
unsigned char serial_read_char(){
	if(UCSR0A&(1<<7)){  //si el bit7 del registro UCSR0A se ha puesto a 1
		return UDR0;    //devuelve el dato almacenado en el registro UDR0
	}
	else//sino
	return 0;//retorna 0
}
///////////////////////////////////////////////
//transmisión de datos del módulo USART AVR modo asíncrono
///////////////////////////////////////////////
void serial_print_char(unsigned char caracter){
        if(caracter==0) return;
	while(!(UCSR0A&(1<<5)));    // mientras el registro UDR0 esté lleno espera
	UDR0 = caracter;            //cuando el el registro UDR0 está vacio se envia el //caracter
}
//////////////////////////////////////////////
//transmisión de cadenas de caracteres con el módulo USART AVR modo asíncrono
///////////////////////////////////////////////
void serial_print_str(char *cadena){	//cadena de caracteres de tipo char
	while(*cadena !=0x00){			//mientras el último valor de la cadena sea diferente
							        //al caracter nulo
		serial_print_char(*cadena);	//transmite los caracteres de cadena
		cadena++;				//incrementa la ubicación de los caracteres en cadena
								//para enviar el siguiente caracter de cadena
	}
}

void serial_println_str(char *cadena){
	serial_print_str(cadena);
	serial_print_char('\r');
	serial_print_char('\n');
}

char* get_RX_buffer(){
   current_size = 0;
   isReady = false;
   return (char*)rx_buffer;
}

bool is_data_ready(){
   return isReady;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//FUNCIONES
int curr_sens;
void interrupt_INT0_Init(){
 cli(); //DESH INT
 EICRA = 0b11; // INT FLANCO DE SUBIDA
 EIMSK = 1; // HAB PIN0
 sei(); //HAB INT
}
void interrupt_TIMER0_Init(){
 cli();
 TCCR0B = 0b101; //PREESCALADOR 1024
 TIMSK0 = 1; //HB TIMER POR DESBORDAMIENTO
 sei();
 TCNT0 = 12; //250ms
}
void ADC_Init(){
 ADMUX = 0b110; //ADC6 (ADC6 Y ADC7 son solo anal�gicos entonces no es necesario configurar el reg DIDR0)
 ADCSRA = 0b10101011; //ADC activado modo autom�tico y preescalador 8
 //ADCSRB= 0b10; //Modo interrupci�n externa //ADIE(3) en 1 para habilitar la interrupci�n por la terminaci�n de una conversi�n anal�gica.
 ADCSRB= 0b100; //Modo Timer0 (250ms)
}
//INTERRUPCIONES
ISR(TIMER0_OVF_vect){
 TCNT0 = 12;
   
}
ISR(INT0_vect){
 //EIFR &= ~(1<<INTF0);
}
ISR(ADC_vect){ //ENCERAR EL BIT 4 DEL REG ADCSRA &=~(1<<ADIF)
   
   PORTB =ADCL;
   PORTC = ADCH;  
  double curr=(((ADC/(1023/5.0))-2.5)/35.35)*2000;
   itoa(curr, buffer,  10);
   curr_2=pow(curr,2);
   sumatoria=sumatoria+curr_2; 

  
   n++;
   if (n==117){
      float i_rms=sqrt(sumatoria/n);
      itoa(i_rms, buffer,  10);
      //serial_print_str("Irms: ");
      //serial_println_str(buffer);
      itoa(i_rms*220*0.92, buffer2,  10);
      //serial_print_str("Power: ");
      strcat(buffer,",");
      strcat(buffer,buffer2);
      serial_println_str(buffer);
      //serial_println_str("W");
      sumatoria=0;
      n=0;
     
      }
   }
//MAIN
int main()
{
   DDRB = 0XFF; //PuertoB salida
 DDRC = 0b11; //Pines0y 1 del puerto C como salida
 PORTD |= (1<<PD2); //Activar resistencia de PULLUP en PIN0 del puerto D
 ADC_Init();
 interrupt_INT0_Init();
 interrupt_TIMER0_Init(); //250ms de retardo
 serial_begin();
 while (1){
 }
 return 0;
}