from gpiozero import PWMLED
from time import sleep

# Mensaje de bienvenida
print("Testeo de LED RGB. Varia cada color individualmente. Ctrl + C para terminar")
print()

try:

	# Inicializacion del RGB
	ledr = PWMLED(pin = 19, initial_value = 0)
	ledg = PWMLED(pin = 13, initial_value = 0)
	ledb = PWMLED(pin = 26, initial_value = 0)

	while True:
		# LED Rojo 0 a 100 en saltos del 10%
		print("LED Rojo 0~100%")
		for i in range(11):
			ledr.value = i / 10
			sleep(.2)
		# Apago LED Rojo
		ledr.value = 0

		# LED Verde 0 a 100 en saltos del 10%
		print("LED Verde 0~100%")
		for i in range(11):
			ledg.value = i / 10
			sleep(.2)
		# Apago LED Verde
		ledg.value = 0

		# LED Azul 0 a 100 en saltos del 10%
		print("LED Azul 0~100%")
		for i in range(11):
			ledb.value = i / 10
			sleep(.2)
		# Apago LED Azul
		ledb.value = 0

except KeyboardInterrupt:
	# Termino el programa
	print()
	print("Programa finalizado")
