#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/usb.h>
#include <linux/proc_fs.h>

#include "inc/usb_descriptor.h"

// Puntero global para dispositivo USB
static struct usb_device *g_usb_dev;

// Puntero a archivo de dispositivo
static struct proc_dir_entry *g_proc_file = NULL;

/**
 * @brief Atiende la lectura por el USB
*/
static ssize_t usb_dev_read(struct file *f, char __user *buf, size_t cnt, loff_t *off) {
	// Buffer para leer
	unsigned char bulk_buffer[USB_BULK_BUFFER_LEN_IN] = {0};
	// Variable de estado
	int status;
	// Cuantos bytes se recibieron
	int rcv_bytes;

	// Mensaje de aviso
	printk("[USB DRIVER] Se recibio algo por USB\n");

	// Leo por USB
	status = usb_bulk_msg(
		g_usb_dev,
		usb_rcvbulkpipe(g_usb_dev, USB_BULK_EP_IN_ADDR),
		bulk_buffer,
		sizeof(bulk_buffer),
		&rcv_bytes,
		USB_BULK_TIMEOUT
	);
	// Verifico el estado de la operacion
	if(status) {
		// No se pudo
		printk("[USB DRIVER] Algo salio mal al recibir por USB\n");
		return -1;
	}
	// Salio todo bien
	printk("[USB DRIVER] Llegaron %d bytes por dispositivo USB. El mensaje fue %s\n", rcv_bytes, (char*)bulk_buffer);
	return rcv_bytes;
}

/**
 * @brief Atiende la escritura en el USB
*/
static ssize_t usb_dev_write(struct file *f, const char __user *buf, size_t cnt, loff_t *off) {
	//
	printk("[USB DRIVER] Se escribio data al dispositivo USB\n");
	return cnt;
}

// Estructura de operaciones para archivos
static struct proc_ops g_fops = {
	.proc_read = usb_dev_read,
	.proc_write = usb_dev_write
};

/**
 * @brief Atiende cada posibilidad de inicializar una interfaz USB
 * @param intf puntero a interfaz USB
 * @param usb_device_id puntero al ID del dispositivo
 * @return cero si inicializa el dispositivo, negativo si
 * decide no hacerlo
*/
static int usb_dev_probe(struct usb_interface *intf, const struct usb_device_id *id) {
	// Consigo el dispositivo
	g_usb_dev = interface_to_usbdev(intf);
	// Verifico que se haya podido obtener
	if(g_usb_dev == NULL) {
		// Mensaje de error
		printk("[USB DRIVER] Error al obtener el dispositivo de la interfaz\n");
		return -ENODEV;
	}

	// Creo un archivo para el dispositivo sin permisos de ejecucion
//	g_proc_file = proc_create("myUSBDev", 0666, NULL, &g_fops);
	// Verifico el resultado
//	if(g_proc_file == NULL) {
		// No se pudo crear
//		printk("[USB DRIVER] Error creando /proc/myUSBDev\n");
//		return -ENOMEM;
//	}
	// Todo salio bien
//	printk("[USB DRIVER] Archivo /proc/myUSBDev creado con exito\n");
	printk("[USB DRIVER] Conexion establecida con dispositivo %04x:%04x\n", USB_DEV_VID, USB_DEV_PID);
	return 0;
}

/**
 * @brief Atiende la desconeccion del dispositivo USB
 * o cuando se quita el driver del kernel
 * @param intf puntero a interfaz USB que
 * fue removida del sistema
*/
static void usb_dev_disconnect(struct usb_interface *intf) {
	printk("[USB DRIVER] El dispositivo fue desconectado\n");
	// Elimino el archivo para operaciones
//	proc_remove(g_proc_file);
	return;
}

// Tabla de dispositivos USB para el driver
static struct usb_device_id g_usb_dev_table[] = {
	{ USB_DEVICE(USB_DEV_VID, USB_DEV_PID) },
	{ },
};
MODULE_DEVICE_TABLE(usb, g_usb_dev_table);

// Informacion del driver USB
static struct usb_driver g_usb_dev_driver = {
	.name = "usb_dev_driver",
	.id_table = g_usb_dev_table,
	.probe = usb_dev_probe,
	.disconnect = usb_dev_disconnect,
};

/**
 * @brief Registra un dispositivo USB
 * @return codigo de error
*/
static int __init usb_driver_init(void) {

	int result;
	// Mensaje de inicializacion
	printk("[USB DRIVER] Intentando registrar el driver\n");
	// Registra el driver en el subsistema USB
	result = usb_register(&g_usb_dev_driver);
	// Reviso si fue posible
	if(result) {
		// Informo error
		printk("[USB DRIVER] Fallo el registro del driver con error %d\n", result);
		return -result;
	}
	// Todo salio bien
	printk("[USB DRIVER] Driver USB registrado con exito!\n");
	return result;
}

/**
 * @brief Anula el registro del driver USB
 * cuando se quita el modulo del kernel
*/
static void __exit usb_driver_exit(void) {
	// Elimino archivo de para operaciones
//	proc_remove(g_proc_file);
	// Anulo el registro del driver
	usb_deregister(&g_usb_dev_driver);
	// Mensaje de salida
	printk("[USB DRIVER] Driver USB fue removido con exito!\n");
	return;
}

// Registro las funciones llamadas al insertar y remover el modulo del kernel
module_init(usb_driver_init);
module_exit(usb_driver_exit);

// Metadata
MODULE_LICENSE("GPL");
MODULE_AUTHOR("carlassaraf");
MODULE_DESCRIPTION("Driver para eco por USB");
MODULE_VERSION("1.0");
