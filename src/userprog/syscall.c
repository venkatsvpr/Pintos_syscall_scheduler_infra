#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

/* Syscall declarations */
size_t  write(int fd, const void *buf, size_t count);
void exit(int);

/* Other function declarations */
static void syscall_handler (struct intr_frame *);
void get_arguments_from_stack (struct intr_frame *f, int *arg, int n);
int add_file (struct file *f);
struct file *get_file_from_fd (int fd);
int close_file (int fd);


/* lock */
struct lock file_system_lock;
struct file_entry
{
	int fd;
	struct file *file;
	struct list_elem elem;
};

void
syscall_init (void) 
{
	lock_init(&file_system_lock);
	intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
	int syscall_code = *(int*) f->esp;
	int arg[20];

	printf ("\r\n");
	hex_dump((uintptr_t)f->esp, (f->esp),150, true);
	//printf("In SysCall Handler Code : %d\n", code);
	//printf ("Pointers *esp  esp %x \r\n", f->esp);
	switch (syscall_code) {
		case SYS_HALT:
			system_halt ();
			break;

		case SYS_EXIT:
			get_arguments_from_stack(f, &arg,1);
			exit(arg[0]);
			break;

		case SYS_EXEC:
			break;

		case SYS_WAIT:
			break;

		case SYS_CREATE:
			break;

		case SYS_REMOVE:
			break;

		case SYS_OPEN:
			break;

		case SYS_FILESIZE:
			break;

		case SYS_READ:
			break;

		case SYS_WRITE:
			get_arguments_from_stack(f,&arg,3);
			f->eax  = write ((int)arg[0],(const void *)arg[1],(size_t)arg[2]);
			break;

		case SYS_SEEK:
			break;

		case SYS_TELL:
			break;

		case SYS_CLOSE:
			break;
			
		default:
			/* To be implemented */
			break;
	}
}


void get_arguments_from_stack (struct intr_frame *f, int *arg, int n)
{
	for (int i = 0; i < n; i++)
    {
		arg[i] = *((int *)(f->esp) + i + 1);
    }
}

/* We have tried to mimic all the syscalls similar to Linux. */
/* SYS_HALT */
void system_halt(void)
{
	shutdown_power_off();
	return;
}


/* SYS_EXIT */
void exit(int status)
{
	/* VENKAT - Feel more code is required here */ 
	printf ("%s: exit(%d)\n", thread_current()->name, status);
	thread_exit();
}

size_t 
write(int fd, const void *buf, size_t count)
{
	if (fd == 1)
	{
		putbuf (buf,count);
		return count;
	}

	lock_acquire(&file_system_lock);
	putbuf (buf,count);
	lock_release(&file_system_lock);

	return count; 
}

int add_file (struct file *f)
{
	/* malloc the entry 
	   populate it
	   update the pointers
	   push it to the list
	   return the ffd 
	  */

}

struct file *
get_file_from_fd (int fd)
{
	
	/* loop through the list entry o the threaadd
		if some list entry matches return the file descriptor
		*/
}

int close_file (int fd)
{
	/* wiht fd loop through the thread
	find the entry 
	remove the file form the list and close
	*/
}