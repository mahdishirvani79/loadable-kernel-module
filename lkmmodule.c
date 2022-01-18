#include <linux/init.h> // For module init and exit
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h> // For fops
#include <linux/uaccess.h>
#include <linux/string.h>
#include <linux/kthread.h>
//#include <linux/slab.h>
//#include <string.h> // Can't use it!

#define DEVICE_NAME "iut_device"
MODULE_LICENSE("GPL");
spinlock_t lock;

// FILE OPERATIONS
static int iut_open(struct inode *, struct file *);
static int iut_release(struct inode *, struct file *);
static ssize_t iut_read(struct file *, char *, size_t, loff_t *);
static ssize_t iut_write(struct file *,const char *, unsigned long, loff_t *);

static struct file_operations fops = {
    .open = iut_open,
    .read = iut_read,
    .write = iut_write,
    .release = iut_release,
    
};

long long accounts[100];

void reverse(char str[], int length)
{
    int start = 0;
    int end = length - 1;
    while (start < end)
    {
        swap(*(str + start), *(str + end));
        start++;
        end--;
    }
}

// Implementation of itoa()
char *my_itoa(int num, char *str, int base)
{
    int i = 0;
    bool isNegative = false;

    // Handle 0 explicitly, otherwise empty string is printed for 0 //
    if (num == 0)
    {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }

    // In standard itoa(), negative numbers are handled only with
    // base 10. Otherwise numbers are considered unsigned.
    if (num < 0 && base == 10)
    {
        isNegative = true;
        num = -num;
    }

    // Process individual digits
    while (num != 0)
    {
        int rem = num % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num = num / base;
    }

    // If number is negative, append '-'
    if (isNegative)
        str[i++] = '-';

    str[i] = '\0'; // Append string terminator

    // Reverse the string
    reverse(str, i);

    return str;
}

int my_atoi(char* str)
{
    // Initialize result
    int res = 0;

    // Iterate through all characters
    // of input string and update result
    // take ASCII character of corresponding digit and
    // subtract the code from '0' to get numerical
    // value and multiply res by 10 to shuffle
    // digits left to update running total
    int i;
    for (i = 0; str[i] != '\0'; ++i)
        res = res * 10 + str[i] - '0';

    // return result.
    return res;
}



// Why "static"? --> To bound it to the current file.
static int major; // Device major number. Driver reacts to this major number.

// Event --> LOAD
static int __init iut_init(void)
{
    major = register_chrdev(0, DEVICE_NAME, &fops); // 0: dynamically assign a major number ||| name is displayed in /proc/devices ||| fops.
    if (major < 0)
    {
        printk(KERN_ALERT "iut_device load failed.\n");
        return major;
    }
    printk(KERN_INFO "iut_device module loaded: %d\n", major);
    return 0;
}

// Event --> UNLOAD
static void __exit iut_exit(void)
{
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "iut_device module unloaded.\n");
}

// Event --> OPEN
static int iut_open(struct inode *inodep, struct file *filep)
{
    printk(KERN_INFO "device opened to initiate accounts.\n");
    int i;
    for (i = 0; i < 100; i++)
    {
        accounts[i] = 2000000;
    }
    return 0;
}

// Event --> CLOSE
static int iut_release(struct inode *inodep, struct file *filep)
{
    printk(KERN_INFO "device closed.\n");
    return 0;
}

// Event --> READ
static ssize_t iut_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
    char message[1000];
    char message2[100];
    char splitter[2] = ","; 
    my_itoa(accounts[0], message2, 10);
    strcat(message, message2);
    strcat(message, splitter);
    int i;
    for (i = 1; i < 100; i++)
    {
        char message3[100];
        my_itoa(accounts[i], message3, 10);
        strcat(message, message3);
        strcat(message, splitter);
    }
    int errors = 0;
    errors = copy_to_user(buffer, message, strlen(message));
    return errors == 0 ? strlen(message) : -EFAULT;
}

static ssize_t iut_write(struct file *filep,const char *buffer, unsigned long len, loff_t *offset)
{
    printk("write is started\n", buffer);
    //printk(buffer);
    char a[3][10];
    int errors;
    char *command;
    //sprintf(command, "%s", buffer);
    //strcpy(command, buffer);
    char* token;
    int i;
    copy_from_user(command, buffer, strlen(buffer));
    
    //printk(KERN_INFO "22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222\n");
    while((token = strsep(&command, ",")) != NULL)
    {
        strcpy(a[i], token);
        i++;
    }
    //printk(KERN_INFO "3333333333333\n");
	//char* token = strsep(command, ",");
	//int i = 0;
	//while (token != NULL) 
    //{
	//	strcpy(a[i], token);
	//	i++;
	//	//printf("%s\n" , token );
	//	token = strsep(NULL, ",");
    //}
    
    spin_lock(&lock);
    if (strcmp(a[0],"e"))
    {
	if(accounts[my_atoi(a[1])] < my_atoi(a[3]))
		{printk(KERN_INFO "the transmitter account doesnt have this amount of money\n" );}
	else {
		accounts[my_atoi(a[1])] -= my_atoi(a[3]);
		accounts[my_atoi(a[2])] += my_atoi(a[3]);
		printk(KERN_INFO "trasnfer done\n");
		}
	}
    if (strcmp(a[0],"v")){
        accounts[my_atoi(a[2])] += my_atoi(a[3]);
        printk(KERN_INFO "money was deposited\n");
        }
    if (strcmp(a[0],"b")){
        if(accounts[my_atoi(a[1])] < my_atoi(a[3]))
            {printk(KERN_INFO "the transmitter account doesnt have this amount of money\n" );}
        else 
            {
            accounts[my_atoi(a[1])] -= my_atoi(a[3]);
            printk(KERN_INFO "withdrawal was made\n"); 
        }
    
    }
     if (strcmp(a[0],"r"))
     {
        printk(KERN_INFO "initiatin...\n");
        int i;
        for (i = 0; i < 100; i++)
        {
            accounts[i] = 2000000;
        }
     }
     //return errors == 0 ? 0 : -EFAULT;
    spin_unlock(&lock);
 
    return len;
}

// Registering load and unload functions.
module_init(iut_init);
module_exit(iut_exit);
