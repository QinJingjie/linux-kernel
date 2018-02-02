#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/list.h>
#include <linux/kvm_host.h>
#include <linux/sched.h>
#include <linux/nem_common.h>
#include <linux/uaccess.h>
#include <asm/uaccess.h>
#include "nem_interface.h"

#define MODULE_NAME "vmm-api"
MODULE_LICENSE("Dual BSD/GPL");


int misc_vmm_api_ioctl(struct file *filep, unsigned int cmd, unsigned long arg);

struct miscdevice vmm_api_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.fops = &vmm_api_fops,
	.name = MODULE_NAME,
	.nodename = MODULE_NAME,
};

int misc_vmm_api_ioctl(struct file *filep, unsigned int cmd, unsigned long arg){
	if(VMM_API_IOC_MAGIC != _IOC_TYPE(cmd)) return -ENOTTY;
	if(_IOC_NR(cmd) > VMM_API_IOC_MAXNR) return -ENOTTY;
	void   *argp = (void   *)arg;
	struct kill_pro_identity *kill_arg = NULL;
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
		kill_arg = kzalloc(sizeof(struct kill_pro_identity), GFP_KERNEL);
		if(!kill_arg)
			goto out_free;
		if (copy_from_user(kill_arg,argp,sizeof(struct kill_pro_identity)))
			goto out_free;
		r = -1;
		printk(KERN_INFO "KILL_UUID: %02hhx%02hhx%02hhx%02hhx-%02hhx%02hhx-%02hhx%02hhx-%02hhx%02hhx-%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx\n"\
		, kill_arg->uuid[0], kill_arg->uuid[1], kill_arg->uuid[2],kill_arg->uuid[3],kill_arg->uuid[4],kill_arg->uuid[5],kill_arg->uuid[6],kill_arg->uuid[7],kill_arg->uuid[8],\
		kill_arg->uuid[9],kill_arg->uuid[10],kill_arg->uuid[11],kill_arg->uuid[12],kill_arg->uuid[13],kill_arg->uuid[14],kill_arg->uuid[15]);
		r = kill_process_by_pid(kill_arg);
		break;
	default:
		break;
	}
out_free:
	kfree(kill_arg);	
	return r;
}

int __init vmm_api_init(void){
	struct nem_operations ops;
	int ret = -1;
	
	ops.create_vm_info = create_vm_info;
	ops.destroy_vm_info = destroy_vm_info;

	//register KVMsafe monitoring interface
//	register_monitor_interface(&ops);
	ret = misc_register(&vmm_api_dev);
	if(!ret){
		printk("vmm_api register success.\n");
	} else{
		printk(KERN_ERR "Error %d adding vmm_api.\n", ret);
	}
	return ret;
}

module_init(vmm_api_init);
module_exit(vmm_api_exit);