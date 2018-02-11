#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/list.h>
#include <linux/kvm_host.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <asm/uaccess.h>
#include <linux/miscdevice.h>
#include "nem_interface.h"
#include "nem_operations.h"

MODULE_LICENSE("Dual BSD/GPL");
#define MODULE_NAME "nem"

long misc_vmm_api_ioctl(struct file *filep, unsigned int cmd, unsigned long arg);

struct file_operations vmm_api_fops = {
//	.ioctl = misc_vmm_api_ioctl,
	.unlocked_ioctl = misc_vmm_api_ioctl,
	//when kernel is 64bit while userspace is 32bit
	.compat_ioctl = misc_vmm_api_ioctl,
};

struct miscdevice vmm_api_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.fops = &vmm_api_fops,
	.name = MODULE_NAME,
	.nodename = MODULE_NAME,
};

long misc_vmm_api_ioctl(struct file *filep, unsigned int cmd, unsigned long arg){
	if(VMM_API_IOC_MAGIC != _IOC_TYPE(cmd)) return -ENOTTY;
	if(_IOC_NR(cmd) > VMM_API_IOC_MAXNR) return -ENOTTY;
	void   *argp = (void   *)arg;
	int r = -1;
/*	To test the consistency between user space and kernel space
       printk(KERN_INFO "CMDis %x PRE_READ_VM is %x VMM_API_READ_ALL_VM is %x VMM_API_READ_PROC_BY_VM is %x VMM_API_GET_CURRENT_PROCESS is %x\n",\
		cmd,VMM_API_PRE_READ_PROC_BY_VM,VMM_API_READ_ALL_VM,VMM_API_READ_PROC_BY_VM,VMM_API_GET_CURRENT_PROCESS);
	printk(KERN_INFO "vm_details size is %d vm_identity size is %d proc_details is %d\n",sizeof(struct vm_details),sizeof(struct vm_identity),sizeof(struct proc_details));*/
	switch(cmd){
	case VMM_API_GET_PROC:
	//	r = deliver_proc_by_vm(arg);
		break;
	case VMM_API_KILL_PROC_BY_PID:
		printk(KERN_INFO "Hello kill!");
		r = kill_process_by_pid(arg);
		break;
	default:
		break;
	}	
	return r;
}

int __init vmm_api_init(void){
	struct nem_operations ops;
	int ret = -1;

	ops.create_vm_info = create_vm_info;
	ops.destroy_vm_info = destroy_vm_info;

	//register KVMsafe monitoring interface
	register_monitor_interface(&ops);
	ret = misc_register(&vmm_api_dev);
	if(!ret){
		printk("vmm_api register success.\n");
	} else{
		printk(KERN_ERR "Error %d adding vmm_api.\n", ret);
	}
	return ret;
}

static void __exit vmm_api_exit(void){
	misc_deregister(&vmm_api_dev);
	unregister_monitor_interface();
	//free_all_structs();
	printk(KERN_INFO "vmm_api_info: unregister device successfully\n");
}

module_init(vmm_api_init);
module_exit(vmm_api_exit);