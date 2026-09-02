/*
 * Modulo de kernel Mini meminfo.
 * Muestra un resumen de la memoria total y libre del sistema.
 * Nota: Esta es mi primeza vez usando C. Asi que voy a documentar todo lo posible
 * para familiarizarme en el futuro
*/

#include <linux/init.h> // Macros para funciones de inicialización y salida
#include <linux/kernel.h> // Funciones y definiciones generales del kernel
#include <linux/module.h> // Crea módulos cargables del kernel.
#include <linux/proc_fs.h> // Permite crear y administrar entradas en /proc
#include <linux/seq_file.h> //Facilita la generación de archivos virtuales
#include <linux/mm.h> // Contiene funciones relacionadas con la memoria
#include <linux/uaccess.h> // Son funciones para transferir datos entre kernel y el usuario

// Nombre de la entrada que será creada dentro de /proc
#define PROC_NAME "mem_simple"

/*
 * Funcion que genera el mensaje que se mostrara al leer /proc/mem_simple.
 * Los  parametros que recibe son "m", como estructura seq_file utilizada para escribir la salida
 * y  "v", como puntero utilizado por la interfaz seq_file
*/
static int mem_simple_show(struct seq_file *m, void *v)
{
    // Estructura proporcionada por el kernel que almacena información relacionada con la memoria del sistema.
    struct sysinfo info;
    unsigned long total_kb, free_kb, used_kb;

    // Obtiene la información actual de la memoria del sistema y la almacena dentro de info.
    si_meminfo(&info); 

    /*
     * Conversión de páginas de memoria a kilobytes. PAGE_SHIFT representa el 
     * tamaño de página mediante una potencia de 2.
    */
    total_kb = info.totalram << (PAGE_SHIFT - 10);
    free_kb  = info.freeram  << (PAGE_SHIFT - 10);
    used_kb  = total_kb - free_kb;

    seq_printf(m, "Memoria Total: %8lu kB (%lu MB)\n", total_kb, total_kb / 1024);
    seq_printf(m, "Memoria Libre:  %8lu kB (%lu MB)\n", free_kb,  free_kb  / 1024);
    seq_printf(m, "Memoria Usada:  %8lu kB (%lu MB)\n", used_kb,  used_kb  / 1024);

    return 0;
}

/*
 * Esta función se ejecuta cuando un proceso abre /proc/mem_simple
 * Los  parametros que recibe son "inode", como información del archivo dentro del sistema de archivos
 * y  "file", como estructura que representa el archivo abierto
*/
static int mem_simple_open(struct inode *inode, struct file *file)
{
    return single_open(file, mem_simple_show, NULL);
}

static const struct proc_ops mem_simple_fops = {
    .proc_open    = mem_simple_open,
    .proc_read    = seq_read,
    .proc_lseek   = seq_lseek,
    .proc_release = single_release,
};

/* 
 * Función que inicializa el módulo. Esta función se ejecuta
 * automáticamente por el kernel cuando el módulo
 * es cargado mediante insmod mem_simple.ko
*/
static int __init mem_simple_init(void)
{
    proc_create(PROC_NAME, 0444, NULL, &mem_simple_fops);
    printk(KERN_INFO "mem_simple: modulo cargado, creada /proc/%s\n", PROC_NAME);
    return 0;
}

// Funció que sirve para eliminar la entrada creada previamente dentro de /proc.
static void __exit mem_simple_exit(void)
{
    remove_proc_entry(PROC_NAME, NULL);
    printk(KERN_INFO "mem_simple: modulo descargado, eliminada /proc/%s\n", PROC_NAME);
}

module_init(mem_simple_init);
module_exit(mem_simple_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Mini meminfo: muestra memoria total y libre en /proc/mem_simple");
MODULE_AUTHOR("Juanpi");
