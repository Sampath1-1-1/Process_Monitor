import time
import os
import sys

# --- Configuration ---
PROC_FILE = "/proc/proc_monitor"
REFRESH_RATE = 2  # Seconds between updates
MEMORY_THRESHOLD_KB = 100 * 1024  # Alert if RSS > 100MB

def read_proc_data():
    """
    Reads /proc/proc_monitor and parses it into a dictionary.
    Returns: {pid: {'uid': uid, 'cmd': cmd, 'rss': rss, 'cpu_total': cpu_ms}}
    """
    processes = {}
    try:
        with open(PROC_FILE, 'r') as f:
            lines = f.readlines()
            
        # Skip header lines until we find the table
        start_parsing = False
        for line in lines:
            if "PID" in line and "RSS" in line:
                start_parsing = True
                continue
            
            if not start_parsing:
                continue
                
            parts = line.split()
            # Ensure line has enough columns (PID, UID, PPID, RSS, CPU, CMD)
            if len(parts) >= 6:
                try:
                    pid = int(parts[0])
                    uid = int(parts[1])
                    rss = int(parts[3])
                    cpu_ms = int(parts[4])
                    cmd = " ".join(parts[5:]) # Join remaining parts as command
                    
                    processes[pid] = {
                        'uid': uid,
                        'cmd': cmd,
                        'rss': rss,
                        'cpu_total': cpu_ms
                    }
                except ValueError:
                    continue
    except FileNotFoundError:
        print(f"Error: {PROC_FILE} not found. Is the kernel module loaded?")
        sys.exit(1)
        
    return processes

def clear_screen():
    os.system('cls' if os.name == 'nt' else 'clear')

def main():
    print("Initializing Monitor...")
    prev_processes = read_proc_data()
    
    while True:
        try:
            time.sleep(REFRESH_RATE)
            curr_processes = read_proc_data()
            
            # List to store processed data for display
            display_list = []
            
            # Calculate Usage
            for pid, p_curr in curr_processes.items():
                if pid in prev_processes:
                    p_prev = prev_processes[pid]
                    
                    # Calculate CPU used in this interval
                    cpu_delta = p_curr['cpu_total'] - p_prev['cpu_total']
                    
                    # Store for sorting
                    display_list.append({
                        'pid': pid,
                        'cmd': p_curr['cmd'],
                        'rss': p_curr['rss'],
                        'cpu_usage': cpu_delta  # Milliseconds used in last interval
                    })
            
            # Sort by CPU Usage (Descending)
            display_list.sort(key=lambda x: x['cpu_usage'], reverse=True)
            
            # --- RENDER DASHBOARD ---
            clear_screen()
            print(f"=== ProcMonitor Dashboard (Refresh: {REFRESH_RATE}s) ===")
            print(f"{'PID':<8} {'RSS (MB)':<12} {'CPU (ms/interval)':<20} {'COMMAND'}")
            print("-" * 60)
            
            # Show Top 15
            for p in display_list[:15]:
                rss_mb = p['rss'] / 1024
                
                # Check for Alert
                alert = ""
                if p['rss'] > MEMORY_THRESHOLD_KB:
                    alert = " [High Memory!]"
                    
                print(f"{p['pid']:<8} {rss_mb:<12.1f} {p['cpu_usage']:<20} {p['cmd']}{alert}")
                
            # Update previous state for next loop
            prev_processes = curr_processes
            
        except KeyboardInterrupt:
            print("\nExiting...")
            break

if __name__ == "__main__":
    main()