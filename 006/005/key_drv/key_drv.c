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

static struct class *key_class;
static struct class_device	*key_drv_class;
static int major = 0;

unsigned int *pGPFCON = NULL; 
unsigned int *pGPFDAT = NULL; 
unsigned int *pGPGCON = NULL; 
unsigned int *pGPGDAT = NULL; 

ssize_t key_drv_read(struct file *file, char __user *buf, size_t size, loff_t *offset)
{
	unsigned char keyval[4];

	if (size != sizeof(keyval))
		return -EINVAL;

	keyval[0] = (*pGPFDAT & (1 << 0)) ? 1:0;
	keyval[1] = (*pGPFDAT & (1 << 2))? 1:0;

	keyval[2] = (*pGPGDAT & (1 << 3)) ? 1:0;
	keyval[3] = (*pGPGDAT & (1 << 11)) ? 1:0;
	 
	
	copy_to_user(buf, keyval, sizeof(keyval));	//把数据从kernel_buf读到用户空间
	
	return sizeof(keyval);
}
int key_drv_open (struct inode *node, struct file *file)
{

	*pGPFCON &= ~((3<<0) | (3<<4)); /* 把GPGCON 0~1, 4~5位置清零 */
	*pGPGCON &= ~((3<<6) | (3<<22)); /* 把GPGCON 6~7 , 22~23 位清零 */


	return 0;
}
/* 定义结构体，传参给内核 */
static const struct file_operations key_drv_fo = {
	.owner		= THIS_MODULE,
	.open		= key_drv_open,
	.read		= key_drv_read,
};

/* 初始化（注册）驱动 */
static int __init key_drv_init(void)
{
	
	major = register_chrdev(0, "key_drv", &key_drv_fo);

	key_class = class_create(THIS_MODULE, "key_drv");
	
	key_drv_class = class_device_create(key_class, NULL, MKDEV(major, 0), NULL, "key_drv"); /* /dev/key_drv */

	pGPFCON = (unsigned int *)ioremap(0x56000050, 16);
	pGPFDAT = pGPFCON + 1;

	pGPGCON = (unsigned int *)ioremap(0X56000060, 16);
	pGPGDAT = pGPGCON + 1;

	return 0;

}

/* 卸载驱动 */
static void __exit key_drv_exit(void)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	class_device_unregister(key_drv_class);
	class_destroy(key_class);
	unregister_chrdev(major, "key_drv");
	iounmap(pGPFCON);
	iounmap(pGPGCON);
}



/* 7. 其他完善：提供设备信息，自动创建设备节点                                     */

module_init(key_drv_init);
module_exit(key_drv_exit);

MODULE_LICENSE("GPL");


