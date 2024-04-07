#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

/**
 * @brief Programa principal
*/
int main(void) {
	// Variable de entrada
	char input[64];
	// Archivo de proc fs
	int f = open("/proc/gpio_driver", O_RDWR);
	// Reviso ques se haya podido abrir
	if(!f) {
		printf("Error intentando comunicarse con el driver\n");
		return -1;
	}

	printf("Para hacer uso del driver escriba: [name] [value]\n");
	printf("Donde name puede ser: red, blue, green, buzzer\n");
	printf("Y value puede ser: on, off\n");
	printf("Use exit para salir\n\n");

	// Bucle del programa
	do {
		printf("prompt: ");
		// Pido el comando
		scanf("%s", input);
		fflush(stdin);
		// Escribo al driver
		write(f, input, strlen(input));

	} while(strcmp(input, "exit"));
	return 0;
}
