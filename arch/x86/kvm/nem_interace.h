#include <linux/kvm_host.h>

// variable exist_monitor_system, judge whether the monitor is on
#define VMM_MONITOR_ON 1
#define VMM_MONITOR_OFF 0

struct nem_operations {
//	int (*intercept_syscall)(struct kvm_vcpu *vcpu);		//syscall interception interface
//	int (*intercept_proc_switch)(struct kvm_vcpu *vcpu);	//process switch interception interface
	void* (*create_vm_info)(struct kvm_vcpu *vcpu);		//initialize process monitor information
	int (*destroy_vm_info)(struct kvm_vcpu *vcpu);		//destroy process monitor information
};

//set kvm intel module monitor interface
int register_monitor_interface(struct nem_operations *ops);

//unset kvm intel module monitor interface
int unregister_monitor_interface(void);