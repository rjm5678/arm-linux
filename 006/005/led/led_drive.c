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


/* 确定主设备号 */
static int major = 0;
static char kernel_buf[1024];

static struct class *led_class;
static struct class_device	*led_class_dev;



/* 实现file_operations结构体成员 */
ssize_t led_drv_read(struct file *file, char __user *buf, size_t size, loff_t *offset)
{
	int err;

	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	err = copy_to_user(buf, kernel_buf, size);	//把数据从kernel_buf读到用户空间
	return size;
}
ssize_t led_drv_write(struct file *file, char __user *buf, size_t size, loff_t *offset)
{
	int err;

	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	err = copy_from_user(kernel_buf, buf, size);	//把用户空间的数据保存到kernel_buf	
	return size;
}
int led_drv_open (struct inode *node, struct file *file)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	return 0;
}
int led_drv_release (struct inode *node, struct file *file)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	return 0;
}



/* 定义结构体，传参给内核 */
static const struct file_operations led_drv = {
	.owner		= THIS_MODULE,
	.open		= led_drv_open,
	.read		= led_drv_read,
	.write		= led_drv_write,
	.release 	= led_drv_release,
};

/* 初始化（注册）驱动 */
static int __init led_init(void)
{
	major = register_chrdev(major, "led", &led_drv);

	led_class = class_create(THIS_MODULE, "led");

	led_class_dev = class_device_create(led_class, NULL, MKDEV(major, 0), NULL, "xyz"); /

	return 0;

}

/* 卸载驱动 */
static void __exit led_exit(void)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	device_destroy(led_class, MKDEV(major, 0));
	class_destroy(led_class);
	unregister_chrdev(major, "led");
}



/* 7. 其他完善：提供设备信息，自动创建设备节点                                     */

module_init(led_init);
module_exit(led_exit);

MODULE_LICENSE("GPL");


