#include <linux/time.h>
#include <linux/kvm_host.h>
#include <linux/nem_common.h>
#include "nem_operations.h"
#include <asm/vmx.h>

static struct virtual_machine_info vm_info = {
	.vcpu = NULL,
	.update_proc_times = 0,
	.last_update_process_time = 0,
	.pnode_head = NULL,
};

int get_current_proc_l(struct kvm_vcpu *vcpu, struct proc_identity *pro_identity, struct task_related_monitor *task);
int kill_process_manually(struct vm_info * vm, struct pnode *pnode);

void create_vm_info(struct kvm_vcpu *vcpu){
//	struct virtual_machine_info *vm_info = NULL;
//	struct timespec tm;

//	vm_info = (struct virtual_machine_info *)kmalloc(sizeof(struct virtual_machine_info), GFP_KERNEL);
//	if(NULL == vm_info){
//		printk(KERN_ALERT "create_vm_info: kmalloc vm_info error!\n");
//		return NULL;
//	}
	vm_info.vcpu = vcpu;
	vm_info.update_proc_times = 0;
	vm_info.last_update_process_time = 0;
}

int kill_process_by_pid(struct kill_pro_identity *kill_arg){
	struct kvm_vcpu *vcpu = NULL;
	struct pnode *pPnode = NULL;

	vcpu = vm_info->vcpu;
	pPnode = pVM->first_pnode;

	unsigned int process_task_addr =  pPnode->proc_details.proc_identity.task_struct_addr;
	printk(KERN_INFO "The To be killed Process's task strcut address is %x",process_task_addr);
}