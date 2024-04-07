#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>

/**
 * @brief Envia un mensaje cuando se inserta 
 * en el kernel
 * @return codigo de error
*/
static int __init hello_kernel_init(void) {
	// Mensaje de kernel
	printk("Hola Kernel!\n");
	// Todo salio bien
	return 0;
}

/**
 * @brief Envia un mensaje cuando se remueve
 * del kernel
*/
static void __exit hello_kernel_exit(void) {
	// Mensaje de kernel
	printk("Chau Kernel!\n");
	return;
}

// Registro las funciones llamadas al insertar y remover el modulo del kernel
module_init(hello_kernel_init);
module_exit(hello_kernel_exit);

// Metadata
MODULE_LICENSE("GPL");
MODULE_AUTHOR("carlassaraf");
MODULE_DESCRIPTION("Hola mundo version kernel");
MODULE_VERSION("1.0");


