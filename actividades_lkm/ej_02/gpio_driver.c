#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/string.h>

#include <asm/io.h>

// GPIO Base address (Physical)
#define GPIO_BASE	0x20200000
// GPIO Pin Output Set 0
#define GPSET0_OFF	0x1c
// GPIO Pin Output Clear 0
#define GPCLR0_OFF	0x28
// Valor para funcion de output
#define GPIO_FUNC_OUTPUT	1

// Maxima cantidad de bytes que puede escribir el usuario
#define MAX_BUFFER_SIZE	64

// GPIOs
#define LED_RED_GPIO	19
#define LED_BLUE_GPIO	26
#define LED_GREEN_GPIO	13
#define BUZZER_GPIO		22

// Puntero a archivo de dispositivo
static struct proc_dir_entry *g_gpio_driver_proc_file = NULL;

// Puntero para direccion de GPIO
static unsigned int *g_gpio_base = NULL;

/**
 * @brief Imprime en user space y kernel el error
 * @param user puntero a buffer de user
 * @param err_msg mensaje de error
*/
static void print_err(char *user, char *err_msg) {
	// Variable local
	char str[64];
	strcpy(str, err_msg);
	copy_to_user(user, str, strlen(str));
	printk(KERN_ERR "%s", str);
}

/**
 * @brief Configura GPIO como salida
 * @param gpio numero de GPIO
*/
static void gpio_set_dir_output(unsigned int gpio) {
	// Punteros de registros
	unsigned int *gpio_fsel;
	// Variables para offsets
	unsigned int fsel_reg_index, fsel_bit_index;
	// Encuentro el indice del FSEL (se agrupan de a diez por registro)
	fsel_reg_index = gpio / 10;
	// Encuentro el offset para el bit del GPIO (son tres bits por GPIO)
	fsel_bit_index = 3 * (gpio % 10);
	// Apunto al FSEL que corresponda
	gpio_fsel = g_gpio_base + fsel_reg_index;
	// Limpio cualquier funcion previa
	*gpio_fsel &= ~(7 << fsel_bit_index);
	// Asigno la funcion de output
	*gpio_fsel |= GPIO_FUNC_OUTPUT << fsel_bit_index;
}

/**
 * @brief Apaga un GPIO
 * @param gpio numero de GPIO
*/
static void gpio_off(unsigned int gpio) {
	// Puntero a registro
	unsigned int *gpio_clr;
	// Configuro como salida
	gpio_set_dir_output(gpio);
	// Obtengo la direccion del registro de clear
	gpio_clr = (unsigned int*) ((char*)g_gpio_base + GPCLR0_OFF);
	// Apago el GPIO
	*gpio_clr |= 1 << gpio;
}

/**
 * @brief Prende un GPIO
 * @param gpio numero de GPIO
*/
static void gpio_on(unsigned int gpio) {
	// Puntero a registro
	unsigned int *gpio_set;
	// Configuro como salida
	gpio_set_dir_output(gpio);
	// Obtengo la direccion del registro de set
	gpio_set = (unsigned int*) ((char*)g_gpio_base + GPSET0_OFF);
	// Prendo el GPIO
	*gpio_set |= 1 << gpio;
}

/**
 * @brief Traduce un nombre a numero de GPIO
 * @param name string con nombre
 * @return numero de GPIO
*/
static int gpio_name_to_number(char *name) {
	// Posibles nombres
	char names[][8] = { "red", "blue", "green", "buzzer" };
	// GPIOs asociados a los nombres
	unsigned int gpios[] = { LED_RED_GPIO, LED_BLUE_GPIO, LED_GREEN_GPIO, BUZZER_GPIO };
	// Variable de contador
	int i;

	// Recorro todos los nombres posibles
	for(i = 0; i < sizeof(names); i++) {
		// Pregunto
		if(!strcmp(names[i], name)) {
			// Si son iguales, devuelvo el numero de GPIO
			return gpios[i];
		}
	}
	// Nombre invalido
	return -1;
}

/**
 * @brief Atiende la lectura en el proc_fs
 * @param f puntero al archivo
 * @param user puntero a buffer del user para devolver
 * @param size cantidad de bytes a leer
*/
static ssize_t gpio_driver_read(struct file *f, char __user *user, size_t size, loff_t *off) {
	// No implemento la lectura
	return 0;
}

/**
 * @brief Atiende la escritura en el proc_fs
 * @param f puntero al archivo
 * @param user puntero a buffer del user que quiso escribir
 * @param size cantidad de bytes a escribir
 * @return cantidad de bytes escritos
*/
static ssize_t gpio_driver_write(struct file *f, const char __user *user, size_t size, loff_t *off) {
	// Buffer local
	char local_buff[MAX_BUFFER_SIZE];
	// Variable para evaluar comando
	char gpio_name[8], gpio_value[4];
	// Variable para numero de GPIO
	int gpio_number;

	// Verifico que no hayan querido escribir de mas
	if(size > MAX_BUFFER_SIZE) {
		// Piso con el maximo
		size = MAX_BUFFER_SIZE;
	}

	// Leo del usuario
	if(copy_from_user(local_buff, user, size)) {
		// No habia nada para escribir
		return 0;
	}
	// Obtengo los valores por separado
	if(sscanf(local_buff, "%s %s", gpio_name, gpio_value) != 2) {
		// Salio mal
		print_err(user, "gpio_driver: Formato incorrecto\n");
		return -1;
	}

	// Obtengo el numero de GPIO
	gpio_number = gpio_name_to_number(gpio_name);
	// Verifico el resultado
	if(gpio_number < 0) {
		// Salio mal
		print_err(user, "gpio_driver: Nombre de GPIO invalido\n");
		return -1;
	}

	// Verifico el estado para el GPIO
	if(!strcmp(gpio_value, "on")) {
		// Prendo el GPIO
		gpio_on(gpio_number);
	}
	else if(!strcmp(gpio_value, "off")) {
		// Apago el GPIO
		gpio_off(gpio_number);
	}
	else {
		// Salio mal
		print_err(user, "gpio_driver: Valor de GPIO invalido\n");
		return -1;
	}

	// Leo del usuario lo que escribio
	return size;
}

// Estructura de operaciones para archivos
static struct proc_ops g_proc_ops = {
	.proc_read = gpio_driver_read,
	.proc_write = gpio_driver_write
};

/**
 * @brief Inicializa el GPIO
 * @return codigo de error
*/
static int __init gpio_driver_init(void) {
	// Intenta mapear la memoria virtual para el GPIO
	g_gpio_base = (unsigned int*)ioremap(GPIO_BASE, PAGE_SIZE);
	// Reviso si fue posible
	if(g_gpio_base == NULL) {
		// Informo error
		printk(KERN_ERR "gpio_driver: No se pudo mapear la memoria fisica del GPIO\n");
		return -1;
	}

	// Creo el proc file
	g_gpio_driver_proc_file = proc_create("gpio_driver", 0666, NULL, &g_proc_ops);
	// Verifico que se haya creado
	if(g_gpio_driver_proc_file == NULL) {
		// Error
		printk(KERN_ERR "gpio_driver: No se pudo crear el proc file\n");
		return -ENOMEM;
	}

	// Todo salio bien
	printk(KERN_INFO "gpio_driver: GPIO driver inicializado con exito!\n");
	return 0;
}

/**
 * @brief Libera el GPIO cuando se quita el  modulo del kernel
*/
static void __exit gpio_driver_exit(void) {
	// Libero los recursos
	iounmap(g_gpio_base);
	proc_remove(g_gpio_driver_proc_file);
	return;
}

// Registro las funciones llamadas al insertar y remover el modulo del kernel
module_init(gpio_driver_init);
module_exit(gpio_driver_exit);

// Metadata
MODULE_LICENSE("GPL");
MODULE_AUTHOR("carlassaraf");
MODULE_DESCRIPTION("Driver para controlar el GPIO de la RPiZeroW");
MODULE_VERSION("1.0");
