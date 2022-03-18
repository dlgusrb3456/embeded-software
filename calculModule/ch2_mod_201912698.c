#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>

MODULE_LICENSE("GPL");

#define DEV_NAME "cal_param_dev"

#define IOCTL_START_NUM 0x80
#define IOCTL_NUM1 IOCTL_START_NUM+1
#define IOCTL_NUM2 IOCTL_START_NUM+2
#define IOCTL_NUM3 IOCTL_START_NUM+3
#define IOCTL_NUM4 IOCTL_START_NUM+4

#define PARAM_IOCTL_NUM 'z'
#define GET_IOCTL _IOWR(PARAM_IOCTL_NUM,IOCTL_NUM1,unsigned long)
#define SET_IOCTL _IOWR(PARAM_IOCTL_NUM,IOCTL_NUM2,unsigned long)
#define ADD_IOCTL _IOWR(PARAM_IOCTL_NUM,IOCTL_NUM3,unsigned long)
#define MUL_IOCTL _IOWR(PARAM_IOCTL_NUM,IOCTL_NUM4,unsigned long)

static long result = 0;
module_param(result,long,0);

static int cal_param_open(struct inode *inode,struct file *file){
	printk("cal_param: open.\n");
	return 0;
}

static int cal_param_release(struct inode *inode, struct file *file){
	printk("cal_param: release. \n");
	return 0;
}

static long cal_param_ioctl(struct file *file, unsigned int cmd, unsigned long arg){

	switch(cmd){
		case GET_IOCTL:
			printk("cal_param: return result %ld \n",result);
			return result;
		case SET_IOCTL:
			printk("cal_param: set result %ld to %ld \n",result,(long)arg);
			result = (long)arg;
			return result;
		case ADD_IOCTL:
			printk("cal_param: ADD result %ld to %ld \n",result,(long)arg);
			result += (long)arg;
			return result;
		case MUL_IOCTL:
			printk("cal_param: MUL result %ld to %ld \n", result,(long)arg);
			result *= (long)arg;
			return result;
		default:
			printk("cal_param: error \n");
			return -1;
	}
	return 0;
}

struct file_operations cal_param_fops = {
	.open = cal_param_open,
	.release = cal_param_release,
	.unlocked_ioctl = cal_param_ioctl,
};

static dev_t dev_num;
static struct cdev *cd_cdev;

static int __init ch2_mod_201912698_init(void){
	printk("cal_param: init modules\n");

	alloc_chrdev_region(&dev_num,0,1,DEV_NAME);
	cd_cdev = cdev_alloc();
	cdev_init(cd_cdev, &cal_param_fops);
	cdev_add(cd_cdev,dev_num,1);

	return 0;
}

static void __exit ch2_mod_201912698_exit(void){
	printk("cal_param: exit modules\n");

	cdev_del(cd_cdev);
	unregister_chrdev_region(dev_num,1);
}

module_init(ch2_mod_201912698_init);
module_exit(ch2_mod_201912698_exit);







