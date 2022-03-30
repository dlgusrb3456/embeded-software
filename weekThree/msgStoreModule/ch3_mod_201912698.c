#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/spinlock.h>
#include <asm/delay.h>

#include "ch3.h"
#define DEV_NAME "ch3_mod_201912698_dev"

MODULE_LICENSE("GPL");
struct msg_st *kern_buf;
spinlock_t my_lock;

void delay(int sec){
	int i, j;
	for(j=0;j<sec;j++){
		for(i=0;i<1000;i++){
			udelay(1000);
		}
	}
}

struct msg_list{
	struct list_head list;
	struct msg_st msg;
};


static struct msg_list mylist;

static int ch3_mod_201912698_read(struct msg_st *buf){
       int ret;

       struct msg_list *mlTmp = 0;
       struct list_head *mlPos =0;
       struct list_head *mlQ =0;

       delay(2);
       spin_lock(&my_lock);
       list_for_each_safe(mlPos, mlQ, &mylist.list){
               mlTmp = list_entry(mlPos,struct msg_list,list);
               printk("ch3_mod_201912698: free msg[%s]",mlTmp->msg.str);
	       kern_buf = &mlTmp->msg;
               list_del(mlPos);
               kfree(mlTmp);
	       break;
       }


       ret = copy_to_user(buf,kern_buf, sizeof(struct msg_st));
       memset(kern_buf, '\0',sizeof(struct msg_st));
       spin_unlock(&my_lock);

       return ret;
}

static int ch3_mod_201912698_write(struct msg_st *buf){
	int ret;
	struct msg_list *mlTmp;

	spin_lock(&my_lock);
	ret = copy_from_user(kern_buf, buf, sizeof(struct msg_list));
	mlTmp = (struct msg_list*)kmalloc(sizeof(struct msg_list), GFP_KERNEL);
	mlTmp->msg = *kern_buf;
	printk("ch3_mod_201912698: enter to list [%s]\n",mlTmp->msg.str);
	list_add_tail(&mlTmp->list, &mylist.list);
	spin_unlock(&my_lock);

	return ret;
}
 		
static long ch3_mod_201912698_ioctl(struct file *file, unsigned int cmd, unsigned long arg){
	struct msg_st *user_buf;
	int ret;

	user_buf = (struct msg_st*)arg;

	switch(cmd) {
		case CH3_IOCTL_READ:
			ret = ch3_mod_201912698_read(user_buf);
			printk("ch3_mod_201912698: ioctl read return %d\n",ret);
			break;
		case CH3_IOCTL_WRITE:
			ret = ch3_mod_201912698_write(user_buf);
			printk("ch3_mod_201912698: ioctl write return %d\n",ret);
			break;
	}
	return 0;
}

static int ch3_mod_201912698_open(struct inode *inode, struct file *file){
	printk("ch3_mod_201912698: open\n");
	return 0;
}

static int ch3_mod_201912698_release(struct inode *inode, struct file *file){
	printk("ch3_mod_201912698: release\n");
	return 0;
}

struct file_operations ch3_mod_201912698_fops = {
	.unlocked_ioctl = ch3_mod_201912698_ioctl,
	.open = ch3_mod_201912698_open,
	.release = ch3_mod_201912698_release
};

static dev_t dev_num;
static struct cdev *cd_cdev;

static int __init ch3_mod_201912698_init(void){
	int ret;
	printk("ch3_mod_201912698: Init Module\n");

	alloc_chrdev_region(&dev_num,0,1,DEV_NAME);
	cd_cdev = cdev_alloc();
	cdev_init(cd_cdev, &ch3_mod_201912698_fops);
	ret = cdev_add(cd_cdev,dev_num,1);

	if(ret<0){
		printk("fail to add character device \n");
		return -1;
	}

	kern_buf = (struct msg_st*)vmalloc(sizeof(struct msg_st));
	memset(kern_buf,'\0',sizeof(struct msg_st));

        INIT_LIST_HEAD(&mylist.list);

        return 0;
}

static void __exit ch3_mod_201912698_exit(void){
        struct msg_list *mlTmp = 0;
        struct list_head *mlPos = 0;
        struct list_head *mlQ =0;

        unsigned int i =0;

        printk("ch3_mod_201912698: Exit Module\n");

        list_for_each_safe(mlPos, mlQ, &mylist.list){
                mlTmp = list_entry(mlPos, struct msg_list,list);
                printk("ch3_mod_201912698: free mlPos[%d], msg[%s]\n",i,mlTmp->msg.str);
                list_del(mlPos);
                kfree(mlTmp);
                i++;
        }

	cdev_del(cd_cdev);
	unregister_chrdev_region(dev_num,1);
	vfree(kern_buf);
}

module_init(ch3_mod_201912698_init);
module_exit(ch3_mod_201912698_exit);
