#include <linux/init.h>
#include <linux/module.h>

static int my_id = 0;

static int __init mod1_init(void)
{
	printk(KERN_NOTICE "mod1 start\n");
	return 0;
}

static void __exit mod1_exit(void)
{
	printk(KERN_NOTICE "mod1 end\n");
}

static int get_my_id(void)
{
	return my_id;
}

static int set_my_id(int id){
	my_id=id;
	if(my_id==0){
		return 0;
	}
	return 1;
}
EXPORT_SYMBOL(get_my_id);
EXPORT_SYMBOL(set_my_id);

module_init(mod1_init);
module_exit(mod1_exit);
