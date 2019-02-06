Assignment 2

Team:
- Alston Huang (alyehuan)
- Kevin Lee (kemalee)

Files Included:
- sched_ule.c
- kern_switch.c 
- benchmark.c
- benchmark_results
- build.sh
- README.md

*PRE-REQUESITE: Must have already ran "make buildkernel" at least once*
Compile Instructions: 
1. run "su" to go into root
2. cd into the directory containing "build.sh"
2. run "chmod 777 build.sh"
3. run "./build.sh"
    -   it will copy over sched_ule.c and kern_switch.c to 
        the current VM's /src/usr/sys/<files>
    -   will execute "make buildkernel -DKERNFAST" (remove flag if first time      running)
    -   After building is done, will prompt the user asking [Y/N] to install
        -   If no build errors, hit [Y]
    -   System will reboot with the new kernel

