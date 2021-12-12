cmd_/home/savva/osi/modules.order := {   echo /home/savva/osi/etx.ko; :; } | awk '!x[$$0]++' - > /home/savva/osi/modules.order
