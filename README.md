## Lab 01

### Q1.
1. The command `more /proc/cpuinfo` give the information related to each of the **logical** processors present on the computer.  
The term **core** refers to the actual physical locations to execute instructions.

2. My machine has 6 cores.

3. My machine has 12 logical processors, after taking into account **hyperthreading**.

4. Each processor has a different frequency: 2045.580 MHz, 1841.798 MHz, ...

5. Architecture: **x86_64**

6. Total available physical memory: 15307.2 MB

7. Total free memory : 7480.3 MB

8. Total number of forks = Total number of processes = 17060  
   Total number of context switches = 21235244


### Q2.

1. PID = 17461

2. CPU: 100%  
   MEM: 0.0%

3. It is in running state.

### Q3.

1. Run the command
    ```bash
    ps -e | grep "<name of the executable>"
    ```

2. Run the above command as mamny times as we wish to find the pids of ancestors, or write a bash script.

3.

4.

5. All the command executables are present in `/bin` folder. 
   The commands present as executables are **ps** and **ls**. The ones that are implemented by the bash code are **history** and **cd**.

### Q4.

1. The first process takes 6468 MB as VM and 5376 MB as PM (the actual memory)
2. The second process is taking the exact same size as the first one.