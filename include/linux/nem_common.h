#define VMM_API_DEV_PATH "/dev/vmm-api"

#ifdef TASK_COMM_LEN	/* this macro is define in include/linux/sched.h */
#define PNODE_CMD_LEN TASK_COMM_LEN
#else
#define PNODE_CMD_LEN 16
#endif
#define VMM_API_IOC_MAGIC 'a'
#define VMM_API_GET_PROC		_IOR(VMM_API_IOC_MAGIC,		1,	struct proc_details)
#define VMM_API_KILL_PROC_BY_PID		_IOWR(VMM_API_IOC_MAGIC,		2, pid_t)

strcut proc_identity {
	unsigned long task_struct_addr;	
	int pid;
	char cmd[PNODE_CMD_LEN];
	unsigned int pgd;
}	

struct proc_details {
	struct proc_identity proc_identity;
	/*
	 * Identify whether the process is dead or alive.
	 * Basic method: check whether the process is in task_struct list.
	 * value: enmu PNODE_STATE
	 * */
	int state;
	int has_next_proc;		/* only used before copy_to_user */
};	