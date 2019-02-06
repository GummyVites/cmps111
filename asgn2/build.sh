cp /usr/home/kemalee/cmps111/asgn2/kern_switch.c /usr/src/sys/kern/kern_switch.c
cp /usr/home/kemalee/cmps111/asgn2/sched_ule.c  /usr/src/sys/kern/sched_ule.c
# cp /usr/home/alyehuan/alyehuan/asgn2/runq.h  /usr/src/sys/sys/runq.h
cd /usr/src
make buildkernel -DKERNFAST
while true; do
    read -p "Install program? [y/n]" yn
    case $yn in
        [Yy]* ) make installkernel; reboot;;
        [Nn]* ) exit;;
        * ) echo "Please answer yes or no.";;
    esac
done
