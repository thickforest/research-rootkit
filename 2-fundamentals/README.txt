linux从3.11内核开始移除了file_operations.readdir接口，我们需要改用file_operations.iterate来替代

fshid2 等以 2 结尾的文件夹都是针对 2.6.32 内核版本的
