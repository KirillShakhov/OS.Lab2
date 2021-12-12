cmd_/home/savva/osi/Module.symvers := sed 's/ko$$/o/' /home/savva/osi/modules.order | scripts/mod/modpost -m -a   -o /home/savva/osi/Module.symvers -e -i Module.symvers   -T -
