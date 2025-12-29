#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/kprobes.h>
#include <linux/mm.h>       // Required for memory stats
#include <linux/moduleparam.h> // Required for filters

#define PROC_NAME "proc_monitor"

// --- Module Parameters (for Filtering) ---
// Default is -1 (show all). Users can change this when loading the module.
static int filter_uid = -1;
module_param(filter_uid, int, 0644);
MODULE_PARM_DESC(filter_uid, "Filter by User ID (UID). Set -1 to show all.");

// --- Atomic Counters (Week 2 Logic) ---
static atomic_t process_create_count = ATOMIC_INIT(0);
static atomic_t process_exit_count = ATOMIC_INIT(0);

// --- KProbe Hooks (Week 2 Logic) ---
static int handler_process_create(struct kprobe *p, struct pt_regs *regs) {
    atomic_inc(&process_create_count);
    return 0;
}
static struct kprobe kp_fork = { .symbol_name = "wake_up_new_task", .pre_handler = handler_process_create };

static int handler_process_exit(struct kprobe *p, struct pt_regs *regs) {
    atomic_inc(&process_exit_count);
    return 0;
}
static struct kprobe kp_exit = { .symbol_name = "do_exit", .pre_handler = handler_process_exit };

// --- Helper: Convert Pages to KB ---
static unsigned long get_rss_kb(struct mm_struct *mm) {
    if (!mm) return 0;
    // get_mm_rss returns number of pages. Multiply by PAGE_SIZE, divide by 1024 for KB.
    return (get_mm_rss(mm) * (PAGE_SIZE / 1024)); 
}

// --- /proc Display Function ---
static int proc_show(struct seq_file *m, void *v)
{
    struct task_struct *task;
    unsigned long rss_kb;
    u64 cpu_time_ms;
    int task_uid;

    // 1. Stats Header
    seq_printf(m, "=== Process Monitor Week 3 ===\n");
    seq_printf(m, "Processes Created: %d | Exited: %d\n", 
               atomic_read(&process_create_count), atomic_read(&process_exit_count));
    
    if (filter_uid != -1) {
        seq_printf(m, "Filtering Active: Showing only UID %d\n", filter_uid);
    } else {
        seq_printf(m, "Filtering: OFF (Showing all)\n");
    }
    seq_printf(m, "\n");

    // 2. Table Header
    // PID: Process ID
    // RSS_KB: Physical Memory used in Kilobytes
    // CPU_MS: Total CPU time (User + System) in milliseconds
    seq_printf(m, "%-6s %-6s %-6s %-10s %-10s %-15s\n", 
               "PID", "UID", "PPID", "RSS(KB)", "CPU(ms)", "COMMAND");

    // 3. Iterate Process List
    rcu_read_lock(); // Safety lock for list iteration
    for_each_process(task) {
        
        // Get the UID of the process owner
        task_uid = from_kuid(&init_user_ns, task_cred_xxx(task, uid));

        // FILTER CHECK: If filter is active AND uid doesn't match, skip this process
        if (filter_uid != -1 && task_uid != filter_uid) {
            continue;
        }

        // MEMORY CHECK (Critical for safety)
        // Kernel threads (kthreadd) have no mm. We must check before accessing.
        if (task->mm) {
            rss_kb = get_rss_kb(task->mm);
        } else {
            rss_kb = 0; // It's a kernel thread
        }

        // CPU CALCULATION
        // utime and stime are usually in nanoseconds in modern kernels
        cpu_time_ms = (task->utime + task->stime); 
        do_div(cpu_time_ms, 1000000); // Convert nanoseconds to milliseconds

        seq_printf(m, "%-6d %-6d %-6d %-10lu %-10llu %-15s\n",
                   task->pid,
                   task_uid,
                   task->real_parent->pid,
                   rss_kb,
                   cpu_time_ms,
                   task->comm);
    }
    rcu_read_unlock();
    
    return 0;
}

static int proc_open(struct inode *inode, struct file *file) {
    return single_open(file, proc_show, NULL);
}

static const struct proc_ops proc_file_ops = {
    .proc_open    = proc_open,
    .proc_read    = seq_read,
    .proc_lseek   = seq_lseek,
    .proc_release = single_release,
};

static int __init proc_monitor_init(void) {
    int ret;
    proc_create(PROC_NAME, 0, NULL, &proc_file_ops);
    
    ret = register_kprobe(&kp_fork);
    if (ret < 0) return ret;
    
    ret = register_kprobe(&kp_exit);
    if (ret < 0) { unregister_kprobe(&kp_fork); return ret; }
    
    printk(KERN_INFO "proc_monitor: Week 3 loaded (Filter UID: %d)\n", filter_uid);
    return 0;
}

static void __exit proc_monitor_exit(void) {
    unregister_kprobe(&kp_fork);
    unregister_kprobe(&kp_exit);
    remove_proc_entry(PROC_NAME, NULL);
    printk(KERN_INFO "proc_monitor: Unloaded\n");
}

module_init(proc_monitor_init);
module_exit(proc_monitor_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sampathkumar");
