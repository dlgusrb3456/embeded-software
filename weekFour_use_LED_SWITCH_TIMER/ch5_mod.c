#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/timer.h>

MODULE_LICENSE("GPL");

#define SWITCH 12
#define LED1 5

struct my_timer_info {
	struct timer_list timer;
	long delay_jiffies;
	int data;
};

static struct my_timer_info my_timer;

static void my_timer_func(struct timer_list *t){
	struct my_timer_info *info = from_timer(info,t,timer);
	int ret = 0;
	ret = gpio_get_value(SWITCH);
	if(ret){
		printk("switch is On\n"); //turn on LED
		gpio_set_value(LED1,1);
	}
	else{
		printk("switch is Off\n");
		gpio_set_value(LED1,0);
	}

	printk("ch5_mod_timer: Jiffies %ld, Data %d \n", jiffies, info->data);
	info->data++;

	mod_timer(&my_timer.timer, jiffies + info->delay_jiffies);
}


static int __init ch5_init(void){
	printk("ch5_mod : init module \n");
	
	gpio_request_one(SWITCH,GPIOF_IN,"SWITCH");
	gpio_request_one(LED1,GPIOF_OUT_INIT_LOW,"LED1");

	my_timer.delay_jiffies=msecs_to_jiffies(500);
	my_timer.data = 100;
	timer_setup(&my_timer.timer, my_timer_func,0);
	my_timer.timer.expires = jiffies + my_timer.delay_jiffies;
	add_timer(&my_timer.timer);

	return 0;
}

static void __exit ch5_exit(void){
	printk("ch5_mod : exit module \n");
	gpio_free(SWITCH);
	gpio_free(LED1);
	del_timer(&my_timer.timer);
}

module_init(ch5_init);
module_exit(ch5_exit);
