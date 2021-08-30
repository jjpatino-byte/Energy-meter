from wiringpi import Serial
import RPi.GPIO as GPIO
from time import*
import random
import requests
import json

KEY="X14QLOVGHHI9HOUS"#Poner aqui su Key de escritura
GPIO.setmode(GPIO.BCM) #puede cambiar a BOARD
baud = 9600
ser  = Serial("/dev/serial0",baud)
 
def recibir(echo = True):
    data = ""
    while True:
     input = ser.getchar()
     if echo:
      ser.putchar(input)
     if input == "\r":
      return (data)
     data += input
 #sleep(0.2)
 
def printsln(menss):
 ser.puts(menss+"\r")
 

def enviar_tp(): 
  #Escritura de datos a Thingsboard (hacerlo durante 90 segundos)
  global enviar
  enviar=recibir(); 
  print(enviar)
  value_1 = enviar[4:].split(",")[0]
  value_2 = enviar[4:].split(",")[1]
  lista = [value_1,value_2] 
  if len(lista) == 2:
   enviar = requests.get("https://api.thingspeak.com/update?api_key="+KEY+"&field1="+str(lista[0])+"&field2="+str(lista[1]))  #cuando se quiere enviar dos o mas datos
   #enviar = requests.get("https://api.thingspeak.com/update?api_key=B3ZRHNV2DUMV48XB&field1="+str(lista[0]))
   if enviar.status_code == requests.codes.ok:
     if enviar.text != '0':
      print("Datos enviados correctamente")
     else:
      print("Tiempo de espera insuficiente (>15seg)")
   else:
     print("Error en el request: ",enviar.status_code)
	 #else:
	 #print("La cadena recibida no contiene 2 elementos, sino:",len(lista),"elementos")
  sleep(15)
	
# Main function
def main():
# Infinite loop
  while True :
  #printsln("Recibiendo datos..  ")
  printsln(recibir())
   if recibir()[0]="$":
   
   enviar_tp()

  
  #sleep(0.4)
# Command line execution
if __name__ == '__main__' :
 main()
