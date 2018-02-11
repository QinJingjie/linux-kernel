#include <linux/time.h>
#include "nem_operations.h"
#include <asm/vmx.h>
#include <linux/list.h>
#include <linux/sched.h>
#include <linux/ctype.h>
#include "nem_interface.h"
static struct virtual_machine_info vm_info = {
	.vcpu = NULL,
	.update_proc_times = 0,
	.last_update_process_time = 0,
	.pnode_head = NULL,
};
char *sysmap = "/proc/kallsyms";
int get_current_proc_l(struct kvm_vcpu *vcpu, struct proc_identity *pro_identity, struct task_related_monitor *task);
int kill_process_manually(struct vm_info * vm, struct pnode *pnode);
extern struct task_struct *init_task_struct;
// static int get_symbol_row(FILE * f, char *row, const char *symbol, int position)
// {
// 	int ret = -1;
// 	while(fgets(row, 500, f) != NULL)
// 	{

//         /* find the correct token to check */
//         int curpos = 0;
//         int position_copy = position;

//         while (position_copy > 0 && curpos < 500) {
//             if (isspace(row[curpos])) {
//                 while (isspace(row[curpos])) {
//                     row[curpos] = '\0';
//                     ++curpos;
//                 }
//                 --position_copy;
//                 continue;
//             }
//             ++curpos;
//         }
//         if (position_copy == 0) {
//             token = row + curpos;
//             while (curpos < 500) {
//                 if (isspace(row[curpos])) {
//                     row[curpos] = '\0';
//                 }
//                 ++curpos;
//             }
//         } else {  some went wrong in the loop above 
//             goto error_exit;
//         }
//         /* check the token */
//         if (strncmp(token, symbol, 500) == 0) {
//             ret = 0;
//             break;
//         }
// 	}
// 	error_exit:
//     if (ret == -1) {
//         memset(row, 0, 500);
//     }
//     return ret;
// }

// static int linux_system_map_symbol_to_address(char *symbol, unsigned long *address)
// {
// 	char *row = NULL;
// 	row = kmalloc(500, GFP_KERNEL);
// 	File *f = NULL;
// 	int ret = -1;
// 	if((f = fopen(sysmap, "r")) == NULL){
// 		printk(KERN_ALERT "could not find sysmap file\n");
// 		address = 0;
// 		goto done;
// 	}
// 	if(get_symbol_row(f, row, symbol, 2) == -1){
// 		printk(KERN_ALERT "could not find symbol\n");
// 		address = 0;
// 		goto done;
// 	}
// 	ret = 0;
// 	printk(KERN_ALERT "find symbol %s address %lx\n", symbol, address);
// 	done:
// 		if(row)
// 			kfree(row);
// 		if(f)
// 			fclose(f);
// 		return ret;
// }

int create_vm_info(struct kvm_vcpu *vcpu){
//	struct virtual_machine_info *vm_info = NULL;
//	struct timespec tm;

//	vm_info = (struct virtual_machine_info *)kmalloc(sizeof(struct virtual_machine_info), GFP_KERNEL);
//	if(NULL == vm_info){
//		printk(KERN_ALERT "create_vm_info: kmalloc vm_info error!\n");
//		return NULL;
//	}
	printk(KERN_ALERT "create_vm_info!\n");
	vm_info.vcpu = vcpu;
	vm_info.update_proc_times = 0;
	vm_info.last_update_process_time = 0;
	return 0;
}

int destroy_vm_info(struct kvm_vcpu *vcpu)
{
	return 1;
}

struct task_struct* get_init_task(void)
{
	struct kvm_vcpu *vcpu = NULL;
	vcpu = vm_info.vcpu;
	unsigned long init_task_addr;
	init_task_addr = 0xffffffffa2e10500;
	struct task_struct *swapper = NULL;
	swapper = kmalloc(sizeof(struct task_struct), GFP_KERNEL);
	if(!read_data_from_guest(vcpu, init_task_addr, swapper, sizeof(struct task_struct)))
	{
		printk("read init_task success\n");
		return swapper;
	}
	printk("read init_task fail swapper: %ld\n", swapper);
	return NULL;
}

struct task_struct* get_task_by_pid(pid_t pid){
	printk(KERN_ALERT "init_task_struct is: %lx\n", init_task_struct);
	struct task_struct *swapper = init_task_struct;
	struct task_struct *p;
	struct list_head *pos;
	if(NULL != swapper){
		list_for_each(pos, &swapper->tasks){
			p = list_entry(pos, struct task_struct, tasks);
			if((int)p->pid == pid){
				return p;
			}
		}
	}
	return NULL;
}

int kill_process_by_pid(pid_t pid){
	struct kvm_vcpu *vcpu = NULL;
	struct pnode *pPnode = NULL;
	vcpu = vm_info.vcpu;
	// pPnode = vm_info.first_pnode;
	// while(pPnode){
	// 	if(pPnode->proc_details.proc_identity.pid == pid){
	// 		break;
	// 	}
	// 	pPnode = pPnode->next_pnode;
	// }
	// if(!pPnode){	/* can not find the process */
	// 	printk("can't find process\n");
	// 	return -1;
	// }
	struct task_struct *task = get_task_by_pid(pid);
	printk(KERN_INFO "Find the pPnode!");
	//unsigned long process_task_addr =  pPnode->proc_details.proc_identity.task_struct_addr;
	printk(KERN_INFO "The To be killed Process's task strcut address is %x",task);

	unsigned long process_pending_sigpending_addr = task + guest_offset[4];
	unsigned long process_pending_signal_addr = task + guest_offset[4] + guest_offset[5];
	unsigned long thread_info_flag_addr = task + guest_offset[3];
	long sigkill_value_0 = 0x0000000000000100;
	long sigkill_value_1 = 0x0000000000000000;
	unsigned long tif_sigpending = 1UL << 2;
	 if(write_data_to_guest(vcpu, process_pending_signal_addr,&sigkill_value_0, sizeof(unsigned long))&&
	 	write_data_to_guest(vcpu, process_pending_sigpending_addr, &sigkill_value_1, sizeof(unsigned long))
	 	&&write_data_to_guest(vcpu, thread_info_flag_addr, &tif_sigpending,  sizeof(unsigned long))){
	 	printk("kill process fail\n");
	 	return -2;
	 }else{
		printk("kill process by pid %d\n", pid);
	 }

	 return 0;
}