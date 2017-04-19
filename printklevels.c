#include<linux/kernel.h> 
#include<linux/module.h> 
#include<linux/init.h> 

static int hello_msg(void) 
{
   printk(KERN_EMERG "Emergency message ! \n ");
   printk(KERN_ALERT "Alert message ! \n ");
   printk(KERN_CRIT "Critical message ! \n ");
   printk(KERN_ERR "Error message ! \n ");
   printk(KERN_WARNING "Warning message ! \n ");
   printk(KERN_NOTICE "Notice message ! \n ");
   printk(KERN_INFO "Info message ! \n ");
   return 0; 
} 

static void bye_debug(void)
{
   printk(KERN_INFO "Debug message ! \n"); 
}

module_init(hello_msg);
module_exit(bye_debug);
