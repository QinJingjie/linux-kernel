#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/list.h>
#include <linux/kvm_host.h>
MODULE_LICENSE("Dual BSD/GPL");
// init_task ffffffff81e10500
// init_thread_union ffffffff81e00000
#define Init_task 0xffffffff8ce10500
#define Init_thread_union 0xffffffff81e00000
extern struct my_kvm_info_struct my_kvm_info;
extern int read_data_from_guest(struct kvm_vcpu *vcpu, u64 address, void *data, unsigned long len);
struct task_struct task;
struct list_head *pos;
struct guest_procinfo{
    int pid; // proc id
    char comm[16]; // proc name
    unsigned long tasks; // next proc
}gpinfo[400];
char buf[16];
int init_module(void)
{
	printk(KERN_ALERT "~~~~~~~~jsk_test: Hello, world!\n");
    printk("~~~~~~~~jsk_test: \n \
           Init_task: %lx \n \
           Init_thread_union: %lx \n \
           cpu: %i \n \
           vcpu_id: %i", Init_task, Init_thread_union, my_kvm_info.vcpu->cpu, my_kvm_info.vcpu->vcpu_id);
	 if(read_data_from_guest(my_kvm_info.vcpu, Init_task, &task, sizeof(struct task_struct))){
         printk("~~~~~~~~jsk_test: read Init_thread_union err\n");
         printk(" pid: %d, state:%ld, flags: %x, stack: %x\n", task.pid, task.state, task.flags, task.stack);
         printk("struct phy address: %lx \n", (unsigned long)&task);
         // for(j = 0;j < 16;j++){
         //    printk("%c",task.comm[j]);
         // }
         printk("\n");
         printk("\n");
         printk("111\n");
         return 0;
	 }else{
	     printk("~~~~~~~~jsk_test: read Init_thread_union SUC!!\n");
         return 0;
	 }
//	gpinfo[i].pid = task->pid;
//	for(j = 0;j < 16;j++){
//        gpinfo[i].comm[j] = task->comm[j];
//	}
//	pos = &(task->tasks);
//	gpinfo[i].tasks = (unsigned long)pos;
//    printk("~~~~~~~jsk_test: %i***%s***%lx\n", gpinfo[i].pid, gpinfo[i].comm, gpinfo[i].tasks);
//    task = list_entry(pos, struct task_struct, tasks);
//
//	while(!read_data_from_guest(my_kvm_info.vcpu, gpinfo[i].tasks, task, sizeof(struct task_struct))){
//	    i += 1;
//        gpinfo[i].pid = task->pid;
//        for(j = 0;j < 16;j++){
//            gpinfo[i].comm[j] = task->comm[j];
//        }
//        pos = &(task->tasks);
//        gpinfo[i].tasks = (unsigned long)pos;
//        printk("~~~~~~~jsk_test: %i***%s***%lx\n", gpinfo[i].pid, gpinfo[i].comm, gpinfo[i].tasks);
//        if(gpinfo[i].tasks == Init_thread_union){
//            printk(KERN_ERR "~~~~~~~~jsk_test: read proc chain over.\n");
//            return 0;
//        }else{
//            task = list_entry(pos, struct task_struct, tasks);
//        }
//	}
//    printk(KERN_ERR "~~~~~~~~jsk_test: read next task err.\n");
	return 0;
}
void cleanup_module(void){
	printk(KERN_ALERT "~~~~~~~~jsk_test.o: byebye, world!\n");
}
