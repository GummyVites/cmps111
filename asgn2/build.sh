cp /usr/home/alyehuan/alyehuan/asgn2/kern_switch.c /usr/src/sys/kern/kern_switch.c
cp /usr/home/alyehuan/alyehuan/asgn2/sched_ule.c  /usr/src/sys/kern/sched_ule.c
cd /usr/src
make buildkernel -DKERNFAST
make installkernel