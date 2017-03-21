rmmod -f 可以强制删除模块,而不检查模块是否在/proc/modules列表中

>>> rmmod kohidko
ERROR: Module kohidko does not exist in /proc/modules
>>> rmmod -f kohidko
>>> dmesg 
kohidko.cleanup_module: Opening the path: /sys/module.
kohidko.cleanup_module: Succeeded in opening: /sys/module
kohidko.cleanup_module: Changing file_op->readdir from ffffffffa031f000 to ffffffff81218e80.
kohidko.cleanup_module: Opening the path: /proc/modules.
kohidko.cleanup_module: Succeeded in opening: /proc/modules
kohidko.cleanup_module: Changing seq_op->show from ffffffffa031f020 to ffffffff810c4580.
kohidko.cleanup_module: Farewell the World!

