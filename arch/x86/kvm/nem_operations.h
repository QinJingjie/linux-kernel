#ifndef KVM_UNIFDEF_H
#define KVM_UNIFDEF_H

#ifdef __i386__
#ifndef CONFIG_X86_32
#define CONFIG_X86_32 1
#endif
#endif

#ifdef __x86_64__
#ifndef CONFIG_X86_64
#define CONFIG_X86_64 1
#endif
#endif

#if defined(__i386__) || defined (__x86_64__)
#ifndef CONFIG_X86
#define CONFIG_X86 1
#endif
#endif

#ifdef __ia64__
#ifndef CONFIG_IA64
#define CONFIG_IA64 1
#endif
#endif

#ifdef __PPC__
#ifndef CONFIG_PPC
#define CONFIG_PPC 1
#endif
#endif

#ifdef __s390__
#ifndef CONFIG_S390
#define CONFIG_S390 1
#endif
#endif

#endif

#include <linux/in.h>
#include <linux/fs.h>
#include <linux/fdtable.h>
#include <linux/file.h>
#include <linux/net.h>
#include <linux/kernel.h>
#include <linux/stat.h>
#include <net/inet_sock.h>
#include <net/sock.h>
#include <linux/socket.h>
#include <linux/kvm_host.h>
#include <linux/nem_common.h>

#define OFFSET_LEN 6

struct virtual_machine_info {
	struct kvm_vcpu *vcpu;

	/*
	 * After a period of time, we need to iterate the task_struct list.
	 * So this variable helps us decide the time.
	 *  */
	unsigned long last_update_process_time;
	unsigned long update_proc_times;
	spinlock_t lock;		/* lock to avoid concurrency problems */
	struct {
		struct pnode *first;
		struct pnode *last;
		int cnt;
		/*
		 * mainly used in ioctl function, keep a pointer to the vm_info so that we can know what to transfer the next time.
		 * Usage: If the value of pRead is NULL, that means read from the first one of the list; otherwise, read the next one
		 * 		of the one which this pointer points to.
		 * Implementation:
		 * 		1. Reset it to NULL whenever delete a member from the list.
		 * 		2. If pRead is NULL, that means you should read from the first one of the list; otherwise, read the next one
		 * 			of the one which this pointer points to. If the next one is NULL, then returning a NULL structure suggests
		 * 			that the read process is over.
		 * 		3. After the read operation, set pRead to the current read structure.
		 * PS: The similar design appears in vmm_list_head, pnode. The implement and usage is the same.
		 * */
		struct pnode *pRead;
	}pnode_head;
	#define first_pnode pnode_head.first
	#define last_pnode pnode_head.last
};

struct pnode {
	/* the vcpu which this process runs on */
	struct kvm_vcpu *vcpu;
	/* construct a simple linked list */
	struct pnode *next_pnode;
	/*
	 * Maintain a list for processes which has socket connections.(A subset of all the pnode.)
	 *  */
	struct pnode *next_net_pnode;
	/* process details */
	struct proc_details proc_details;
};

struct task_related_monitor{
	int pid;
	int tgid;
	char comm[TASK_COMM_LEN];
	int tasks_offset;	//struct list_head tasks; offset
	int mm_offset;	// struct mm_struct *mm; offset
	int mm_pgd_offset; //pgd_t *pgd; offset
	int mm_start_code_offset; //unsigned long start_code; offset
};

//tasks, pid, comm, thread_info, pending, pending_singal
static unsigned long guest_offset[OFFSET_LEN] = {
	0x380,0x480,0x628,0x0,0x6a0,0x10
};

int create_vm_info(struct kvm_vcpu *vcpu);		//initialize process monitor information
int destroy_vm_info(struct kvm_vcpu *vcpu);		//destroy process monitor information
int kill_process_by_pid(pid_t pid);