// Vendor ID y Product ID para el USB
#define USB_DEV_VID	0x2e8a
#define USB_DEV_PID	0x000a

// Datos del endpoint

// Cantidad maxima de bytes para transmitir
#define USB_BULK_BUFFER_LEN_OUT	64
#define USB_BULK_BUFFER_LEN_IN	64

// Direcciones de los endpoints para transmision tipo bulk
#define USB_BULK_EP_OUT_ADDR	0x02
#define USB_BULK_EP_IN_ADDR		0x82

// Timeouts para la transferencia
#define USB_BULK_TIMEOUT	100
