//-----------------------------------------------------------------------------
//   _____ _                  _____  _                  ______ _      ______ ______ ___  _  _    
//  / ____| |                |  __ \(_)                |  ____| |    |  ____|____  / _ \| || |   
// | |    | |__   __ _ _ __  | |  | |___   _____ _ __  | |__  | |    | |__      / / (_) | || |_  
// | |    | '_ \ / _` | '__| | |  | | \ \ / / _ \ '__| |  __| | |    |  __|    / / > _ <|__   _| 
// | |____| | | | (_| | |    | |__| | |\ V /  __/ |    | |____| |____| |____  / / | (_) |  | |   
//  \_____|_| |_|\__,_|_|    |_____/|_| \_/ \___|_|    |______|______|______|/_/   \___/   |_|   
//                                                                                              
// Engineer      : Marc-André Lafaille Magnan
//
// Create Date   : 8:58:00 29/09/2014
// Design Name   :
// Module Name   :
// Project Name  :
// Target Devices: x86 code
// Tool versions :
// Description   :
//
//
// Revision:
// Revision 0.01 - File Created based on Bruno De Keplar example
// Additional Comments:
//
//-----------------------------------------------------------------------------


// Table of content -----------------------------------------------------------
//  Inclusion           [Incl]
//  Define              [Def]
//  Licence             [Lic]
//  Function            [Func]
//  Struct              [Strc]
//  Global Variable     [GVar]
//  Function definition [Fdef]


// inclusion [Incl] -----------------------------------------------------------
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/fcntl.h>
#include <linux/spinlock.h>
#include <linux/wait.h>
#include <asm/atomic.h>
#include <asm/uaccess.h>

// Define [Def] ---------------------------------------------------------------
#define READWRITE_BUFSIZE   (16U)
#define DEFAULT_BUFSIZE     (256U)
#define DRIVER_NAME         ("Buf_driv - ")
// Licence [Lic] --------------------------------------------------------------
MODULE_AUTHOR  ("M.-A. Lf. Magnan");
MODULE_LICENSE ("Dual BSD/GPL");

// Function [Func] ------------------------------------------------------------

// Buf_Init     : Inits Buffer and BDev structure
// Buf_Exit     : Deletion of all structure (Buffer, BDev)
// Buf_Open     : Reserve ressource with (R,W, R/W)
// Buf_Release  : Release ressource
// Buf_Read     :
// Buf_Write    :
// Buf_Ioctl    :

int     Buf_Init    (void);
void    Buf_Exit    (void);
int     Buf_Open    (struct inode *inode, struct file *flip);
int     Buf_Release (struct inode *inode, struct file *flip);
ssize_t Buf_Read    (struct file *flip,       char __user *ubuf, size_t count, loff_t *f_ops);
ssize_t Buf_Write   (struct file *flip, const char __user *ubuf, size_t count, loff_t *f_ops);
long    Buf_Ioctl   (struct file *flip, unsigned int cmd, unsigned long arg);

int     Buf_In      (struct Buf_Struct *Buf, unsigned short *Data);
int     Buf_Out     (struct Buf_Struct *Buf, unsigned short *Data);

module_init(Buf_Init);
module_exit(Buf_Exit);

// Struct [Strc] --------------------------------------------------------------

struct Buf_Struct {
    unsigned int    InIdx;
    unsigned int    OutIdx;
    unsigned short  BufFull;
    unsigned short  BufEmpty;
    unsigned int    BufSize;
    unsigned short  *Buffer;
} Buffer;

struct Buf_Dev {
    unsigned short      *ReadBuf;
    unsigned short      *WriteBuf;
    struct   semaphore  SemBuf;
    unsigned short      numWriter;
    unsigned short      numReader;
    dev_t    dev;                   // Major, minor device number
    struct   cdev       cdev;       // character device (see cdev.h)
} BDev;

struct file_operations Buf_fops = {
    owner          : THIS_MODULE,
    open           : Buf_Open,
    release        : Buf_Release,
    read           : Buf_Read,
    write          : Buf_Write,
    unlocked_ioctl : Buf_Ioctl
    //fsync
    //fasync
    //lock
    //flush
};

// Global Variable [GVar] -----------------------------------------------------

structure BDev      *BDev_ptr;
structure Buffer    *Buf_ptr;
unsigned short      *Buf_str; // storage

int Buf_Major;
int Buf_Minor = 0;

int BufCode = -1;
EXPORT_SYMBOL_GPL(BufCode);

// Function definition [Fdef] -------------------------------------------------



int Buf_Init (void) {
    int Result = 0; // test variable

    BDev_ptr     = (struct BDev*)     kmalloc(sizeof(struct BDev),     GFP_KERNEL);
    Buf_ptr      = (struct Buffer*)   kmalloc(sizeof(struct Buffer),   GFP_KERNEL);
    Buf_str      = (unsigned short*)  kmalloc(sizeof(DEFAULT_BUFSIZE * sizeof(unsigned short)), GFP_KERNEL);

    if (!BDev_ptr && !Buf_ptr && !Buf_str) {
        printk(KERN_ERR DRIVER_NAME "Buf_Init:error_kmalloc")
        return -1; // look for a better error code
    }
    else {
        Buf_ptr->InIdx    = 0;
        Buf_ptr->OutIdx   = 0;
        Buf_ptr->BufFull  = 0;
        Buf_ptr->BufEmpty = 1;
        Buf_ptr->BufSize  = DEFAULT_BUFSIZE;
        Buf_ptr->*Buffer  = ;// Buf_str/&Buf_str/*Buf_str; // what do I do here?


        unsigned short      *ReadBuf;
        unsigned short      *WriteBuf;
        sema_init(&BDev_ptr.semaphore,1);
        BDev_ptr->numWriter = 0;
        BDev_ptr->numReader = 0;
        dev_t    dev;

        cdev_init (&BDev_ptr->cdev, &Buf_fops);
        BDev_ptr->cdev.owner = THIS_MODULE;
        BDev_ptr->cdev.ops = &Buf_fops;     // Is this needed ?

// myClass = class_create(THIS_MODULE,"MyDriverName");
// device_create(myClass, NULL, dev, NULL,"MyDriverName");

        // Inscription au noyeau
        Result = cdev_add (struct cdev *cdev, dev_t num, unsigned int count);
        if(Result < 0) {
            printk(KERN_ERR DRIVER_NAME "Buf_Init:error_registration")
            return -1;
        }
        else {
            printk(KERN_NOTICE DRIVER_NAME "Buffer initialized")
            return 0;
        }
    }
}

void    Buf_Exit (void) {
    // deleted buffer and bdev
    cdev_del(&BDev_ptr->cdev); // Unregister from kernel

    printk(KERN_NOTICE DRIVER_NAME "Buffer destroyed");
}

int     Buf_Open (struct inode *inode, struct file *flip) {
}

int     Buf_Release (struct inode *inode, struct file *flip) {
}

ssize_t Buf_Read (struct file *flip, char __user *ubuf, size_t count, loff_t *f_ops) {
}

ssize_t Buf_Write (struct file *flip, const char __user *ubuf, size_t count, loff_t *f_ops) {
}

long    Buf_Ioctl (struct file *flip, unsigned int cmd, unsigned long arg) {
}

int Buf_In (struct Buf_Struct *Buf, unsigned short *Data) {
    if (Buf->BufFull) {
        return -1; }
    Buf->BufEmpty = 0;
    Buf->Buffer[Buf->InIdx] = *Data;
    Buf->InIdx = (Buf->InIdx + 1) % Buf->BufSize;
    if (Buf->InIdx == Buf->OutIdx) {
        Buf->BufFull = 1; }
    return 0;
}

int Buf_Out (struct Buf_Struct *Buf, unsigned short *Data) {
    if (Buf->BufEmpty) {
        return -1; }
    Buf->BufFull = 0;
    *Data        = Buf->Buffer[Buf->OutIdx];
    Buf->OutIdx  = (Buf->OutIdx + 1) % Buf->BufSize;
    if (Buf->OutIdx == Buf->InIdx) {
        Buf->BufEmpty = 1; }
    return 0;
}