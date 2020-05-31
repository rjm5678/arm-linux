#include <linux/module.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/miscdevice.h>
#include <linux/kernel.h>
#include <linux/major.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/stat.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/tty.h>
#include <linux/kmod.h>
#include <linux/mm.h>
#include <linux/highmem.h>
#include <linux/blkdev.h>
#include <linux/module.h>
#include <linux/backing-dev.h>
#include <linux/interrupt.h>
#include <asm/uaccess.h>



/* 确定主设备号 */
static int major = 0;
static char kernel_buf[1024];
static struct class *hello_class;


/* 实现file_operations结构体成员 */
ssize_t hello_drv_read(struct file *file, char __user *buf, size_t size, loff_t *offset)
{
	int err;

	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	err = copy_to_user(buf, kernel_buf, size);	//把数据从kernel_buf读到用户空间
	return size;
}
ssize_t hello_drv_write(struct file *file, const char __user *buf, size_t size, loff_t *offset)
{
	int err;

	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	err = copy_from_user(kernel_buf, buf, size);	//把用户空间的数据保存到kernel_buf	
	return size;
}
int hello_drv_open (struct inode *node, struct file *file)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	return 0;
}
int hello_drv_release (struct inode *node, struct file *file)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	return 0;
}



/* 定义结构体，传参给内核 */
static const struct file_operations hello_drv = {
	.owner		= THIS_MODULE,
	.open		= hello_drv_open,
	.read		= hello_drv_read,
	.write		= hello_drv_write,
	.release 	= hello_drv_release,
};

/* 初始化（注册）驱动 */
static int __init hello_init(void)
{
	int err;
	
	major = register_chrdev(major, "hello", &hello_drv);

	hello_class = class_create(THIS_MODULE, "hello_class");
	err = PTR_ERR(hello_class);
	if (IS_ERR(hello_class)) {
		printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
		unregister_chrdev(major, "hello");
		return -1;
	}
	device_create(hello_class, NULL, MKDEV(major, 0), NULL, "hello"); /* /dev/hello */

	return 0;

}

/* 卸载驱动 */
static void __exit hello_exit(void)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	device_destroy(hello_class, MKDEV(major, 0));
	class_destroy(hello_class);
	unregister_chrdev(major, "hello");
}

/* 7. 其他完善：提供设备信息，自动创建设备节点                                     */

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");


