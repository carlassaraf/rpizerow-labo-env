from gpiozero import Buzzer, Button
from time import sleep

# Mensaje de bienvenida
print("Hace sonar el buzzer cuando se aprieta el pulsador. Ctrl + C para terminar")
print()

try:

	# Inicializo buzzer
	buzzer = Buzzer(22)
	# Inicializo pulsador
	button = Button(18)

	while True:
		# Verifico si esta presionado
		if button.is_pressed:
			# Enciendo el buzzer
			buzzer.on()
			print("Pulsador apretado!")
			sleep(.2)

		else:
			# Apago buzzer
			buzzer.off()

except KeyboardInterrupt:
	# Termino el programa
	print()
	print("Programa finalizado")
