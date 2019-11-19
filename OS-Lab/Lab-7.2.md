# 使用 SHELL 计算磁盘上所有目录下平均文件个数、所有目录平均深度、所有文件名平均长度

这里主要用到的就是 `awk` 的特性配合 `find` 进行数据处理。

```bash
#!bin/bash

echo 'Average filename length is:'
find . -type d | xargs -l -I {} sh -c "find {} -type f -printf '%f\n'" | awk '{print length}' | awk '{ total+=$1; count+=1} END {print total/count}'
echo 'Average file count is:'
find . -type d | xargs -l -I {} sh -c "(ls -l {} | wc -l); (ls -l {} | grep "^d" | wc -l)" | awk '{if(count % 2 == 0){ total+=$1; count+=1} else {total-=$1; count += 1}} END {print total*2/count}'
echo 'Average directory depth is:'
find . -type d -printf '%d\n' | awk '{ total+=$1; count+=1} END {print total/count}'
```

为了保护我的价值连城的 SSD，我们选择在`/usr/share/man` 下执行这段脚本。结果如下啊：

```bash
name1e5s@DESKTOP-D8BIQ3U:/usr/share/man$ bash ~/7.2.sh
Average filename length is:
14.6361
Average file count is:
56.8641
Average directory depth is:
1.66019
```

