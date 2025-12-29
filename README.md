# Process Monitor and Analyzer: A Minimal Linux Kernel Module

## 🎯 Project Objective
To design, develop, and deploy a lightweight Linux Kernel Module (LKM) that hooks into the `/proc` filesystem and provides real-time statistics on process creation, termination, and resource usage (CPU & Memory).

## 🛠️ Tech Stack
- **Language:** C (Kernel Module), Python (User-space Dashboard)
- **Kernel:** Linux 5.x / 6.x
- **Mechanisms:** Kprobes, `task_struct` analysis, Procfs
- **Tools:** Make, GCC, Insmod

## 🚀 Features
1. **Kernel Space (Module):**
   - Lists all currently running processes.
   - **Real-time Tracking:** Uses Kprobes to hook `wake_up_new_task` (creation) and `do_exit` (termination).
   - **Metrics:** Calculates per-process RSS Memory (KB) and cumulative CPU time (ms).
   - **Filtering:** Supports filtering by User ID (UID) via module parameters.
2. **User Space (CLI Tool):**
   - Python-based dashboard that auto-refreshes every 2 seconds.
   - Sorts processes by active CPU usage.
   - Alerts if memory usage exceeds 100MB.

## 📦 Build & Run Instructions

### 1. Compile the Module
Run the makefile to build the kernel object (`.ko`):
```bash
make
```

### 2. Load the Kernel Module
You can load the module in two modes:

**Mode A: Monitor All Processes**
```bash
sudo insmod proc_monitor.ko
```

**Mode B: Filter by User ID (e.g., monitor only UID 1000)**
```bash
sudo insmod proc_monitor.ko filter_uid=1000
```

### 3. Verify Data
Check if the module is outputting data to the virtual file:
```bash
cat /proc/proc_monitor
```

### 4. Run the Dashboard
Start the Python visualization tool:
```bash
python3 monitor.py
```
*(Press Ctrl+C to exit the dashboard)*

### 5. Unload the Module
When finished, remove the module from the kernel:
```bash
sudo rmmod proc_monitor
```

## 🧹 Cleanup
To remove compiled files and clean the directory:
```bash
make clean
```

## 📝 Author
**Sampathkumar**
Project completed as a 4-week Linux Kernel Challenge.
