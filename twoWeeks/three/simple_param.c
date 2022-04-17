#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>

MODULE_LICENSE("GPL");

#define DEV_NAME "simple_param_dev"

#define IOCTL_START_NUM 0x80
#define IOCTL_NUM1 IOCTL_START_NUM+1
#define IOCTL_NUM2 IOCTL_START_NUM+2

#define PARAM_IOCTL_NUM 'z'
#define PARAM_GET _IOWR(PARAM_IOCTL_NUM,IOCTL_NUM1,unsigned long) // 읽기와 쓰기가 가능한 cmd 생성, (매직번호, 구분번호, 데이터 길이)
#define PARAM_SET _IOWR(PARAM_IOCTL_NUM,IOCTL_NUM2,unsigned long)

static long my_id = 0;
module_param(my_id,long,0); //parameter 값을 커널 모듈에게 전달해주기 (main 함수 실행할때 argc,argv 넘기는 것처럼 insmod할때 값을 지정할 수 있음)

static int simple_param_open(struct inode *inode, struct file *file){
	printk("simple_param: open\n");
	return 0;
}

static int simple_param_release(struct inode *inode,struct file *file){
	printk("simple_param: release\n");
	return 0;
}

static long simple_param_ioctl(struct file *file, unsigned int cmd, unsigned long arg){ //cmd: 수향할 명령을 구분하기 위해 사용, 
											//arg: 하나만 넘길수도 있지만 여러개 넘기고 싶을때 주소를 입력해도 됨.
											//실패시: -1반환, 성공시 음이 아닌 정수 반환
	printk("simple_param_ioctl Run\n");

	switch(cmd){
		case PARAM_GET:
			printk("simple_param: return my_id %ld \n",my_id);
			return my_id;
		case PARAM_SET:
			printk("simple_param: set my_id %ld to %ld \n",my_id,(long)arg);
			my_id = (long)arg;
			return my_id;
		default:
			printk("simple_param: error");
			return -1;
	}
	return 0;
}

static dev_t dev_num;
static struct cdev *cd_cdev;

struct file_operations simple_param_fops = {		//cdev안의 ops struct 초기화.
	.open = simple_param_open,
	.release = simple_param_release,
	.unlocked_ioctl = simple_param_ioctl,
};


static int __init simple_param_init(void){
	printk("simple_param: init modules\n");
	alloc_chrdev_region(&dev_num,0,1,DEV_NAME); //dev_num에 매이져 넘버 받고, 마이너 넘버 받는 곳에 0 관습적으로 넣고, 
						    //마이너 넘버 몇개 받을건지 1개 넣고, /proc/devices에 넣을 디바이스 네임 넣기
		`				    //Makefile에서 동적 할당된 Major넘버를 받아옴 (Module명과, awk(collumm 검색), /proc/devices등 사용)
	cd_cdev=cdev_alloc();
	cdev_init(cd_cdev,&simple_param_fops);
	cdev_add(cd_cdev,dev_num,1);		    //cdev_add를 통해 Device Drivers에 등록
	return 0;
}

static void __exit simple_param_exit(void){
	printk("simple_param: exit module\n");

	cdev_del(cd_cdev);
	unregister_chrdev_region(dev_num,1);
}

module_init(simple_param_init);
module_exit(simple_param_exit);

