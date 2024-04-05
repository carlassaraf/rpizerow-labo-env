#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>

/**
 * @brief Llamada al conectar el modulo
 * @return devuelve codigo de error
*/
static int __init hello_kernel_init(void) {
	// Mensaje de Kernel
	printk("Hola Kernel!\n");
	// Retorno cero para indicar que todo salio bien
	return 0;
}

/**
 * @brief Llamada cuando se desconecta el modulo
*/
static void __exit hello_kernel_exit(void) {
	// Mensaje de Kernel
	printk("Chau Kernel!\n");
	return;
}

// Indico que funcion llamar al inicializar el modulo
module_init(hello_kernel_init);
module_exit(hello_kernel_exit);

// Metadata
MODULE_LICENSE("GPL");
MODULE_AUTHOR("carlassaraf");
MODULE_DESCRIPTION("Hola mundo desde el Kernel");
MODULE_VERSION("1.0");
