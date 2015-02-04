#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/random.h>
#include <asm/uaccess.h>

/* 
*  Generates a random unsigned char value
*/ 
unsigned char get_random_byte(int max) 
{
         unsigned char c;
         get_random_bytes(&c, 1);
         return c%max;
}

static ssize_t dice_read(struct file * file, char * buf, size_t count, loff_t *ppos)
{	
	char v_byte  = get_random_byte(6)+1;

	int len = sizeof(v_byte); /* Don't include the null byte. */

    /*
     * We only support reading the whole string at once.
     */
   // if (count < len)
    //        return -EINVAL;
    /*
     * If file position is non-zero, then assume the string has
     * been read and indicate there is no more data to be read.
     */
    if (*ppos != 0)
            return 0;
    /*
     * Besides copying the string to the user provided buffer,
     * this function also checks that the user has permission to
     * write to the buffer, that it is mapped, etc.
     */
    if (copy_to_user(buf, &v_byte, 1))
            return -EINVAL;
    /*
     * Tell the user how much data we wrote.
     */
    *ppos = len;

    return len;
}


/*
 * The only file operation we care about is read.
 */

static const struct file_operations dice_fops = {
        .owner          = THIS_MODULE,
        .read           = dice_read,
};

static struct miscdevice dice_dev = {
        /*
         * We don't care what minor number we end up with, so tell the
         * kernel to just pick one.
         */
        MISC_DYNAMIC_MINOR,
        /*
         * Name ourselves /dev/dice.
         */
        "dice",
        /*
         * What functions to call when a program performs file
         * operations on the device.
         */
        &dice_fops
};

static int __init dice_init(void)
{
        int ret;

        /*
         * Create the "dice" device in the /sys/class/misc directory.
         * Udev will automatically create the /dev/dice device using
         * the default rules.
         */
        ret = misc_register(&dice_dev);
        if (ret)
                printk(KERN_ERR
                       "Unable to register dice device\n");

        return ret;
}

module_init(dice_init);

static void __exit dice_exit(void)
{
        misc_deregister(&dice_dev);
}

module_exit(dice_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dan Mahoney <dpm25@pitt.edu>");
MODULE_DESCRIPTION("Dice Dev Module");
MODULE_VERSION("dev");

