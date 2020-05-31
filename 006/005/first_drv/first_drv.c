#include <linux/module.h>

#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/miscdevice.h>
#include <linux/kernel.h>
#include <linux/major.h>
#include <linux/mutex.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/stat.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/tty.h>
#include <linux/kmod.h>
#include <linux/gfp.h>
#include <linux/mm.h>
#include <linux/highmem.h>
#include <linux/blkdev.h>
#include <linux/module.h>
#include <linux/backing-dev.h>
#include <linux/interrupt.h>
#include <asm/uaccess.h>

unsigned int *pGPFCON = NULL; //指针变量pGPFCON设置为地址0x56000050
unsigned int *pGPFDAT = NULL; //指针变量pGPFDAT设置为地址0x56000054


static int major = 0;

static struct class *first_class;
static struct class_device *first_drv_class;


ssize_t first_drv_write(struct file *file, const char __user *buf, size_t size, loff_t *offset)
{
	int val;

//	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	copy_from_user(&val, buf, size);	//把用户空间的数据保存到kernel_buf	
	if (val == 1)
	{
		/* leds on */
		*pGPFDAT = ~((1<<4)| (1<<5) | (1<<6));
	}
	else if (val == 0)
	{	
		/* leds off */
		*pGPFDAT = (1<<4)| (1<<5) | (1<<6);
	}
	
	return 0;
}
int first_drv_open (struct inode *node, struct file *file)
{
//	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);

	*pGPFCON &= ~((3<<8)| (3<<10) | (3<<12)); /* 把GPFCON 8~13位清零 */
	*pGPFCON |=  ((1<<8)| (1<<10) | (1<<12)); /* 把GPFCON 9、11、13位置1，即设置为输出模式*/ 

	return 0;
}

/* 定义结构体，传参给内核 */
static const struct file_operations first_drv = {
	.owner		= THIS_MODULE,
	.open		= first_drv_open,
	.write		= first_drv_write,
};

/* 初始化（注册）驱动 */
static int __init first_drv_init(void)
{
//	int err;
	
	major = register_chrdev(0, "first_drv", &first_drv);

	first_class = class_create(THIS_MODULE, "first_drv");
	
	first_drv_class = class_device_create(first_class, NULL, MKDEV(major, 0), NULL, "first_drv"); /* /dev/hello */

	pGPFCON = (unsigned int *)ioremap(0x56000050, 16);
	pGPFDAT = pGPFCON + 1;
		
	return 0;

}

/* 卸载驱动 */
static void __exit first_drv_exit(void)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	class_device_unregister(first_drv_class);
	class_destroy(first_class);
	unregister_chrdev(major, "first_drv");
	iounmap(pGPFCON);
}


/* 7. 其他完善：提供设备信息，自动创建设备节点                                     */

module_init(first_drv_init);
module_exit(first_drv_exit);

MODULE_LICENSE("GPL");


