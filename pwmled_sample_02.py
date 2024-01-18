from gpiozero import PWMLED
from time import sleep
from random import randint

# Mensaje de bienvenida
print("Testeo de LED RGB. Asigna color al azar.. Ctrl + C para terminar")
print()

try:

	# Inicializacion del RGB
	ledr = PWMLED(pin = 19, initial_value = 0)
	ledg = PWMLED(pin = 13, initial_value = 0)
	ledb = PWMLED(pin = 26, initial_value = 0)

	while True:
		# Asigno valor aleatorio para LED Rojo
		ledr.value = randint(0, 100) / 100
		# Asigno valor aleatorio para LED Verde
		ledg.value = randint(0, 100) / 100
		# Asigno valor aleatorio para LED Azul
		ledb.value = randint(0, 100) / 100
		# Demora
		sleep(.5)

except KeyboardInterrupt:
	# Termino el programa
	print()
	print("Programa finalizado")
