#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/time.h>
#include <linux/mm.h>
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("name1e5s");
MODULE_DESCRIPTION("Count page fault.");
MODULE_VERSION("0.1");

static const char *pf_count_name = "pfcount";
static const char *jiffies_name = "jiffies";

static int fill_info(struct seq_file *m, void *v) {
    seq_printf(m, "%ld\n", pfcount);
	return 0;
}

static int fill_info_jiffies(struct seq_file *m, void *v) {
    seq_printf(m, "%ld\n", jiffies);
	return 0;
}

static int open_pf(struct inode *inode, struct file *file) {
	return single_open(file, fill_info, NULL);
}

static int open_jiffies(struct inode *inode, struct file *file) {
	return single_open(file, fill_info_jiffies, NULL);
}


static const struct file_operations fops = {
	.llseek = seq_lseek,
	.open = open_pf,
	.owner = THIS_MODULE,
	.read = seq_read,
	.release = single_release,
};

static const struct file_operations fops_jiffies = {
	.llseek = seq_lseek,
	.open = open_jiffies,
	.owner = THIS_MODULE,
	.read = seq_read,
	.release = single_release,
};

static int __init pf_init(void) {
    proc_create(pf_count_name, 0, NULL, &fops);
    proc_create(jiffies_name, 0, NULL, &fops_jiffies);
    return 0;
}


static void __exit pf_exit(void){
    remove_proc_entry(pf_count_name, NULL);
    remove_proc_entry(jiffies_name, NULL);
}

module_init(pf_init);
module_exit(pf_exit);